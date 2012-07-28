// stream_socket_test.cpp

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
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/stat.hpp"
#include "yield/sockets/stream_socket_pair.hpp"
#include "yield/sockets/tcp_socket.hpp"

namespace yield {
namespace sockets {
INSTANTIATE_TYPED_TEST_CASE_P(StreamSocket, ChannelTest, StreamSocketPair);
INSTANTIATE_TYPED_TEST_CASE_P(StreamSocket, SocketTest, StreamSocketPair);

TEST(StreamSocket, accept) {
  StreamSocket client_stream_socket(TCPSocket::DOMAIN_DEFAULT),
               listen_stream_socket(TCPSocket::DOMAIN_DEFAULT);
  if (listen_stream_socket.bind(SocketAddress::IN_LOOPBACK)) {
    if (listen_stream_socket.listen()) {
      if (client_stream_socket.connect(*listen_stream_socket.getsockname())) {
        auto_Object<StreamSocket> server_stream_socket
        = listen_stream_socket.accept();
        return;
      }
    }
  }

  throw Exception();
}

TEST(StreamSocket, dup) {
  auto_Object<StreamSocket> socket_ = StreamSocketPair().first().dup();
}

TEST(StreamSocket, getsockname_exception) {
  try {
    StreamSocket(TCPSocket::DOMAIN_DEFAULT).getsockname();
  } catch (Exception&) {
    return;
  }
  ASSERT_TRUE(false);
}

TEST(StreamSocket, getpeername_exception) {
  try {
    StreamSocket(TCPSocket::DOMAIN_DEFAULT).getpeername();
  } catch (Exception&) {
    return;
  }
  ASSERT_TRUE(false);
}

TEST(StreamSocket, inc_ref) {
  auto_Object<StreamSocket> socket_ = StreamSocketPair().first().inc_ref();
}

TEST(StreamSocket, listen) {
  StreamSocket stream_socket(TCPSocket::DOMAIN_DEFAULT);

  if (!stream_socket.bind(SocketAddress::IN_LOOPBACK)) {
    throw Exception();
  }

  if (!stream_socket.listen()) {
    throw Exception();
  }
}

TEST(StreamSocket, setsockopt_KEEPALIVE) {
  if (!StreamSocketPair().first().setsockopt(StreamSocket::Option::KEEPALIVE, true)) {
    throw Exception();
  }
}

class StreamSocketSendFileTest : public ::testing::Test {
public:
  void SetUp() {
    TearDown();
    auto_Object<yield::fs::File> file
    = yield::fs::FileSystem().creat(test_file_path);
    file->write("test", 4);
    file->close();
  }

  void TearDown() {
    yield::fs::FileSystem().unlink(test_file_path);
    ASSERT_FALSE(yield::fs::FileSystem().exists(test_file_path));
  }

protected:
  StreamSocketSendFileTest()
    : test_file_path("StreamSocketSendFileTest.txt") {
  }

  yield::fs::Path test_file_path;
};

TEST_F(StreamSocketSendFileTest, sendfile) {
  auto_Object<yield::fs::File> file
  = yield::fs::FileSystem().open(test_file_path);
  auto_Object<yield::fs::Stat> stbuf = file->stat();
  size_t size = static_cast<size_t>(stbuf->get_size());

  StreamSocketPair stream_sockets;
  ssize_t sendfile_ret = stream_sockets.first().sendfile(*file, 0, size);
  ASSERT_EQ(sendfile_ret, stbuf->get_size());

  Buffer buffer(Buffer::getpagesize());
  ssize_t read_ret = stream_sockets.second().read(buffer);
  ASSERT_EQ(read_ret, static_cast<ssize_t>(size));

  file->close();
}

TEST(StreamSocket, setsockopt_LINGER) {
  StreamSocketPair stream_sockets;

  if (!stream_sockets.first().setsockopt(StreamSocket::Option::LINGER, 1)) {
    throw Exception();
  }

  if (!stream_sockets.first().setsockopt(StreamSocket::Option::LINGER, 30)) {
    throw Exception();
  }

  if (!stream_sockets.first().setsockopt(StreamSocket::Option::LINGER, false)) {
    throw Exception();
  }
}

TEST(StreamSocket, shutdown) {
  StreamSocketPair stream_sockets;

  if (!stream_sockets.first().shutdown(true, false)) {
    throw Exception();
  }
  if (!stream_sockets.first().shutdown(false, true)) {
    throw Exception();
  }

  if (!stream_sockets.second().shutdown(true, true)) {
    throw Exception();
  }
}

TEST(StreamSocket, want_accept) {
  StreamSocket listen_stream_socket(TCPSocket::DOMAIN_DEFAULT);
  if (listen_stream_socket.bind(SocketAddress::IN_LOOPBACK)) {
    if (listen_stream_socket.listen()) {
      if (listen_stream_socket.set_blocking_mode(false)) {
        StreamSocket* server_stream_socket = listen_stream_socket.accept();
        ASSERT_EQ(server_stream_socket, static_cast<StreamSocket*>(NULL));
        ASSERT_TRUE(listen_stream_socket.want_accept());
        return;
      }
    }
  }

  throw Exception();
}
}
}
