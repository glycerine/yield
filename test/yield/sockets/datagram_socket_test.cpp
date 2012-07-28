// datagram_socket_test.cpp

// Copyright (c) 2012 Minor Gordon
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
#include "yield/auto_object.hpp"
#include "yield/sockets/datagram_socket_pair.hpp"

namespace yield {
namespace sockets {
INSTANTIATE_TYPED_TEST_CASE_P(DatagramSocket, ChannelTest, DatagramSocketPair);
INSTANTIATE_TYPED_TEST_CASE_P(DatagramSocket, SocketTest, DatagramSocketPair);

TEST(DatagramSocket, recvfrom) {
  DatagramSocketPair sockets;
  sockets.first().write("m", 1);
  char m;
  SocketAddress peername;
  ssize_t recvfrom_ret = sockets.second().recvfrom(&m, 1, 0, peername);
  if (recvfrom_ret == -1) {
    throw Exception();
  }
  ASSERT_EQ(recvfrom_ret, 1);
  ASSERT_EQ(m, 'm');
  ASSERT_EQ(peername, *sockets.first().getsockname());
  ASSERT_EQ(peername, *sockets.second().getpeername());
}

TEST(DatagramSocket, recvmsg) {
  DatagramSocketPair sockets;
  sockets.first().write("mn", 2);
  char m, n;
  iovec iov[2];
  iov[0].iov_base = &m;
  iov[0].iov_len = 1;
  iov[1].iov_base = &n;
  iov[1].iov_len = 1;
  SocketAddress peername;
  ssize_t recvmsg_ret = sockets.second().recvmsg(iov, 2, 0, peername);
  if (recvmsg_ret == -1) {
    throw Exception();
  }
  ASSERT_EQ(recvmsg_ret, 2);
  ASSERT_EQ(m, 'm');
  ASSERT_EQ(n, 'n');
  ASSERT_EQ(peername, *sockets.first().getsockname());
  ASSERT_EQ(peername, *sockets.second().getpeername());
}

TEST(DatagramSocket, sendmsg) {
  DatagramSocketPair sockets;
  iovec iov[2];
  iov[0].iov_base = const_cast<char*>("m");
  iov[0].iov_len = 1;
  iov[1].iov_base = const_cast<char*>("n");
  iov[1].iov_len = 1;
  ssize_t sendmsg_ret =
    sockets.first().sendmsg(iov, 2, 0, *sockets.second().getsockname());
  if (sendmsg_ret == -1) {
    throw Exception();
  }
  ASSERT_EQ(sendmsg_ret, 2);
  char mn[2];
  ssize_t read_ret = sockets.second().read(mn, 2);
  ASSERT_EQ(read_ret, 2);
  ASSERT_EQ(mn[0], 'm');
  ASSERT_EQ(mn[1], 'n');
}

TEST(DatagramSocket, sendto) {
  DatagramSocketPair sockets;
  ssize_t sendto_ret
  = sockets.first().sendto("m", 1, 0, *sockets.second().getsockname());
  if (sendto_ret == -1) {
    throw Exception();
  }
  ASSERT_EQ(sendto_ret, 1);
  char m;
  ssize_t read_ret = sockets.second().read(&m, 1);
  ASSERT_EQ(read_ret, 1);
  ASSERT_EQ(m, 'm');
}
}
}
