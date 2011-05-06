// yield/http/server/http_request_queue.cpp

// Copyright (c) 2011 Minor Gordon
// All rights reserved

// This source file is part of the Yield project.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// * Neither the name of the Yield project nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "http_request_parser.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/log.hpp"
#include "yield/fs/file.hpp"
#include "yield/http/http_message_body_chunk.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_response.hpp"
#include "yield/http/server/http_request_queue.hpp"
#include "yield/sockets/tcp_socket.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/aio_queue.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yield/sockets/aio/sendfile_aiocb.hpp"

namespace yield {
namespace http {
namespace server {
using yield::fs::File;
using yield::sockets::Socket;
using yield::sockets::SocketAddress;
using yield::sockets::TCPSocket;
using yield::sockets::aio::acceptAIOCB;
using yield::sockets::aio::AIOQueue;

class HTTPRequestQueue::Connection : public EventHandler {
public:
  class AIOCB {
  public:
    Connection& get_connection() {
      return connection;
    }

  protected:
    AIOCB(Connection& connection)
      : connection(connection.inc_ref())
    { }

    virtual ~AIOCB() {
      Connection::dec_ref(connection);
    }

  private:
    Connection& connection;
  };

public:
  class recvAIOCB
    : public yield::sockets::aio::recvAIOCB,
      public HTTPRequestQueue::Connection::AIOCB {
  public:
    recvAIOCB(Connection& connection, YO_NEW_REF Buffer& buffer)
      : yield::sockets::aio::recvAIOCB(connection.get_socket(), buffer, 0),
        HTTPRequestQueue::Connection::AIOCB(connection)
    { }
  };

public:
  class sendAIOCB
    : public yield::sockets::aio::sendAIOCB,
      public HTTPRequestQueue::Connection::AIOCB {
  public:
    sendAIOCB(Connection& connection, YO_NEW_REF Buffer& buffer)
      : yield::sockets::aio::sendAIOCB(connection.get_socket(), buffer, 0),
        HTTPRequestQueue::Connection::AIOCB(connection)
    { }
  };

public:
  class sendfileAIOCB
    : public yield::sockets::aio::sendfileAIOCB,
      public HTTPRequestQueue::Connection::AIOCB {
  public:
    sendfileAIOCB(Connection& connection, fd_t fd)
      : yield::sockets::aio::sendfileAIOCB(connection.get_socket(), fd),
        HTTPRequestQueue::Connection::AIOCB(connection)
    { }
  };

public:
  enum State { STATE_CONNECTED, STATE_ERROR };

public:
  Connection(
    AIOQueue& aio_queue,
    SocketAddress& peername,
    TCPSocket& socket_,
    Log* log = NULL
  ) : aio_queue(aio_queue.inc_ref()),
      log(Object::inc_ref(log)),
      peername(peername.inc_ref()),
      socket_(static_cast<TCPSocket&>(socket_.inc_ref()))
  {
    state = STATE_CONNECTED;
  }

  ~Connection() {
    AIOQueue::dec_ref(aio_queue);
    Log::dec_ref(log);
    TCPSocket::dec_ref(socket_);
  }

public:
  SocketAddress& get_peername() const {
    return peername;
  }

  TCPSocket& get_socket() const {
    return socket_;
  }

  State get_state() const {
    return state;
  }

public:
  void handle(YO_NEW_REF acceptAIOCB& accept_aiocb) {
    if (
      accept_aiocb.get_recv_buffer() != NULL
      &&
      accept_aiocb.get_return() > 0
    )
      parse(*accept_aiocb.get_recv_buffer());
    else {
      Buffer* recv_buffer
        = new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
      recvAIOCB* recv_aiocb = new recvAIOCB(*this, *recv_buffer);
      if (!aio_queue.enqueue(*recv_aiocb)) {
        recvAIOCB::dec_ref(*recv_aiocb);
        state = STATE_ERROR;
      }
    }

    acceptAIOCB::dec_ref(accept_aiocb);
  }

  void handle(YO_NEW_REF recvAIOCB& recv_aiocb) {
    if (recv_aiocb.get_return() > 0)
      parse(recv_aiocb.get_buffer());

    recvAIOCB::dec_ref(recv_aiocb);
  }

  void handle(YO_NEW_REF sendAIOCB& send_aiocb) {
    sendAIOCB::dec_ref(send_aiocb);
  }

  void handle(YO_NEW_REF sendfileAIOCB& sendfile_aiocb) {
    sendfileAIOCB::dec_ref(sendfile_aiocb);
  }

public:
  // yield::Object
  const char* get_type_name() const {
    return "yield::http::server::HTTPRequestQueue::Connection";
  }

  Connection& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::EventHandler
  void handle(YO_NEW_REF Event& event) {
    switch (event.get_type_id()) {
    case HTTPMessageBodyChunk::TYPE_ID: {
      handle(static_cast<HTTPMessageBodyChunk&>(event));
    }
    break;

    case HTTPResponse::TYPE_ID: {
      handle(static_cast<HTTPResponse&>(event));
    }
    break;

    default:
      debug_break();
      break;
    }
  }

private:
  void handle(YO_NEW_REF HTTPMessageBodyChunk& http_message_body_chunk) {
    Buffer* send_buffer;
    if (http_message_body_chunk.data() != NULL)
      send_buffer = &http_message_body_chunk.data()->inc_ref();
    else
      send_buffer = &Buffer::copy("0\r\n\r\n", 5);
    HTTPMessageBodyChunk::dec_ref(http_message_body_chunk);

    sendAIOCB* send_aiocb = new sendAIOCB(*this, *send_buffer);
    if (!aio_queue.enqueue(*send_aiocb)) {
      sendAIOCB::dec_ref(*send_aiocb);
      state = STATE_ERROR;
    }
  }

  void handle(YO_NEW_REF HTTPResponse& http_response) {
    if (log != NULL) {
      log->get_stream(Log::Level::DEBUG) << get_type_name() 
        << ": sending " << http_response;
    }

    http_response.finalize();
    Buffer& http_response_header = http_response.get_header().inc_ref();
    Object* http_response_body = Object::inc_ref(http_response.get_body());
    HTTPResponse::dec_ref(http_response);

    if (http_response_body != NULL) {
      switch (http_response_body->get_type_id()) {
      case Buffer::TYPE_ID: {
        http_response_header.set_next_buffer(
          static_cast<Buffer*>(http_response_body)
        );
      }
      break;

      case File::TYPE_ID: {
        sendAIOCB* send_aiocb = new sendAIOCB(*this, http_response_header);
        if (aio_queue.enqueue(*send_aiocb)) {
          sendfileAIOCB* sendfile_aiocb
            = new sendfileAIOCB(*this, *static_cast<File*>(http_response_body));
          if (aio_queue.enqueue(*sendfile_aiocb))
            return;
          else {
            sendfileAIOCB::dec_ref(*sendfile_aiocb);
            state = STATE_ERROR;
          }
        } else {
          sendAIOCB::dec_ref(*send_aiocb);
          state = STATE_ERROR;
        }
      }
      break;

      default: debug_break();
      }
    }

    sendAIOCB* send_aiocb = new sendAIOCB(*this, http_response_header);
    if (!aio_queue.enqueue(*send_aiocb)) {
      sendAIOCB::dec_ref(*send_aiocb);
      state = STATE_ERROR;
    }
  }

private:
  void parse(Buffer& recv_buffer) {
    debug_assert_false(recv_buffer.empty());

    HTTPRequestParser http_request_parser(recv_buffer, peername);

    for (;;) {
      Object& object = http_request_parser.parse();

      switch (object.get_type_id()) {
      case Buffer::TYPE_ID: {
        Buffer& next_recv_buffer = static_cast<Buffer&>(object);
        recvAIOCB* recv_aiocb = new recvAIOCB(*this, next_recv_buffer);
        if (!aio_queue.enqueue(*recv_aiocb)) {
          recvAIOCB::dec_ref(*recv_aiocb);
          state = STATE_ERROR;
        }
      }
      return;

      case HTTPRequest::TYPE_ID: {
        HTTPRequest& http_request = static_cast<HTTPRequest&>(object);

        if (log != NULL) {
          log->get_stream(Log::Level::DEBUG) << get_type_name() 
            << ": parsed " << http_request;
        }

        http_request.set_response_handler(*this);
        aio_queue.enqueue(http_request);
      }
      break;

      case HTTPResponse::TYPE_ID: {
        HTTPResponse& http_response = static_cast<HTTPResponse&>(object);
        debug_assert_eq(http_response.get_status_code(), 400);

        if (log != NULL) {
          log->get_stream(Log::Level::DEBUG) << get_type_name() 
            << ": parsed " << http_response;
        }

        handle(http_response);
        return;
      }
      break;

      default:
        debug_break();
        break;
      }
    }
  }

private:
  AIOQueue& aio_queue;
  Log* log;
  SocketAddress& peername;
  TCPSocket& socket_;
  State state;
};


HTTPRequestQueue::HTTPRequestQueue(
  const SocketAddress& sockname,
  YO_NEW_REF Log* log
) : aio_queue(*new AIOQueue(log)),
    log(Object::inc_ref(log)),
    socket_(*new TCPSocket(sockname.get_family())) {
  if (aio_queue.associate(socket_)) {
#ifdef __linux__
    if (socket_.setsockopt(Socket::Option::REUSEADDR, true)) {
#endif
      if (socket_.bind(sockname)) {
        if (socket_.listen()) {
          Buffer* recv_buffer
            = new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
          acceptAIOCB* accept_aiocb = new acceptAIOCB(socket_, recv_buffer);
          if (aio_queue.enqueue(*accept_aiocb))
            return;
        }
      }
#ifdef __linux__
    }
#endif
  }

  throw Exception();
}

HTTPRequestQueue::~HTTPRequestQueue() {
  for (
    vector<Connection*>::iterator connection_i = connections.begin();
    connection_i != connections.end();
    ++connection_i
  ) {
    Connection* connection = *connection_i;
    TCPSocket& socket_ = connection->get_socket();
    socket_.set_blocking_mode(true);
    socket_.setsockopt(TCPSocket::Option::LINGER, 30);
    socket_.shutdown();
    socket_.close();
    Connection::dec_ref(*connection);
  }

  socket_.close();

  AIOQueue::dec_ref(aio_queue);
  Log::dec_ref(log);
  TCPSocket::dec_ref(socket_);
}

YO_NEW_REF Event* HTTPRequestQueue::dequeue(const Time& timeout) {
  Time timeout_remaining(timeout);

  for (;;) {
    Time start_time = Time::now();

    Event* event = aio_queue.dequeue(timeout_remaining);

    if (event != NULL) {
      switch (event->get_type_id()) {
      case acceptAIOCB::TYPE_ID: {
        handle(static_cast<acceptAIOCB&>(*event));
      }
      break;

      case Connection::recvAIOCB::TYPE_ID: {
        handle<Connection::recvAIOCB>(
          static_cast<Connection::recvAIOCB&>(*event)
        );
      }
      break;

      case Connection::sendAIOCB::TYPE_ID: {
        handle<Connection::sendAIOCB>(
          static_cast<Connection::sendAIOCB&>(*event)
        );
      }
      break;

      case Connection::sendfileAIOCB::TYPE_ID: {
        handle<Connection::sendfileAIOCB>(
          static_cast<Connection::sendfileAIOCB&>(*event)
        );
      }
      break;

      default: return event;
      }
    }

    Time elapsed_time = Time::now() - start_time;
    if (timeout_remaining > elapsed_time)
      timeout_remaining -= elapsed_time;
    else
      return NULL;
  }
}

bool HTTPRequestQueue::enqueue(YO_NEW_REF Event& event) {
  return aio_queue.enqueue(event);
}

void HTTPRequestQueue::handle(YO_NEW_REF acceptAIOCB& accept_aiocb) {
  if (accept_aiocb.get_return() >= 0) {
    TCPSocket& accepted_socket
      = static_cast<TCPSocket&>(*accept_aiocb.get_accepted_socket());

    if (aio_queue.associate(accepted_socket)) {
      Connection* connection
        = new Connection(
                  aio_queue,
                  *accept_aiocb.get_peername(),
                  static_cast<TCPSocket&>(accepted_socket),
                  log                  
                );

      connection->handle(accept_aiocb);

      if (connection->get_state() == Connection::STATE_CONNECTED)
        connections.push_back(connection);
      else
        Connection::dec_ref(*connection);
    } else {
      accepted_socket.shutdown();
      accepted_socket.close();
      TCPSocket::dec_ref(accepted_socket);
    }
  } else {
    acceptAIOCB::dec_ref(accept_aiocb);
  }

  Buffer* recv_buffer
    = new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
  acceptAIOCB* next_accept_aiocb = new acceptAIOCB(socket_, recv_buffer);
  if (!aio_queue.enqueue(*next_accept_aiocb))
    acceptAIOCB::dec_ref(next_accept_aiocb);
}

template <class AIOCBType>
void HTTPRequestQueue::handle(YO_NEW_REF AIOCBType& aiocb) {
  Connection& connection = aiocb.get_connection();
  if (connection.get_state() == Connection::STATE_CONNECTED) {
    connection.handle(aiocb);

    if (connection.get_state() == Connection::STATE_ERROR) {
      for (
        vector<Connection*>::iterator connection_i = connections.begin();
        connection_i != connections.end();
        ++connection_i
      ) {
        if (*connection_i == &connection) {
          connections.erase(connection_i);
          connection.get_socket().close();
          Connection::dec_ref(connection);
        }
      }
    }
  }
  else
    AIOCBType::dec_ref(aiocb);
}
}
}
}
