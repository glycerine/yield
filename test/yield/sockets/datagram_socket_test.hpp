// datagram_socket_test.hpp

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

#ifndef _YIELD_SOCKETS_DATAGRAM_SOCKET_TEST_HPP_
#define _YIELD_SOCKETS_DATAGRAM_SOCKET_TEST_HPP_

#include "socket_test.hpp"
#include "yield/sockets/datagram_socket.hpp"

namespace yield {
namespace sockets {
template <class SocketType>
class DatagramSocketRecvFromTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    SocketPair sockets(SocketType::DOMAIN_DEFAULT, SocketType::TYPE);
    sockets.first().write("m", 1);
    char m;
    SocketAddress peername;
    ssize_t recvfrom_ret
      = static_cast<DatagramSocket&>(sockets.second())
          .recvfrom(&m, 1, 0, peername);
    throw_assert_eq(recvfrom_ret, 1);
    throw_assert_eq(m, 'm');
    throw_assert_eq(peername, *sockets.first().getsockname());
    throw_assert_eq(peername, *sockets.second().getpeername());
  }
};


template <class DatagramSocketType>
class DatagramSocketTestSuite : public SocketTestSuite<DatagramSocketType> {
public:
  DatagramSocketTestSuite() {
    add(
      "DatagramSocket::recvfrom",
      new DatagramSocketRecvFromTest<DatagramSocketType>
    );
  }
};
}
}

#endif
