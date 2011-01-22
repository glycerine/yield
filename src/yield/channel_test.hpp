// yield/channel_test.hpp

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


#ifndef _YIELD_CHANNEL_TEST_HPP_
#define _YIELD_CHANNEL_TEST_HPP_


#include "yield/assert.hpp"
#include "yield/channel.hpp"
#include "yield/channel_pair.hpp"
#include "yield/exception.hpp"
#include "yunit.hpp"


namespace yield {
class ChannelPairFactory : public Object {
public:
  virtual ~ChannelPairFactory() { }

  virtual ChannelPair& createChannelPair() = 0;

  // yield::Object
  ChannelPairFactory& inc_ref() {
    return Object::inc_ref(*this);
  }
};


class ChannelTest : public yunit::Test {
public:
  virtual ~ChannelTest() {
    ChannelPairFactory::dec_ref(channel_pair_factory);
  }

  virtual void setup() {
    channel_pair = &channel_pair_factory.createChannelPair();
  }

  virtual void teardown() {
    ChannelPair::dec_ref(channel_pair);
    channel_pair = NULL;
  }

protected:
  ChannelTest(ChannelPairFactory& channel_pair_factory)
    : channel_pair_factory(channel_pair_factory.inc_ref()),
      test_string("test string")
  { }

  Channel& get_read_channel() {
    return channel_pair->get_read_channel();
  }
  const string& get_test_string() const {
    return test_string;
  }
  Channel& get_write_channel() {
    return channel_pair->get_write_channel();
  }

  void read() {
    string test_string;
    test_string.resize(get_test_string().size());
    ssize_t read_ret
    = get_read_channel().read
      (
        const_cast<char*>(test_string.data()),
        test_string.capacity()
      );

    if (read_ret >= 0) {
      throw_assert_eq
      (
        static_cast<size_t>(read_ret),
        get_test_string().size()
      );

      throw_assert_eq(test_string, get_test_string());
    } else
      throw Exception();
  }

  void write() {
    ssize_t write_ret
    = get_write_channel().write
      (
        get_test_string().data(),
        get_test_string().size()
      );

    if (write_ret >= 0) {
      throw_assert_eq
      (
        static_cast<size_t>(write_ret),
        get_test_string().size()
      );
    } else
      throw Exception();
  }

private:
  ChannelPairFactory& channel_pair_factory;
  ChannelPair* channel_pair;
  string test_string;
};


class ChannelCloseTest : public ChannelTest {
public:
  ChannelCloseTest(ChannelPairFactory& channel_pair_factory)
    : ChannelTest(channel_pair_factory)
  { }

  // Test
  void run() {
    if (!get_read_channel().close())
      throw Exception();
  }
};


class ChannelOperatorFDTest : public ChannelTest {
public:
  ChannelOperatorFDTest(ChannelPairFactory& channel_pair_factory)
    : ChannelTest(channel_pair_factory)
  { }

  // Test
  virtual void run() {
    debug_assert_ne(static_cast<fd_t>(get_read_channel()), INVALID_FD);
    debug_assert_ne(static_cast<fd_t>(get_write_channel()), INVALID_FD);
  }
};


class ChannelReadTest : public ChannelTest {
public:
  ChannelReadTest(ChannelPairFactory& channel_pair_factory)
    : ChannelTest(channel_pair_factory)
  { }

  // Test
  virtual void run() {
    this->write();
    this->read();
  }
};


class ChannelReadVOneTest : public ChannelTest {
public:
  ChannelReadVOneTest(ChannelPairFactory& channel_pair_factory)
    : ChannelTest(channel_pair_factory)
  { }

  // Test
  virtual void run() {
    this->write();

    string test_string;
    test_string.resize(get_test_string().size());
    iovec iov;
    iov.iov_base = const_cast<char*>(test_string.data());
    iov.iov_len = test_string.size();

    ssize_t readv_ret = get_read_channel().readv(&iov, 1);
    if (readv_ret >= 0) {
      throw_assert_eq
      (
        static_cast<size_t>(readv_ret),
        get_test_string().size()
      );

      throw_assert_eq(test_string, get_test_string());
    } else
      throw Exception();
  }
};


class ChannelReadVTwoTest : public ChannelTest {
public:
  ChannelReadVTwoTest(ChannelPairFactory& channel_pair_factory)
    : ChannelTest(channel_pair_factory)
  { }

  // Test
  virtual void run() {
    this->write();

    iovec iov[2];

    string test;
    test.resize(4);
    iov[0].iov_base = const_cast<char*>(test.data());
    iov[0].iov_len = 4;

    string _string;
    _string.resize(7);
    iov[1].iov_base = const_cast<char*>(_string.data());
    iov[1].iov_len = 7;

    ssize_t readv_ret = get_read_channel().readv(iov, 2);
    if (readv_ret >= 0) {
      throw_assert_eq
      (
        static_cast<size_t>(readv_ret),
        get_test_string().size()
      );
      throw_assert_eq(test, "test");
      throw_assert_eq(_string, " string");
    } else
      throw Exception();
  }
};


class ChannelSetBlockingModeTest : public ChannelTest {
public:
  ChannelSetBlockingModeTest(ChannelPairFactory& channel_pair_factory)
    : ChannelTest(channel_pair_factory)
  { }

  // Test
  void run() {
    if
    (
      !get_read_channel().set_blocking_mode(true)
      &&
      Exception::get_last_error_code() != ENOTSUP
    )
      throw Exception();

    if
    (
      !get_write_channel().set_blocking_mode(false)
      &&
      Exception::get_last_error_code() != ENOTSUP
    )
      throw Exception();
  }
};


class ChannelWriteTest : public ChannelTest {
public:
  ChannelWriteTest(ChannelPairFactory& channel_pair_factory)
    : ChannelTest(channel_pair_factory)
  { }

  // Test
  virtual void run() {
    this->write();
    this->read();
  }
};


class ChannelWriteVOneTest : public ChannelTest {
public:
  ChannelWriteVOneTest(ChannelPairFactory& channel_pair_factory)
    : ChannelTest(channel_pair_factory)
  { }

  // Test
  virtual void run() {
    iovec iov;
    iov.iov_base = const_cast<char*>(get_test_string().data());
    iov.iov_len = get_test_string().size();
    ssize_t writev_ret
    = get_write_channel().writev(&iov, 1);

    if (writev_ret >= 0) {
      throw_assert_eq
      (
        static_cast<size_t>(writev_ret),
        get_test_string().size()
      );
    } else
      throw Exception();

    this->read();
  }
};


class ChannelWriteVTwoTest : public ChannelTest {
public:
  ChannelWriteVTwoTest(ChannelPairFactory& channel_pair_factory)
    : ChannelTest(channel_pair_factory)
  { }

  // Test
  virtual void run() {
    iovec iov[2];

    iov[0].iov_base = const_cast<char*>(get_test_string().data());
    iov[0].iov_len = 4;

    iov[1].iov_base
    = const_cast<char*>(get_test_string().data()) + 4;
    iov[1].iov_len = get_test_string().size() - 4;

    ssize_t writev_ret
    = get_write_channel().writev(iov, 2);

    if (writev_ret >= 0) {
      throw_assert_eq
      (
        static_cast<size_t>(writev_ret),
        get_test_string().size()
      );
    } else
      throw Exception();

    this->read();
  }
};


class ChannelTestSuite : public yunit::TestSuite {
public:
  ChannelTestSuite(YO_NEW_REF ChannelPairFactory& channel_pair_factory)
    : channel_pair_factory(channel_pair_factory) {
    add
    (
      "Channel::close",
      new ChannelCloseTest(channel_pair_factory)
    );

    add
    (
      "Channel::operator fd_t()",
      new ChannelOperatorFDTest(channel_pair_factory)
    );

    add("Channel::read", new ChannelReadTest(channel_pair_factory));

    add
    (
      "Channel::readv( iov, 1 )",
      new ChannelReadVOneTest(channel_pair_factory)
    );

    add
    (
      "Channel::readv( iov, 2 )",
      new ChannelReadVTwoTest(channel_pair_factory)
    );

    add
    (
      "Channel::set_blocking_mode",
      new ChannelSetBlockingModeTest(channel_pair_factory)
    );

    add
    (
      "Channel::write",
      new ChannelWriteTest(channel_pair_factory)
    );

    add
    (
      "Channel::writev( iov, 1 )",
      new ChannelWriteVOneTest(channel_pair_factory)
    );

    add
    (
      "Channel::writev( iov, 2 )",
      new ChannelWriteVTwoTest(channel_pair_factory)
    );
  }

  ~ChannelTestSuite() {
    ChannelPairFactory::dec_ref(channel_pair_factory);
  }

  ChannelPairFactory& get_channel_pair_factory() {
    return channel_pair_factory;
  }

private:
  ChannelPairFactory& channel_pair_factory;
};
}


#endif
