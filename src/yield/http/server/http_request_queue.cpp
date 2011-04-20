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

#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/log.hpp"
#include "yield/http/http_message_body_chunk.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_request_parser.hpp"
#include "yield/http/http_response.hpp"
#include "yield/http/server/http_request_queue.hpp"
#include "yield/sockets/tcp_socket.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/aio_queue.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"

namespace yield {
namespace http {
namespace server {
using yield::sockets::SocketAddress;
using yield::sockets::StreamSocket;
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
  Connection(
    AIOQueue& aio_queue,
    SocketAddress& peername,
    TCPSocket& socket_,
    Log* error_log = NULL,
    Log* trace_log = NULL
  ) : aio_queue(aio_queue.inc_ref()),
      error_log(Object::inc_ref(error_log)),      
      peername(peername.inc_ref()),
      socket_(static_cast<TCPSocket&>(socket_.inc_ref())),
      trace_log(Object::inc_ref(trace_log))
  { }

  ~Connection() {
    close();

    AIOQueue::dec_ref(aio_queue);
    Log::dec_ref(error_log);
    StreamSocket::dec_ref(socket_);
    Log::dec_ref(trace_log);
  }

public:
  void close() {
    get_socket().shutdown();
    get_socket().close();
  }

  SocketAddress& get_peername() const {
    return peername;
  }

  TCPSocket& get_socket() const {
    return socket_;
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
      enqueue(
        *new recvAIOCB(
          *this,
          *new Buffer(Buffer::getpagesize(), Buffer::getpagesize())
        )
      );
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
      DebugBreak();
      break;
    }
  }

private:
  void enqueue(YO_NEW_REF recvAIOCB& recv_aiocb) {
    if (!aio_queue.enqueue(recv_aiocb)) {
      recvAIOCB::dec_ref(recv_aiocb);
      Connection::dec_ref(*this);
    }
  }

  void enqueue(YO_NEW_REF sendAIOCB& send_aiocb) {
    if (!aio_queue.enqueue(send_aiocb)) {
      sendAIOCB::dec_ref(send_aiocb);
      Connection::dec_ref(*this);
    }
  }

private:
  void handle(YO_NEW_REF HTTPMessageBodyChunk& http_message_body_chunk) {
    Buffer* send_buffer;
    if (http_message_body_chunk.data() != NULL)
      send_buffer = &http_message_body_chunk.data()->inc_ref();
    else
      send_buffer = &Buffer::copy("0\r\n\r\n", 5);

    enqueue(*new sendAIOCB(*this, *send_buffer));

    HTTPMessageBodyChunk::dec_ref(http_message_body_chunk);
  }

  void handle(YO_NEW_REF HTTPResponse& http_response) {
    sendAIOCB* send_aiocb
    = new sendAIOCB(*this, static_cast<Buffer&>(http_response).inc_ref());

    enqueue(*send_aiocb);

    HTTPResponse::dec_ref(http_response);
  }

private:
  void parse(Buffer& recv_buffer) {
    debug_assert_false(recv_buffer.empty());

    HTTPRequestParser
    http_request_parser(recv_buffer, static_cast<uint32_t>(get_socket()));

    for (;;) {
      Object& object = http_request_parser.parse();

      switch (object.get_type_id()) {
      case Buffer::TYPE_ID: {
        Buffer& next_recv_buffer = static_cast<Buffer&>(object);
        enqueue(*new recvAIOCB(*this, next_recv_buffer));
        return;
      }
      break;

      case HTTPRequest::TYPE_ID: {
        HTTPRequest& http_request = static_cast<HTTPRequest&>(object);
        if (trace_log != NULL)
          trace_log->get_stream() << get_type_name() 
            << ": parsed " << http_request;
        http_request.set_response_handler(*this);
        aio_queue.enqueue(http_request);
      }
      break;

      case HTTPResponse::TYPE_ID: {
        HTTPResponse& http_response = static_cast<HTTPResponse&>(object);
        debug_assert_eq(http_response.get_status_code(), 400);
        if (trace_log != NULL)
          trace_log->get_stream() << get_type_name() 
            << ": parsed " << http_response;
        handle(http_response);
        return;
      }
      break;

      default:
        DebugBreak();
        break;
      }
    }
  }

private:
  AIOQueue& aio_queue;
  Log* error_log;
  SocketAddress& peername;
  TCPSocket& socket_;
  Log* trace_log;
};


HTTPRequestQueue::HTTPRequestQueue(
  const SocketAddress& sockname,
  Log* error_log,
  Log* trace_log
) : aio_queue(*new AIOQueue(error_log, trace_log)),
    error_log(Object::inc_ref(error_log)),
    socket_(*new TCPSocket(sockname.get_family())),
    trace_log(Object::inc_ref(trace_log)) {

  if (aio_queue.associate(socket_)) {
    if (socket_.bind(sockname)) {
      if (socket_.listen()) {
        if (
          aio_queue.enqueue(
            *new acceptAIOCB(
                   socket_,
                   new Buffer(Buffer::getpagesize(), Buffer::getpagesize())
                 )
          )
        )
          return;
      }
    }
  }

  throw Exception();
}

HTTPRequestQueue::~HTTPRequestQueue() {
  socket_.close();

  AIOQueue::dec_ref(aio_queue);
  Log::dec_ref(error_log);
  TCPSocket::dec_ref(socket_);
  Log::dec_ref(trace_log);
}

YO_NEW_REF Event* HTTPRequestQueue::dequeue(const Time& timeout) {
  Event* event = aio_queue.dequeue(timeout);
  if (event != NULL) {
    switch (event->get_type_id()) {
    case acceptAIOCB::TYPE_ID: {
      acceptAIOCB& accept_aiocb = static_cast<acceptAIOCB&>(*event);

      if (accept_aiocb.get_return() >= 0) {
        StreamSocket& accepted_socket = *accept_aiocb.get_accepted_socket();

        if (aio_queue.associate(accepted_socket)) {
          Connection& connection
            = *new Connection(
                     aio_queue,
                     *accept_aiocb.get_peername(),
                     static_cast<TCPSocket&>(accepted_socket),
                     error_log,
                     trace_log
                   );

          connection.handle(accept_aiocb);
        } else
          DebugBreak();
      } else
        DebugBreak();

      aio_queue.enqueue(
        *new acceptAIOCB(
                socket_,
                new Buffer(Buffer::getpagesize(), Buffer::getpagesize())
              )
      );
    }
    break;

    case Connection::recvAIOCB::TYPE_ID: {
      Connection::recvAIOCB& recv_aiocb
        = static_cast<Connection::recvAIOCB&>(*event);
      Connection& connection
        = static_cast<Connection&>(recv_aiocb.get_connection());

      if (recv_aiocb.get_return() > 0)
        connection.handle(recv_aiocb);
      else {
        connection.handle(recv_aiocb);
        Connection::dec_ref(connection);
      }
    }
    break;

    case Connection::sendAIOCB::TYPE_ID: {
      Connection::sendAIOCB& send_aiocb
        = static_cast<Connection::sendAIOCB&>(*event);
      Connection& connection = send_aiocb.get_connection();

      if (send_aiocb.get_return() >= 0)
        connection.handle(send_aiocb);
      else {
        connection.handle(send_aiocb);
        Connection::dec_ref(connection);
      }
    }
    break;

    default: return event;
    }
  }

  return NULL;
}

bool HTTPRequestQueue::enqueue(YO_NEW_REF Event& event) {
  return aio_queue.enqueue(event);
}
}
}
}
