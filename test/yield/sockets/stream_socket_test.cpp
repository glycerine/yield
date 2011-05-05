// stream_socket_test.cpp

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

#include "socket_test.hpp"
#include "yield/sockets/stream_socket_pair.hpp"
#include "yield/sockets/tcp_socket.hpp"

TEST_SUITE_EX(
  StreamSocket,
  yield::sockets::SocketTestSuite<yield::sockets::StreamSocketPair>
);

namespace yield {
namespace sockets {
TEST(StreamSocket, accept) {
  StreamSocket client_stream_socket(TCPSocket::DOMAIN_DEFAULT),
               listen_stream_socket(TCPSocket::DOMAIN_DEFAULT);
  if (listen_stream_socket.bind(SocketAddress::IN_LOOPBACK))    
    if (listen_stream_socket.listen()) {
      if (client_stream_socket.connect(*listen_stream_socket.getsockname())) {
        auto_Object<StreamSocket> server_stream_socket
          = listen_stream_socket.accept();
        return;
      }
  }

  throw Exception();
}

TEST(StreamSocket, dup) {
  auto_Object<StreamSocket> socket_ = StreamSocketPair().first().dup();
}

TEST(StreamSocket, inc_ref) {
  auto_Object<StreamSocket> socket_ = StreamSocketPair().first().inc_ref();
}

TEST(StreamSocket, listen) {
  StreamSocket stream_socket(TCPSocket::DOMAIN_DEFAULT);

  if (!stream_socket.bind(SocketAddress::IN_LOOPBACK))
    throw Exception();

  if (!stream_socket.listen())
    throw Exception();
}

TEST(StreamSocket, setsockopt_KEEPALIVE) {
  if (!StreamSocketPair().first().setsockopt(StreamSocket::Option::KEEPALIVE, true))
    throw Exception();
}

TEST(StreamSocket, setsockopt_LINGER) {
  StreamSocketPair stream_sockets;

  if (!stream_sockets.first().setsockopt(StreamSocket::Option::LINGER, 1))
    throw Exception();

  if (!stream_sockets.first().setsockopt(StreamSocket::Option::LINGER, 30))
    throw Exception();

  if (!stream_sockets.first().setsockopt(StreamSocket::Option::LINGER, false))
    throw Exception();
}
}
}
