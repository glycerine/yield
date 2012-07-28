// socket_test.hpp

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

#ifndef _YIELD_SOCKETS_SOCKET_TEST_HPP_
#define _YIELD_SOCKETS_SOCKET_TEST_HPP_

#include "../channel_test.hpp"
#include "yield/auto_object.hpp"
#include "yield/sockets/socket_address.hpp"
#include "yield/sockets/stream_socket.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace sockets {
template <class TypeParam>
class SocketTest : public ::testing::Test {
};

TYPED_TEST_CASE_P(SocketTest);

//template <class TypeParam>
//class SocketBindTest : public ::testing::Test {
//public:
//  // ::testing::Test
//  void run() {
//    if (!TypeParam().bind(31000))
//      throw Exception();
//  }
//};

TYPED_TEST_P(SocketTest, getfqdn) {
  string hostname = Socket::gethostname();
  string fqdn = Socket::getfqdn();
  ASSERT_FALSE(fqdn.empty());
  ASSERT_EQ(fqdn.find(hostname), 0);
  ASSERT_GE(fqdn.size(), hostname.size());
}

TYPED_TEST_P(SocketTest, gethostname) {
  string hostname = Socket::gethostname();
  ASSERT_FALSE(hostname.empty());
  ASSERT_NE(hostname, "localhost");
}

TYPED_TEST_P(SocketTest, getpeername) {
  TypeParam().first().getpeername();
}

TYPED_TEST_P(SocketTest, getsockname) {
  TypeParam().first().getsockname();
}

TYPED_TEST_P(SocketTest, MessageFlags) {
  Socket::MessageFlags dontroute(Socket::MessageFlags::DONTROUTE);
  Socket::MessageFlags oob(Socket::MessageFlags::OOB);
  Socket::MessageFlags peek(Socket::MessageFlags::PEEK);
}

TYPED_TEST_P(SocketTest, set_blocking_mode) {
  TypeParam sockets;

  if (!sockets.first().set_blocking_mode(true)) {
    throw Exception();
  }

  if (!sockets.first().set_blocking_mode(false)) {
    throw Exception();
  }

  if (!sockets.first().set_blocking_mode(true)) {
    throw Exception();
  }
}

//template <class TypeParam>
//class SocketSetSocketOptionTest : public ::testing::Test {
//public:
//  SocketSetSocketOptionTest(int option_name, int option_value = 1)
//    : option_name(option_name) {
//  }
//
//  // ::testing::Test
//  void run() {
//    TypeParam sockets;
//    if (!sockets.first().setsockopt(option_name, option_value)) {
//      throw Exception();
//    }
//  }
//
//private:
//  int option_name, option_value;
//};

TYPED_TEST_P(SocketTest, want_recv) {
  TypeParam sockets;
  if (!sockets.first().set_blocking_mode(false)) {
    throw Exception();
  }
  auto_Object<Buffer> buffer = new Buffer(1);
  ssize_t recv_ret = sockets.first().recv(*buffer, 0);
  ASSERT_EQ(recv_ret, -1);
  ASSERT_TRUE(sockets.first().want_recv());
}

REGISTER_TYPED_TEST_CASE_P(SocketTest, getfqdn, gethostname, getpeername, getsockname, MessageFlags, set_blocking_mode, want_recv);
}
}

#endif
