// yield/sockets/socket_address_test.cpp

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
#include "yield/auto_object.hpp"
#include "yield/net/sockets/socket_address.hpp"
#include "yunit.hpp"


#define TEST_NODENAME "localhost"
#define TEST_SERVNAME static_cast<uint16_t>( 80 )


TEST_SUITE(SocketAddress);

namespace yield {
namespace net {
namespace sockets {
TEST(SocketAddress, copy_constructor) {
  SocketAddress sockaddr1(TEST_NODENAME, TEST_SERVNAME);
  SocketAddress sockaddr2(sockaddr1);
  throw_assert_eq(sockaddr1, sockaddr2);
}

TEST(SocketAddress, getnameinfo) {
  SocketAddress sockaddr(TEST_NODENAME, TEST_SERVNAME);

  string hostname;
  if (!sockaddr.getnameinfo(hostname, false))
    throw Exception();
  throw_assert_false(hostname.empty());
  // hostname may not == TEST_NODENAME if the address was resolved differently

  string hostname_numeric;
  if (!sockaddr.getnameinfo(hostname_numeric, true))
    throw Exception();
  throw_assert
  (
    hostname_numeric.find('.') != string::npos
    ||
    hostname_numeric.find(':') != string::npos
  );
}

TEST(SocketAddress, operator_string) {
  SocketAddress sockaddr(TEST_NODENAME, TEST_SERVNAME);
  string sockaddr_string = sockaddr;
  throw_assert_false(sockaddr_string.empty());
}

TEST(SocketAddress, resolving_constructor) {
  SocketAddress(TEST_NODENAME, 80);
  SocketAddress(TEST_NODENAME, "80");
  SocketAddress(TEST_NODENAME, TEST_SERVNAME);
}
}
}
}
