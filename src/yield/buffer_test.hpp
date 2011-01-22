// yield/buffer_test.hpp

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


#ifndef _YIELD_BUFFER_TEST_HPP_
#define _YIELD_BUFFER_TEST_HPP_


#include "yield/auto_object.hpp"
#include "yield/assert.hpp"
#include "yield/buffer.hpp"
#include "yunit.hpp"


namespace yield {
typedef yunit::Test BufferTest;


template <class BufferType>
class BufferCapacityTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    throw_assert_le(buffer->capacity(), 4096);
  }
};


template <class BufferType>
class BufferCopyTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType("test", 4);
    auto_Object<BufferType> buffer2 = new BufferType(*buffer);
    throw_assert_eq(*buffer, *buffer2);
  }
};


template <class BufferType>
class BufferDataTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    throw_assert_ne(buffer->data(), NULL);
  }
};


template <class BufferType>
class BufferEmptyTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    throw_assert(buffer->empty());
    buffer->put('m', 1);
    throw_assert_false(buffer->empty());
  }
};


template <class BufferType>
class BufferGetNextBufferTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    auto_Object<BufferType> buffer2 = new BufferType;
    buffer->set_next_buffer(buffer2->inc_ref());
    throw_assert_eq(buffer->get_next_buffer(), &buffer2.get());
  }
};


template <class BufferType>
class BufferInitCStringTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType("test", 4);
    throw_assert_eq(buffer->size(), 4);
    throw_assert_eq(strncmp(*buffer, "test", 4), 0);
  }
};


template <class BufferType>
class BufferInitStringTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    string test("test");
    auto_Object<BufferType> buffer = new BufferType(test);
    throw_assert_eq(buffer->size(), 4);
    throw_assert_eq(strncmp(*buffer, "test", 4), 0);
  }
};


template <class BufferType>
class BufferOperatorArrayTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    buffer->put('m', 1);
    throw_assert_eq((*buffer)[0], 'm');
  }
};


template <class BufferType>
class BufferOperatorCastTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    throw_assert_ne(static_cast<void*>(*buffer), NULL);
    throw_assert_ne(static_cast<char*>(*buffer), NULL);
    throw_assert_ne(static_cast<uint8_t*>(*buffer), NULL);
  }
};


template <class BufferType>
class BufferOperatorEqualsTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer1 = new BufferType;
    buffer1->put('m', 2);

    auto_Object<BufferType> buffer2 = new BufferType;
    buffer2->put('m', 2);
    throw_assert_eq(*buffer1, *buffer2);
    buffer2->put('m', 2);
    throw_assert_ne(*buffer1, *buffer2);

    auto_Object<BufferType> buffer3 = new BufferType;
    buffer3->put('n', 1);
    throw_assert_ne(*buffer1, *buffer3);
    buffer3->put('n', 1);
    throw_assert_ne(*buffer1, *buffer3);
  }
};


template <class BufferType>
class BufferPutBufferTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    auto_Object<BufferType> buffer2 = new BufferType;
    buffer2->put('m', 1);
    buffer->put(*buffer2);
    throw_assert_eq(buffer->size(), 1);
    throw_assert_eq(memcmp(*buffer, "m", 1), 0);
  }
};


template <class BufferType>
class BufferPutCharTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    buffer->put('m', 1);
    throw_assert_eq(buffer->size(), 1);
    throw_assert_eq(memcmp(*buffer, "m", 1), 0);
  }
};


template <class BufferType>
class BufferPutCStringTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    buffer->put("m");
    throw_assert_eq(buffer->size(), 1);
    throw_assert_eq(memcmp(*buffer, "m", 1), 0);
  }
};


template <class BufferType>
class BufferPutCStringSizeTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    buffer->put("m", 1);
    throw_assert_eq(buffer->size(), 1);
    throw_assert_eq(memcmp(*buffer, "m", 1), 0);
  }
};


template <class BufferType>
class BufferPutIOVecTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    iovec iov;
    iov.iov_base = const_cast<char*>("m");
    iov.iov_len = 1;
    buffer->put(iov);
    throw_assert_eq(buffer->size(), 1);
    throw_assert_eq(memcmp(*buffer, "m", 1), 0);
  }
};


template <class BufferType>
class BufferPutStringTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    buffer->put(string("m"));
    throw_assert_eq(buffer->size(), 1);
    throw_assert_eq(memcmp(*buffer, "m", 1), 0);
  }
};


template <class BufferType>
class BufferResizeTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    throw_assert_eq(buffer->size(), 0);

    buffer->resize(1);
    throw_assert_eq(buffer->size(), 1);

    buffer->resize(0);
    throw_assert_eq(buffer->size(), 0);

    buffer->resize(buffer->capacity());
    throw_assert_eq(buffer->size(), buffer->capacity());

    size_t capacity = buffer->capacity();
    buffer->resize(capacity + 1);
    throw_assert_ge(buffer->capacity(), capacity + 1);
    throw_assert_eq(buffer->size(), capacity + 1);
  }
};


template <class BufferType>
class BufferSetNextBufferTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    auto_Object<BufferType> buffer2 = new BufferType;
    buffer->set_next_buffer(buffer2->inc_ref());
  }
};


template <class BufferType>
class BufferSizeTest : public BufferTest {
public:
  // yunit::Test
  void run() {
    auto_Object<BufferType> buffer = new BufferType;
    throw_assert_eq(buffer->size(), 0);
    buffer->put('m', 1);
    throw_assert_eq(buffer->size(), 1);
    buffer->put('m', buffer->capacity() - 1);
    throw_assert_eq(buffer->size(), buffer->capacity());
  }
};
}


#endif
