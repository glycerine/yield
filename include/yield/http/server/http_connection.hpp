// yield/http/server/http_connection.hpp

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

#ifndef _YIELD_HTTP_SERVER_HTTP_CONNECTION_HPP_
#define _YIELD_HTTP_SERVER_HTTP_CONNECTION_HPP_

#include "yield/event_handler.hpp"
#include "yield/sockets/aio/aio_queue.hpp"
#include "yield/sockets/aio/recv_aiocb.hpp"
#include "yield/sockets/aio/send_aiocb.hpp"
#include "yield/sockets/aio/sendfile_aiocb.hpp"
#include "yield/sockets/tcp_socket.hpp"

namespace yield {
namespace sockets {
namespace aio {
class acceptAIOCB;
}
}

namespace http {
class HTTPMessageBodyChunk;
class HTTPResponse;

namespace server {
class HTTPConnection : public EventHandler {
public:
  class AIOCB {
  public:
    HTTPConnection& get_connection() {
      return connection;
    }

  protected:
    AIOCB(HTTPConnection& connection)
      : connection(connection.inc_ref())
    { }

    virtual ~AIOCB() {
      HTTPConnection::dec_ref(connection);
    }

  private:
    HTTPConnection& connection;
  };

public:
  class recvAIOCB
    : public yield::sockets::aio::recvAIOCB,
      public HTTPConnection::AIOCB {
  public:
    recvAIOCB(HTTPConnection& connection, YO_NEW_REF Buffer& buffer)
      : yield::sockets::aio::recvAIOCB(connection.get_socket(), buffer, 0),
        HTTPConnection::AIOCB(connection)
    { }
  };

public:
  class sendAIOCB
    : public yield::sockets::aio::sendAIOCB,
      public HTTPConnection::AIOCB {
  public:
    sendAIOCB(HTTPConnection& connection, YO_NEW_REF Buffer& buffer)
      : yield::sockets::aio::sendAIOCB(connection.get_socket(), buffer, 0),
        HTTPConnection::AIOCB(connection)
    { }
  };

public:
  class sendfileAIOCB
    : public yield::sockets::aio::sendfileAIOCB,
      public HTTPConnection::AIOCB {
  public:
    sendfileAIOCB(HTTPConnection& connection, fd_t fd)
      : yield::sockets::aio::sendfileAIOCB(connection.get_socket(), fd),
        HTTPConnection::AIOCB(connection)
    { }
  };

public:
  enum State { STATE_CONNECTED, STATE_ERROR };

public:
  HTTPConnection(
    yield::sockets::aio::AIOQueue& aio_queue,
    EventHandler& http_request_handler,
    yield::sockets::SocketAddress& peername,
    yield::sockets::TCPSocket& socket_,
    Log* log = NULL
  );

  ~HTTPConnection();

public:
  EventHandler& get_http_request_handler() {
    return http_request_handler;
  }
  
  yield::sockets::SocketAddress& get_peername() const {
    return peername;
  }

  yield::sockets::TCPSocket& get_socket() const {
    return socket_;
  }

  State get_state() const {
    return state;
  }

public:
  void handle(YO_NEW_REF yield::sockets::aio::acceptAIOCB& accept_aiocb);
  void handle(YO_NEW_REF recvAIOCB& recv_aiocb);
  void handle(YO_NEW_REF sendAIOCB& send_aiocb);
  void handle(YO_NEW_REF sendfileAIOCB& sendfile_aiocb);

public:
  // yield::Object
  const char* get_type_name() const {
    return "yield::http::server::HTTPConnection";
  }

  HTTPConnection& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::EventHandler
  void handle(YO_NEW_REF Event& event);

private:
  void handle(YO_NEW_REF HTTPMessageBodyChunk& http_message_body_chunk);
  void handle(YO_NEW_REF HTTPResponse& http_response);

private:
  void parse(Buffer& recv_buffer);

private:
  yield::sockets::aio::AIOQueue& aio_queue;
  EventHandler& http_request_handler;
  Log* log;
  yield::sockets::SocketAddress& peername;
  yield::sockets::TCPSocket& socket_;
  State state;
};
}
}
}

#endif
