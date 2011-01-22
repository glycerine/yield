// yield/string_buffer_test.cpp

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


#include "buffer_test.hpp"
#include "yield/string_buffer.hpp"


TEST_SUITE(StringBuffer);

namespace yield {
TEST_EX(StringBuffer, capacity, BufferCapacityTest<StringBuffer>) {
  BufferCapacityTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, copy, BufferCopyTest<StringBuffer>) {
  BufferCopyTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, data, BufferDataTest<StringBuffer>) {
  BufferDataTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, empty, BufferEmptyTest<StringBuffer>) {
  BufferEmptyTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, get_next_buffer, BufferGetNextBufferTest<StringBuffer>) {
  BufferGetNextBufferTest<StringBuffer>::run();
}

TEST(StringBuffer, init_capacity) {
  {
    auto_Object<StringBuffer> buffer = new StringBuffer(1);
    throw_assert_eq(buffer->capacity(), 1);
  }

  {
    auto_Object<StringBuffer> buffer = new StringBuffer(4096);
    throw_assert_eq(buffer->capacity(), 4096);
  }

  {
    auto_Object<StringBuffer> buffer = new StringBuffer(0);
    throw_assert_eq(buffer->capacity(), 0);
  }
}

TEST(StringBuffer, init_capacity_data_size) {
  auto_Object<StringBuffer> buffer = new StringBuffer(4096, "test", 4);
  throw_assert_eq(buffer->capacity(), 4096);
  throw_assert_eq(strncmp(*buffer, "test", 4), 0);
  throw_assert_eq(buffer->size(), 4);
}

TEST_EX(StringBuffer, init_c_string, BufferInitCStringTest<StringBuffer>) {
  BufferInitCStringTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, init_string, BufferInitStringTest<StringBuffer>) {
  BufferInitStringTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, operator_array, BufferOperatorArrayTest<StringBuffer>) {
  BufferOperatorArrayTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, operator_cast, BufferOperatorCastTest<StringBuffer>) {
  BufferOperatorCastTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, operator_equals, BufferOperatorEqualsTest<StringBuffer>) {
  BufferOperatorEqualsTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, put_buffer, BufferPutBufferTest<StringBuffer>) {
  BufferPutBufferTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, put_char, BufferPutCharTest<StringBuffer>) {
  BufferPutCharTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, put_c_string, BufferPutCStringTest<StringBuffer>) {
  BufferPutCStringTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, put_c_string_size, BufferPutCStringSizeTest<StringBuffer>) {
  BufferPutCStringSizeTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, put_iovec, BufferPutIOVecTest<StringBuffer>) {
  BufferPutIOVecTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, put_string, BufferPutStringTest<StringBuffer>) {
  BufferPutStringTest<StringBuffer>::run();
}

TEST(StringBuffer, reserve) {
  //auto_Object<StringBuffer> buffer = new StringBuffer( 0 );
  //throw_assert_eq( buffer->capacity(), 0 );

  //buffer->reserve( 0 );
  //throw_assert_eq( buffer->capacity(), 0 );

  //buffer->reserve( 1 );
  //throw_assert_eq( buffer->capacity(), StringBuffer::getbuffersize() );

  //buffer->reserve( buffer->capacity() + 1 );
  //throw_assert_eq( buffer->capacity(), StringBuffer::getbuffersize()*2 );

  //buffer->reserve( StringBuffer::getbuffersize() );
  //throw_assert_eq( buffer->capacity(), StringBuffer::getbuffersize()*2 );
}

TEST_EX(StringBuffer, resize, BufferResizeTest<StringBuffer>) {
  BufferResizeTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, set_next_buffer, BufferSetNextBufferTest<StringBuffer>) {
  BufferSetNextBufferTest<StringBuffer>::run();
}

TEST_EX(StringBuffer, size, BufferSizeTest<StringBuffer>) {
  BufferSizeTest<StringBuffer>::run();
}
}
