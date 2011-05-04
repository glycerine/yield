// socket_test.hpp

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

#ifndef _YIELD_SOCKETS_SOCKET_TEST_HPP_
#define _YIELD_SOCKETS_SOCKET_TEST_HPP_

#include "../channel_test.hpp"
#include "socket_pair_factory.hpp"
#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/sockets/socket_address.hpp"
#include "yield/sockets/stream_socket.hpp"

namespace yield {
namespace sockets {
template <class SocketType>
class SocketBindTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    if (!SocketType().bind(31000))
      throw Exception();
  }
};


class SocketGetFQDNTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    string hostname = Socket::gethostname();
    string fqdn = Socket::getfqdn();
    throw_assert_false(fqdn.empty());
    throw_assert_eq(fqdn.find(hostname), 0);
    throw_assert_ge(fqdn.size(), hostname.size());
  }
};


class SocketGetHostNameTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    string hostname = Socket::gethostname();
    throw_assert_false(hostname.empty());
    throw_assert_ne(hostname, "localhost");
  }
};


template <class SocketType>
class SocketGetPeernameTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    SocketPair sockets(SocketPair::DOMAIN_DEFAULT, SocketType::TYPE);
    sockets.first().getpeername();
  }
};


template <class SocketType>
class SocketGetSocknameTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    SocketPair sockets(SocketPair::DOMAIN_DEFAULT, SocketType::TYPE);
    sockets.first().getsockname();
  }
};


template <class SocketType>
class SocketSetBlockingModeTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    SocketType socket_;

    if (!socket_.set_blocking_mode(true))
      throw Exception();

    if (!socket_.set_blocking_mode(false))
      throw Exception();
  }
};


template <class SocketType>
class SocketSetSocketOptionTest : public yunit::Test {
public:
  SocketSetSocketOptionTest(int option_name, int option_value = 1)
    : option_name(option_name) {
  }

  // yunit::Test
  void run() {
    SocketPair sockets(SocketPair::DOMAIN_DEFAULT, SocketType::TYPE);
    if (!sockets.first().setsockopt(option_name, option_value))
      throw Exception();
  }

private:
  int option_name, option_value;
};


template <class SocketType>
class SocketShutdownTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    SocketType socket_;
    if (!socket_->shutdown(true, false))
      throw Exception();
    if (!socket_->shutdown(false, true))
      throw Exception();

    if (!SocketType().shutdown(true, true))
      throw Exception();
  }
};


template <class SocketType>
class SocketTestSuite : public ChannelTestSuite {
public:
  SocketTestSuite()
    : ChannelTestSuite(
      *new SocketPairFactory(
        SocketPair::DOMAIN_DEFAULT,
        SocketType::TYPE,
        SocketPair::PROTOCOL_DEFAULT
      )
    ) {
    add("Socket::bind", new SocketBindTest<SocketType>);
    add("Socket::getfqdn", new SocketGetFQDNTest);
    add("Socket::gethostname", new SocketGetHostNameTest);
    add("Socket::getpeername", new SocketGetPeernameTest<SocketType>);
    add("Socket::getsockname", new SocketGetSocknameTest<SocketType>);
    add("Socket::set_blocking_mode",new SocketSetBlockingModeTest<SocketType>);

    add(
      "Socket::setsockopt(RCVBUF, 4096)",
      new SocketSetSocketOptionTest<SocketType>(Socket::Option::RCVBUF, 4096)
    );

    add(
      "Socket::setsockopt(REUSEADDR, 1)",
      new SocketSetSocketOptionTest<SocketType>(Socket::Option::REUSEADDR, 1)
    );

    add(
      "Socket::setsockopt(SNDBUF, 4096)",
      new SocketSetSocketOptionTest<SocketType>(Socket::Option::SNDBUF, 4096)
    );

    //add("Socket::shutdown", new SocketShutdownTest<SocketType>);
  }
};
}
}

#endif
