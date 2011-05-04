// stream_socket_test.hpp

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

#ifndef _YIELD_SOCKETS_STREAM_SOCKET_TEST_HPP_
#define _YIELD_SOCKETS_STREAM_SOCKET_TEST_HPP_

#include "socket_test.hpp"

namespace yield {
namespace sockets {
template <class StreamSocketType>
class StreamSocketDupTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    auto_Object<StreamSocket> socket_ = StreamSocketType().dup();
  }
};


template <class StreamSocketType>
class StreamSocketListenTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    StreamSocketType socket_;
    if (!socket_.bind(31000))
      throw Exception();
    if (!socket_.listen())
      throw Exception();
  }
};


template <class StreamSocketType>
class StreamSocketTestSuite : public SocketTestSuite<StreamSocketType> {
public:
  StreamSocketTestSuite() {
    add("StreamSocket::dup", new StreamSocketDupTest<StreamSocketType>);
    add("StreamSocket::listen", new StreamSocketListenTest<StreamSocketType>);

    add(
      "StreamSocket::setsockopt(KEEPALIVE)",
      new SocketSetSocketOptionTest<StreamSocketType>(
            StreamSocket::Option::KEEPALIVE,
            1
          )
    );

    add(
      "StreamSocket::setsockopt(LINGER, 1)",
      new SocketSetSocketOptionTest<StreamSocketType>(
            StreamSocket::Option::LINGER,
            1
          )
    );

    add(
      "StreamSocket::setsockopt(LINGER, 30)",
      new SocketSetSocketOptionTest<StreamSocketType>(
            StreamSocket::Option::LINGER,
            30
          )
    );
  }
};
}
}

#endif
