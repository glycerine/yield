// yield/sockets/client/stream_socket_client.cpp

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
#include "yield/sockets/socket_address.hpp"
#include "yield/sockets/stream_socket.hpp"
#include "yield/sockets/aio/aio_queue.hpp"
#include "yield/sockets/client/stream_socket_client.hpp"
#include "yield/uri/uri.hpp"


namespace yield {
namespace sockets {
namespace client {
using yield::uri::URI;
using yield::sockets::SocketAddress;
using yield::sockets::StreamSocket;


StreamSocketClient::StreamSocketClient(
  const Configuration& configuration,
  Log* error_log,
  Log* trace_log,
  const URI& uri
)
  : StreamSocketPeer<SocketClient>(error_log, trace_log),
    configuration(configuration),
    peername(*new SocketAddress(uri.get_host().c_str(), uri.get_port()))
{ }

StreamSocketClient::~StreamSocketClient() {
  SocketAddress::dec_ref(peername);
}

void StreamSocketClient::service(YO_NEW_REF Event& event) {
  switch (event.get_type_id()) {
  case connectAIOCB::TYPE_ID: {
    connectAIOCB& connect_aiocb = static_cast<connectAIOCB&>(event);
    Connection& connection
    = static_cast<Connection&>(connect_aiocb.get_connection());

    if (connect_aiocb.get_error() == 0) {
      if (get_trace_log() != NULL) {
        get_trace_log()->get_stream(Log::INFO) <<
                                               connection.get_log_prefix() << ": " <<
                                               connect_aiocb.get_type_name() <<
                                               "( socket=" <<
                                               static_cast<socket_t>(connect_aiocb.get_socket()) <<
                                               ", peername=" << connect_aiocb.get_peername() <<
                                               ", return=" << connect_aiocb.get_return() <<
                                               ", send_buffer=";

        if (connect_aiocb.get_send_buffer() != NULL)
          get_trace_log()->write(*connect_aiocb.get_send_buffer(), Log::INFO);
        else
          get_trace_log()->write("NULL )", Log::INFO);
      }
    } else {
      if (get_error_log() != NULL) {
        get_error_log()->get_stream(Log::ERR) <<
                                              connection.get_log_prefix() << ": " <<
                                              "error in " << connect_aiocb.get_type_name() << ":" <<
                                              Exception(connect_aiocb.get_error());
      }
    }

    connection.handle(connect_aiocb);
  }
  break;

  default:
    StreamSocketPeer<SocketClient>::service(event);
    break;
  }
}


StreamSocketClient::Configuration::Configuration
(
  uint16_t concurrency_level,
  const Time& connect_timeout,
  const Time& recv_timeout,
  const uint16_t recv_tries_max,
  const Time& send_timeout,
  const uint16_t send_tries_max
)
  : concurrency_level(concurrency_level),
    connect_timeout(connect_timeout),
    recv_timeout(recv_timeout),
    recv_tries_max(recv_tries_max),
    send_timeout(send_timeout),
    send_tries_max(send_tries_max)
{ }

void
StreamSocketClient::Configuration::
set_connect_timeout(
  const Time& connect_timeout
) {
  this->connect_timeout = connect_timeout;
}

void
StreamSocketClient::Configuration::set_recv_timeout
(
  const Time& recv_timeout
) {
  this->recv_timeout = recv_timeout;
}

void
StreamSocketClient::Configuration::set_send_timeout
(
  const Time& send_timeout
) {
  this->send_timeout = send_timeout;
}


StreamSocketClient::Connection::Connection
(
  StreamSocketClient& client,
  YO_NEW_REF StreamSocket& socket_
)
  : StreamSocketPeer<SocketClient>::Connection
  (
    client,
    client.get_peername(),
    socket_
  ),
  configuration(client.get_configuration()) {
#ifdef _WIN32
  if (!socket_.bind(SocketAddress::IN_ANY))
    throw Exception();
#endif

  if (!get_aio_queue().associate(socket_))
    throw Exception();
}

void
StreamSocketClient::Connection::enqueue
(
  YO_NEW_REF connectAIOCB& connect_aiocb
) {
  if (!get_aio_queue().enqueue(connect_aiocb)) {
    //uint32_t last_error_code = Exception::get_last_error_code();
    connectAIOCB::dec_ref(connect_aiocb);
    DebugBreak();
  }
}
}
}
}
