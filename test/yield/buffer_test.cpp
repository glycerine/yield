// buffer_test.cpp

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

#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "gtest/gtest.h"

#include <sstream>

namespace yield {
TEST(Buffer, as_read_iovec) {
  auto_Object<Buffer> buffer = new Buffer(2);
  iovec read_iovec = buffer->as_read_iovec();
  ASSERT_EQ(read_iovec.iov_base, buffer->data());
  ASSERT_EQ(read_iovec.iov_len, 2);
  buffer->put(1);
  read_iovec = buffer->as_read_iovec();
  ASSERT_EQ(
    read_iovec.iov_base,
    static_cast<char*>(buffer->data()) + buffer->size()
  );
  ASSERT_EQ(read_iovec.iov_len, 1);
}

TEST(Buffer, as_write_iovec) {
  auto_Object<Buffer> buffer = new Buffer(2);
  iovec write_iovec = buffer->as_write_iovec();
  ASSERT_EQ(write_iovec.iov_base, buffer->data());
  ASSERT_EQ(write_iovec.iov_len, 0);
  buffer->put(1);
  write_iovec = buffer->as_write_iovec();
  ASSERT_EQ(write_iovec.iov_base, buffer->data());
  ASSERT_EQ(write_iovec.iov_len, 1);
}

TEST(Buffer, capacity) {
  auto_Object<Buffer> buffer = new Buffer(2);
  ASSERT_EQ(buffer->capacity(), 2);
}

TEST(Buffer, constructor_alignment_capacity) {
  auto_Object<Buffer> buffer = new Buffer(4096, 2);
  ASSERT_EQ(buffer->capacity(), 2);
  ASSERT_TRUE(buffer->is_page_aligned());
}

TEST(Buffer, constructor_capacity) {
  auto_Object<Buffer> buffer = new Buffer(2);
  ASSERT_EQ(buffer->capacity(), 2);
}

class TestBuffer : public Buffer {
public:
  TestBuffer()
    : Buffer(Buffer::getpagesize(), static_cast<void*>(NULL), 0) {
  }
};

TEST(Buffer, constructor_capacity_data_size) {
  auto_Object<TestBuffer> buffer = new TestBuffer;
}

TEST(Buffer, copy_alignment_capacity_data_size) {
  auto_Object<Buffer> buffer = Buffer::copy(4096, 5, "test", 4);
  ASSERT_EQ(strncmp(*buffer, "test", 4), 0);
  ASSERT_GE(buffer->capacity(), 5u);
  ASSERT_EQ(buffer->size(), 4);
}

TEST(Buffer, copy_Buffer) {
  auto_Object<Buffer> buffer1 = Buffer::copy("test", 4);
  auto_Object<Buffer> buffer2 = Buffer::copy(*buffer1);
  ASSERT_EQ(strncmp(*buffer1, *buffer2, 4), 0);
  ASSERT_EQ(buffer2->size(), 4);
}

TEST(Buffer, copy_capacity_data_size) {
  auto_Object<Buffer> buffer = Buffer::copy(5, "test", 4);
  ASSERT_EQ(strncmp(*buffer, "test", 4), 0);
  ASSERT_GE(buffer->capacity(), 5u);
  ASSERT_EQ(buffer->size(), 4);
}

TEST(Buffer, copy_const_void_size_t) {
  auto_Object<Buffer> buffer = Buffer::copy("test", 4);
  ASSERT_EQ(strncmp(*buffer, "test", 4), 0);
  ASSERT_EQ(buffer->size(), 4);
}

TEST(Buffer, copy_c_string) {
  auto_Object<Buffer> buffer = Buffer::copy("test");
  ASSERT_EQ(strncmp(*buffer, "test", 4), 0);
  ASSERT_EQ(buffer->size(), 4);
}

TEST(Buffer, copy_string) {
  auto_Object<Buffer> buffer = Buffer::copy(string("test"));
  ASSERT_EQ(strncmp(*buffer, "test", 4), 0);
  ASSERT_EQ(buffer->size(), 4);
}

TEST(Buffer, data) {
  auto_Object<Buffer> buffer = new Buffer(2);
  ASSERT_NE(buffer->data(), static_cast<void*>(NULL));
  memcpy_s(buffer->data(), buffer->capacity(), "te", 2);
}

TEST(Buffer, empty) {
  auto_Object<Buffer> buffer = new Buffer(2);
  ASSERT_TRUE(buffer->empty());
  buffer->put("te");
  ASSERT_FALSE(buffer->empty());
}

TEST(Buffer, get_next_buffer) {
  auto_Object<Buffer> buffer = new Buffer(2);
  auto_Object<Buffer> buffer2 = new Buffer(2);
  buffer->set_next_buffer(buffer2->inc_ref());
  ASSERT_EQ(buffer->get_next_buffer(), &buffer2.get());
}

TEST(Buffer, getpagesize) {
  size_t pagesize = Buffer::getpagesize();
  ASSERT_GT(pagesize, 0u);
  ASSERT_EQ(pagesize % 2, 0);
}

TEST(Buffer, get_type_id) {
  auto_Object<Buffer> buffer = new Buffer(2);
  ASSERT_EQ(buffer->get_type_id(), Buffer::TYPE_ID);
}

TEST(Buffer, get_type_name) {
  auto_Object<Buffer> buffer = new Buffer(2);
  ASSERT_EQ(strcmp(buffer->get_type_name(), "yield::Buffer"), 0);
}

TEST(Buffer, inc_ref) {
  auto_Object<Buffer> buffer = new Buffer(2);
  buffer->inc_ref();
  Buffer::dec_ref(*buffer);
}

TEST(Buffer, is_page_aligned) {
  auto_Object<Buffer> buffer
  = new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
  ASSERT_TRUE(buffer->is_page_aligned());
}

TEST(Buffer, is_page_aligned_const_void) {
  auto_Object<Buffer> buffer
  = new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
  ASSERT_TRUE(Buffer::is_page_aligned(buffer->data()));
}

TEST(Buffer, is_page_aligned_iovec) {
  auto_Object<Buffer> buffer
  = new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
  ASSERT_TRUE(Buffer::is_page_aligned(buffer->as_read_iovec()));
}

TEST(Buffer, operator_array) {
  auto_Object<Buffer> buffer = Buffer::copy("m", 1);
  ASSERT_EQ((*buffer)[0], 'm');
}

TEST(Buffer, operator_cast) {
  auto_Object<Buffer> buffer = new Buffer(2);
  ASSERT_EQ(static_cast<const char*>(*buffer), buffer->data());
  ASSERT_EQ(static_cast<char*>(*buffer), buffer->data());
  ASSERT_EQ(static_cast<uint8_t*>(*buffer), buffer->data());
  ASSERT_EQ(static_cast<const uint8_t*>(*buffer), buffer->data());
  ASSERT_EQ(static_cast<void*>(*buffer), buffer->data());
  ASSERT_EQ(static_cast<const void*>(*buffer), buffer->data());
}

TEST(Buffer, operator_equals_Buffer) {
  auto_Object<Buffer> buffer1 = new Buffer(2);
  buffer1->put("mm");

  auto_Object<Buffer> buffer2 = new Buffer(4);
  buffer2->put("mm");
  ASSERT_EQ(*buffer1, *buffer2);
  buffer2->put("mm");
  ASSERT_NE(*buffer1, *buffer2);

  auto_Object<Buffer> buffer3 = new Buffer(2);
  buffer3->put('n');
  ASSERT_NE(*buffer1, *buffer3);
  buffer3->put('n');
  ASSERT_NE(*buffer1, *buffer3);
}

TEST(Buffer, operator_equals_c_string) {
  auto_Object<Buffer> buffer = Buffer::copy("test");
  ASSERT_EQ(*buffer, "test");
  ASSERT_NE(*buffer, "test1");
  ASSERT_NE(*buffer, "text");
}

TEST(Buffer, operator_equals_string) {
  auto_Object<Buffer> buffer = Buffer::copy("test");
  ASSERT_EQ(*buffer, string("test"));
  ASSERT_NE(*buffer, string("test1"));
  ASSERT_NE(*buffer, string("text"));
}

TEST(Buffer, print) {
  auto_Object<Buffer> buffer = Buffer::copy("test");

  {
    std::ostringstream oss;
    oss << *buffer;
    ASSERT_NE(oss.str().size(), 0u);
  }

  {
    auto_Object<Buffer> buffer = new Buffer(Buffer::getpagesize());
    for (uint16_t i = 0; i < Buffer::getpagesize(); ++i) {
      buffer->put('m');
    }
    std::ostringstream oss;
    oss << *buffer;
    ASSERT_NE(oss.str().size(), 0u);
    ASSERT_LT(oss.str().size(), Buffer::getpagesize());
  }

  {
    std::ostringstream oss;
    oss << &buffer.get();
    ASSERT_NE(oss.str().size(), 0u);
  }

  {
    std::ostringstream oss;
    oss << static_cast<Buffer*>(NULL);
    ASSERT_EQ(oss.str(), "NULL");
  }
}

TEST(Buffer, put_Buffer) {
  auto_Object<Buffer> buffer = new Buffer(2);
  auto_Object<Buffer> buffer2 = new Buffer(2);
  buffer2->put('m');
  buffer->put(*buffer2);
  ASSERT_EQ(buffer->size(), 1);
  ASSERT_EQ(memcmp(*buffer, "m", 1), 0);
}

TEST(Buffer, put_char) {
  auto_Object<Buffer> buffer = new Buffer(2);
  buffer->put('m');
  ASSERT_EQ(buffer->size(), 1);
  ASSERT_EQ(memcmp(*buffer, "m", 1), 0);
}

TEST(Buffer, put_c_string) {
  auto_Object<Buffer> buffer = new Buffer(2);
  buffer->put("m");
  ASSERT_EQ(buffer->size(), 1);
  ASSERT_EQ(memcmp(*buffer, "m", 1), 0);
}

TEST(Buffer, put_const_void_size_t) {
  auto_Object<Buffer> buffer = new Buffer(2);
  buffer->put("m", 1);
  ASSERT_EQ(buffer->size(), 1);
  ASSERT_EQ(memcmp(*buffer, "m", 1), 0);
}

TEST(Buffer, put_iovec) {
  auto_Object<Buffer> buffer = new Buffer(2);
  iovec iov;
  iov.iov_base = const_cast<char*>("m");
  iov.iov_len = 1;
  buffer->put(iov);
  ASSERT_EQ(buffer->size(), 1);
  ASSERT_EQ(memcmp(*buffer, "m", 1), 0);
}

TEST(Buffer, put_string) {
  auto_Object<Buffer> buffer = new Buffer(2);
  buffer->put(string("m"));
  ASSERT_EQ(buffer->size(), 1);
  ASSERT_EQ(memcmp(*buffer, "m", 1), 0);
}

//TEST(Buffer, resize) {
//  auto_Object<Buffer> buffer = new Buffer(2);
//  ASSERT_EQ(buffer->size(), 0);
//
//  buffer->resize(1);
//  ASSERT_EQ(buffer->size(), 1);
//
//  buffer->resize(0);
//  ASSERT_EQ(buffer->size(), 0);
//
//  buffer->resize(buffer->capacity());
//  ASSERT_EQ(buffer->size(), buffer->capacity());
//}

TEST(Buffer, set_next_buffer) {
  auto_Object<Buffer> buffer = new Buffer(2);
  auto_Object<Buffer> buffer2 = new Buffer(2);
  auto_Object<Buffer> buffer3 = new Buffer(2);

  buffer->set_next_buffer(buffer2->inc_ref());
  ASSERT_EQ(buffer->get_next_buffer(), &buffer2.get());

  buffer->set_next_buffer(&buffer3->inc_ref());
  ASSERT_EQ(buffer->get_next_buffer(), &buffer3.get());

  buffer->set_next_buffer(NULL);
  ASSERT_EQ(buffer->get_next_buffer(), static_cast<void*>(NULL));
}

TEST(Buffer, size) {
  auto_Object<Buffer> buffer = new Buffer(2);
  ASSERT_EQ(buffer->size(), 0);
  buffer->put('m');
  ASSERT_EQ(buffer->size(), 1);
  while (buffer->size() < buffer->capacity()) {
    buffer->put('m');
  }
  ASSERT_EQ(buffer->size(), buffer->capacity());
  buffer->put('m');
  ASSERT_EQ(buffer->size(), buffer->capacity());
}
}
