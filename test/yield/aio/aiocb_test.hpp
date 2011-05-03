// aiocb_test.hpp

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

#ifndef _YIELD_AIO_AIOCB_TEST_HPP_
#define _YIELD_AIO_AIOCB_TEST_HPP_

#include "yield/assert.hpp"
#include "yield/aio/aiocb.hpp"
#include "yunit.hpp"

namespace yield {
namespace aio {
typedef yunit::Test AIOCBTest;


class MockAIOCB : public AIOCB {
public:
  MockAIOCB(fd_t fd, off_t offset) : AIOCB(fd, offset) { }
};


class AIOCBConstructorsTest : public AIOCBTest {
public:
  // yunit::Test
  void run() {
    MockAIOCB(INVALID_FD, 0);
  }
};


class AIOCBGetErrorTest : public AIOCBTest {
public:
  // yunit::Test
  void run() {
    MockAIOCB aiocb(INVALID_FD, 0);
    throw_assert_eq(aiocb.get_error(), 0);
    aiocb.set_error(5);
    throw_assert_eq(aiocb.get_error(), 5);
  }
};


class AIOCBGetOffsetTest : public AIOCBTest {
public:
  // yunit::Test
  void run() {
    throw_assert_eq(MockAIOCB(INVALID_FD, 0).get_offset(), 0);
    throw_assert_eq(MockAIOCB(INVALID_FD, 1).get_offset(), 1);
  }
};


class AIOCBGetReturnTest : public AIOCBTest {
public:
  // yunit::Test
  void run() {
    MockAIOCB aiocb(INVALID_FD, 0);
    throw_assert_eq(aiocb.get_return(), -1);
    aiocb.set_return(0);
    throw_assert_eq(aiocb.get_return(), 0);
  }
};


class AIOCBGetTypeIdTest : public AIOCBTest {
public:
  // yunit::Test
  void run() {
    throw_assert_eq(MockAIOCB(INVALID_FD, 0).get_type_id(), AIOCB::TYPE_ID);
  }
};


class AIOCBGetTypeNameTest : public AIOCBTest {
public:
  // yunit::Test
  void run() {
    throw_assert_eq(
      strcmp(
        MockAIOCB(INVALID_FD, 0).get_type_name(),
        "yield::aio::win32::AIOCB"
      ),
      0
    );
  }
};


class AIOCBIncRefTest : public AIOCBTest {
public:
  // yunit::Test
  void run() {
    MockAIOCB(INVALID_FD, 0).inc_ref();
  }
};


class AIOCBTestSuite : public yunit::TestSuite {
public:
  AIOCBTestSuite() {
    add("AIOCB::constructors", new AIOCBConstructorsTest);
    add("AIOCB::get_error", new AIOCBGetErrorTest);
    add("AIOCB::get_offset", new AIOCBGetOffsetTest);
    add("AIOCB::get_return", new AIOCBGetReturnTest);
    add("AIOCB::get_type_id", new AIOCBGetTypeIdTest);
    add("AIOCB::get_type_name", new AIOCBGetTypeNameTest);
    add("AIOCB::inc_ref", new AIOCBIncRefTest);
  }
};
}
}

#endif
