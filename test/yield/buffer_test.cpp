// buffer_test.cpp

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

#include "yield/auto_object.hpp"
#include "yield/assert.hpp"
#include "yield/buffer.hpp"
#include "yunit.hpp"

TEST_SUITE(Buffer);

namespace yield {
TEST(Buffer, capacity) {
  auto_Object<Buffer> buffer = new Buffer(2);
  throw_assert_le(buffer->capacity(), 2);
}

TEST(Buffer, copy_Buffer) {
  auto_Object<Buffer> buffer1 = Buffer::copy("test", 4);
  auto_Object<Buffer> buffer2 = Buffer::copy(*buffer1);
  throw_assert_eq(strncmp(*buffer1, *buffer2, 4), 0);
}

TEST(Buffer, copy_const_void_size_t) {
  auto_Object<Buffer> buffer = Buffer::copy("test", 4);
  throw_assert_eq(strncmp(*buffer, "test", 4), 0);
}

TEST(Buffer, data) {
  auto_Object<Buffer> buffer = new Buffer(2);
  throw_assert_ne(buffer->data(), NULL);
}

TEST(Buffer, getpagesize) {
  size_t pagesize = Buffer::getpagesize();
  throw_assert_gt(pagesize, 0);
  throw_assert_eq(pagesize % 2, 0);
}

TEST(Buffer, is_page_aligned) {
  auto_Object<Buffer> buffer = new Buffer(Buffer::getpagesize());
  throw_assert(Buffer::is_page_aligned(buffer->data()));
  iovec iov;
  iov.iov_base = buffer->data();
  iov.iov_len = buffer->capacity();
  throw_assert(Buffer::is_page_aligned(iov));
}

TEST(Buffer, operator_array) {
  auto_Object<Buffer> buffer = Buffer::copy("m", 1);
  throw_assert_eq((*buffer)[0], 'm');
}

TEST(Buffer, operator_cast) {
  auto_Object<Buffer> buffer = new Buffer(2);
  throw_assert_ne(static_cast<void*>(*buffer), NULL);
  throw_assert_ne(static_cast<char*>(*buffer), NULL);
  throw_assert_ne(static_cast<uint8_t*>(*buffer), NULL);
}
}
