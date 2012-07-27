// channel_test.cpp

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

#include "yield/buffer.hpp"
#include "yield/channel.hpp"
#include "gtest/gtest.h"

namespace yield {
class MockChannel : public Channel {
public:
  // yield::Channel
  bool close() {
    return true;
  }

  ssize_t read(Buffer& buffer) {
    return Channel::read(buffer);
  }

  ssize_t read(void* buf, size_t buflen) {
    // ASSERT_NE(buf, NULL);
    // ASSERT_EQ(buflen, Buffer::getpagesize());
    return Buffer::getpagesize() / 2;
  }

  ssize_t readv(const iovec* iov, int iovlen) {
    // ASSERT_NE(iov, NULL);
    // ASSERT_EQ(iovlen, 2);
    return Buffer::getpagesize() / 2;
  }

  ssize_t write(const Buffer& buffer) {
    return Channel::write(buffer);
  }

  ssize_t write(const void* buf, size_t buflen) {
    // ASSERT_NE(buf, NULL);
    // ASSERT_TRUE(buflen == Buffer::getpagesize() / 2 || buflen == 11);
    return buflen;
  }
};

TEST(Channel, read_Buffer) {
  Buffer buffer(Buffer::getpagesize());
  ssize_t read_ret = MockChannel().read(buffer);
  ASSERT_EQ(read_ret, static_cast<ssize_t>(Buffer::getpagesize() / 2));
  ASSERT_EQ(buffer.size(), Buffer::getpagesize() / 2);
}

TEST(Channel, read_Buffers) {
  Buffer buffer2(Buffer::getpagesize());
  Buffer buffer1(Buffer::getpagesize());
  buffer1.set_next_buffer(buffer2.inc_ref());
  ssize_t read_ret = MockChannel().read(buffer1);
  ASSERT_EQ(read_ret, static_cast<ssize_t>(Buffer::getpagesize() / 2));
  ASSERT_EQ(buffer1.size(), Buffer::getpagesize() / 2);
  ASSERT_EQ(buffer2.size(), 0);
}

TEST(Channel, write_Buffer) {
  Buffer buffer(Buffer::getpagesize());
  buffer.put(NULL, Buffer::getpagesize() / 2);
  ssize_t write_ret = MockChannel().write(buffer);
  ASSERT_EQ(write_ret, static_cast<ssize_t>(Buffer::getpagesize() / 2));
}

TEST(Channel, write_Buffers) {
  Buffer buffer2(Buffer::getpagesize());
  buffer2.put(NULL, Buffer::getpagesize() / 4);
  Buffer buffer1(Buffer::getpagesize());
  buffer1.put(NULL, Buffer::getpagesize() / 4);
  buffer1.set_next_buffer(buffer2.inc_ref());
  ssize_t write_ret = MockChannel().write(buffer1);
  ASSERT_EQ(write_ret, static_cast<ssize_t>(Buffer::getpagesize() / 2));
}

TEST(Channel, writev) {
  iovec iov[2];
  iov[0].iov_base = const_cast<char*>("test ");
  iov[0].iov_len = 5;
  iov[1].iov_base = const_cast<char*>("string");
  iov[1].iov_len = 6;
  ssize_t write_ret = MockChannel().writev(iov, 2);
  ASSERT_EQ(write_ret, 11);
}
}
