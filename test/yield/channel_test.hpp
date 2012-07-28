// channel_test.hpp

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

#ifndef _YIELD_CHANNEL_TEST_HPP_
#define _YIELD_CHANNEL_TEST_HPP_

#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "yield/channel.hpp"
#include "yield/channel_pair.hpp"
#include "yield/exception.hpp"
#include "gtest/gtest.h"

namespace yield {
template <class ChannelPairType>
class ChannelTest : public ::testing::Test {
public:
  virtual ~ChannelTest() {
  }

  virtual void SetUp() {
    channel_pair = new ChannelPairType();
  }

  virtual void TearDown() {
    ChannelPair::dec_ref(channel_pair);
    channel_pair = NULL;
  }

protected:
  ChannelTest() : test_string("test string") {
  }

protected:
  Channel& get_read_channel() {
    return channel_pair->get_read_channel();
  }

  const string& get_test_string() const {
    return test_string;
  }

  Channel& get_write_channel() {
    return channel_pair->get_write_channel();
  }

protected:
  void check_read(const void* buf, ssize_t read_ret) {
    if (read_ret >= 0) {
      ASSERT_EQ(
        static_cast<size_t>(read_ret),
        get_test_string().size()
      );

      ASSERT_EQ(
        memcmp(buf, get_test_string().data(), get_test_string().size()),
        0
      );
    } else {
      throw Exception();
    }
  }

  void check_write(ssize_t write_ret) {
    if (write_ret >= 0) {
      ASSERT_EQ(
        static_cast<size_t>(write_ret),
        get_test_string().size()
      );
    } else {
      throw Exception();
    }
  }

  void read() {
    string test_string;
    test_string.resize(get_test_string().size());
    check_read(
      test_string.data(),
      get_read_channel().read(
        const_cast<char*>(test_string.data()),
        test_string.capacity()
      )
    );
  }

  void write() {
    check_write(
      get_write_channel().write(
        get_test_string().data(),
        get_test_string().size()
      )
    );
  }

private:
  ChannelPair* channel_pair;
  string test_string;
};

TYPED_TEST_CASE_P(ChannelTest);

TYPED_TEST_P(ChannelTest, close) {
  if (!get_read_channel().close()) {
    throw Exception();
  }
}

TYPED_TEST_P(ChannelTest, read) {
  write();
  read();
}

TYPED_TEST_P(ChannelTest, read_Buffer) {
  this->write();
  auto_Object<Buffer> test_buffer = new Buffer(get_test_string().size());
  check_read(*test_buffer, get_read_channel().read(*test_buffer));
}

TYPED_TEST_P(ChannelTest, read_Buffers) {
  this->write();
  auto_Object<Buffer> test_buffer = new Buffer(get_test_string().size());
  test_buffer->set_next_buffer(new Buffer(get_test_string().size()));
  check_read(*test_buffer, get_read_channel().read(*test_buffer));
}

TYPED_TEST_P(ChannelTest, readv_one) {
  this->write();

  string test_string;
  test_string.resize(get_test_string().size());
  iovec iov;
  iov.iov_base = const_cast<char*>(test_string.data());
  iov.iov_len = test_string.size();

  check_read(
    test_string.data(),
    get_read_channel().readv(&iov, 1)
  );
}

TYPED_TEST_P(ChannelTest, readv_two) {
  this->write();

  string test_string;
  test_string.resize(get_test_string().size());
  iovec iov[2];
  iov[0].iov_base = const_cast<char*>(test_string.data());
  iov[0].iov_len = 4;
  iov[1].iov_base = const_cast<char*>(test_string.data()) + 4;
  iov[1].iov_len = 7;

  check_read(
    test_string.data(),
    get_read_channel().readv(iov, 2)
  );
}

TYPED_TEST_P(ChannelTest, write) {
  write();
  read();
}

TYPED_TEST_P(ChannelTest, write_Buffer) {
  auto_Object<Buffer> test_buffer = Buffer::copy(get_test_string());
  check_write(get_write_channel().write(*test_buffer));

  read();
}

TYPED_TEST_P(ChannelTest, write_Buffers) {
  auto_Object<Buffer> test_buffer = Buffer::copy(get_test_string());
  test_buffer->set_next_buffer(new Buffer(1));
  check_write(get_write_channel().write(*test_buffer));

  read();
}

TYPED_TEST_P(ChannelTest, writev_one) {
  iovec iov;
  iov.iov_base = const_cast<char*>(get_test_string().data());
  iov.iov_len = get_test_string().size();
  check_write(get_write_channel().writev(&iov, 1));

  read();
}

TYPED_TEST_P(ChannelTest, writev_two) {
  iovec iov[2];
  iov[0].iov_base = const_cast<char*>(get_test_string().data());
  iov[0].iov_len = 4;
  iov[1].iov_base = const_cast<char*>(get_test_string().data()) + 4;
  iov[1].iov_len = get_test_string().size() - 4;
  check_write(get_write_channel().writev(iov, 2));

  read();
}

REGISTER_TYPED_TEST_CASE_P(ChannelTest, close, read, read_Buffer, read_Buffers, readv_one, readv_two, write, write_Buffer, write_Buffers, writev_one, writev_two);
}

#endif
