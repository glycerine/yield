// yield/http/server/http_server.cpp

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
#include "yield/http/http_request.hpp"
#include "yield/http/http_request_parser.hpp"
#include "yield/http/http_response.hpp"
#include "yield/http/server/http_server.hpp"
#include "yield/sockets/tcp_socket.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/aio_queue.hpp"

namespace yield {
namespace http {
namespace server {
using yield::sockets::SocketAddress;
using yield::sockets::StreamSocket;
using yield::sockets::TCPSocket;
using yield::sockets::aio::acceptAIOCB;
using yield::sockets::server::StreamSocketServer;

class HTTPServer::Connection : public StreamSocketServer::Connection {
public:
  Connection(
    HTTPServer& http_server,
    SocketAddress& peername,
    YO_NEW_REF StreamSocket& socket_
  )
    : StreamSocketServer::Connection(http_server, peername, socket_),
      http_request_handler(http_server.http_request_handler.inc_ref())
  { }

  // Object
  const char* get_type_name() const {
    return "yield::http::HTTPServer::Connection";
  }

  // EventHandler
  void handle(YO_NEW_REF Event& event) {
    debug_assert_eq(event.get_type_id(), HTTPResponse::TYPE_ID);

    handle(static_cast<HTTPResponse&>(event));
  }

  // StreamSocketPeer::Connection
  void handle(YO_NEW_REF recvAIOCB& recv_aiocb) {
    if (recv_aiocb.get_return() > 0)
      parse(recv_aiocb.get_buffer());

    recvAIOCB::dec_ref(recv_aiocb);
  }

  void handle(YO_NEW_REF sendAIOCB& send_aiocb) {
    sendAIOCB::dec_ref(send_aiocb);
  }

  // StreamSocketServer::Connection
  void handle(YO_NEW_REF acceptAIOCB& accept_aiocb) {
    if
    (
      accept_aiocb.get_recv_buffer() != NULL
      &&
      accept_aiocb.get_return() > 0
    )
      parse(*accept_aiocb.get_recv_buffer());

    acceptAIOCB::dec_ref(accept_aiocb);
  }

private:
  void handle(YO_NEW_REF HTTPResponse& http_response) {
    sendAIOCB* send_aiocb
      = new sendAIOCB(
        *this,
        static_cast<Buffer&>(http_response).inc_ref()
      );

    enqueue(*send_aiocb);

    HTTPResponse::dec_ref(http_response);
  }

  void parse(Buffer& recv_buffer) {
    debug_assert_false(recv_buffer.empty());

    HTTPRequestParser http_request_parser(recv_buffer);

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
        http_request.set_response_handler(*this);
        http_request_handler.handle(http_request);
      }
      break;

      case HTTPResponse::TYPE_ID: {
        HTTPResponse& http_response = static_cast<HTTPResponse&>(object);
        debug_assert_eq(http_response.get_status_code(), 400);
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
  EventHandler& http_request_handler;
};


HTTPServer::HTTPServer(
  YO_NEW_REF EventHandler& http_request_handler,
  const SocketAddress& sockname,
  Log* error_log,
  Log* trace_log
)
  : StreamSocketServer(
    error_log,
    *new TCPSocket(sockname.get_family()),
    sockname,
    trace_log
  ),
  http_request_handler(http_request_handler)
{ }

HTTPServer::~HTTPServer() {
  EventHandler::dec_ref(http_request_handler);
}

StreamSocketServer::Connection&
HTTPServer::create_connection(SocketAddress& peername, StreamSocket& socket_) {
  return *new Connection(*this, peername, socket_);
}
}
}
}
