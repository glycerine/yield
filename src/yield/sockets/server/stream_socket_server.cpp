// yield/sockets/server/stream_socket_server.cpp

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
#include "yield/buffer.hpp"
#include "yield/exception.hpp"
#include "yield/log.hpp"
#include "yield/page.hpp"
#include "yield/sockets/aio/accept_aiocb.hpp"
#include "yield/sockets/aio/aio_queue.hpp"
#include "yield/sockets/server/stream_socket_server.hpp"
#include "yield/sockets/stream_socket.hpp"


namespace yield {
namespace sockets {
namespace server {
using yield::sockets::aio::acceptAIOCB;

StreamSocketServer::StreamSocketServer(
  Log* error_log,
  YO_NEW_REF StreamSocket& socket_,
  const SocketAddress& sockname,
  Log* trace_log
)
  : StreamSocketPeer<SocketServer>(error_log, trace_log),
    socket_(socket_) {
  debug_assert_eq(sockname.get_family(), socket_.get_domain());

  if
  (
    socket_.bind(sockname)
    &&
    socket_.listen()
    &&
    get_aio_queue().associate(socket_)
  ) {
    acceptAIOCB* accept_aiocb = new acceptAIOCB(socket_, new Page);
    enqueue(*accept_aiocb);
  } else
    throw Exception();
}

StreamSocketServer::~StreamSocketServer() {
  StreamSocket::dec_ref(socket_);
}

void StreamSocketServer::enqueue(YO_NEW_REF acceptAIOCB& accept_aiocb) {
  if (!get_aio_queue().enqueue(accept_aiocb)) {
    acceptAIOCB::dec_ref(accept_aiocb);
    DebugBreak();
  }
}

void StreamSocketServer::service(YO_NEW_REF Event& event) {
  switch (event.get_type_id()) {
  case acceptAIOCB::TYPE_ID: {
    acceptAIOCB& accept_aiocb = static_cast<acceptAIOCB&>(event);

    if (accept_aiocb.get_error() == 0) {
      StreamSocket& accepted_socket = *accept_aiocb.get_accepted_socket();

      if (get_aio_queue().associate(accepted_socket)) {
        Connection& connection
        = create_connection(accept_aiocb.get_peername(), accepted_socket);

        if (get_trace_log() != NULL) {
          Buffer* recv_buffer = accept_aiocb.get_recv_buffer();
          if (recv_buffer != NULL) {
            get_trace_log()->get_stream(Log::INFO) <<
                                                   connection.get_log_prefix() <<
                                                   ": accepted connection from " <<
                                                   accept_aiocb.get_peername() << " with data ";
            get_trace_log()->write(*recv_buffer, Log::DEBUG);
          } else {
            get_trace_log()->get_stream(Log::INFO) <<
                                                   connection.get_log_prefix() <<
                                                   ": accepted connection from " <<
                                                   accept_aiocb.get_peername();
          }
        }

        connection.handle(accept_aiocb);
      } else
        DebugBreak();
    } else
      DebugBreak();

    acceptAIOCB* next_accept_aiocb
    = new acceptAIOCB(get_socket(), new Page);

    enqueue(*next_accept_aiocb);
  }
  break;

  default:
    StreamSocketPeer<SocketServer>::service(event);
    break;
  }
}


StreamSocketServer::Connection::Connection
(
  StreamSocketServer& server,
  YO_NEW_REF SocketAddress& peername,
  StreamSocket& socket_
)
  : StreamSocketPeer<SocketServer>::Connection(server, peername, socket_)
{ }
}
}
}
