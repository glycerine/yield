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

#include "yield/auto_object.hpp"
#include "yield/sockets/tcp_socket.hpp"
#include "yunit.hpp"

TEST_SUITE(StreamSocket);

namespace yield {
namespace sockets {
TEST(StreamSocket, constructor) {
  StreamSocket socket_(TCPSocket::DOMAIN_DEFAULT);
}

TEST(StreamSocket, create) {
  auto_Object<StreamSocket> socket_
    = StreamSocket::create(TCPSocket::DOMAIN_DEFAULT);
}

TEST(StreamSocket, dup) {
  auto_Object<StreamSocket> socket_
    = StreamSocket(TCPSocket::DOMAIN_DEFAULT).dup();
}

TEST(StreamSocket, inc_ref) {
  auto_Object<StreamSocket> socket_
    = StreamSocket(TCPSocket::DOMAIN_DEFAULT).inc_ref();
}

TEST(StreamSocket, listen) {
  StreamSocket socket_(TCPSocket::DOMAIN_DEFAULT);
  if (!socket_.bind(31000))
    throw Exception();
  if (!socket_.listen())
    throw Exception();
}
}
}
