// yield/buffers.cpp

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

#include "yield/debug.hpp"
#include "yield/buffer.hpp"
#include "yield/buffers.hpp"

namespace yield {
void
Buffers::as_read_iovecs(
  Buffer& buffers,
  vector<iovec>& read_iovecs
) {
  Buffer* next_buffer = &buffers;
  do {
    read_iovecs.push_back(next_buffer->as_read_iovec());
    next_buffer = next_buffer->get_next_buffer();
  } while (next_buffer != NULL);
}

size_t
Buffers::as_write_iovecs(
  const Buffer& buffers,
  vector<iovec>& write_iovecs
) {
  size_t ret = 0;

  const Buffer* next_buffer = &buffers;
  do {
    iovec write_iovec = next_buffer->as_write_iovec();
    ret += write_iovec.iov_len;
    write_iovecs.push_back(write_iovec);
    next_buffer = next_buffer->get_next_buffer();
  } while (next_buffer != NULL);

  return ret;
}

size_t
Buffers::as_write_iovecs(
  const Buffer& buffers,
  size_t offset,
  vector<iovec>& write_iovecs
) {
  size_t ret = 0;

  const Buffer* next_buffer = &buffers;
  do {
    iovec write_iovec = next_buffer->as_write_iovec();
    if (write_iovec.iov_len < offset) {
      offset -= write_iovec.iov_len;
      next_buffer = next_buffer->get_next_buffer();
    } else if (write_iovec.iov_len == offset) {
      next_buffer = next_buffer->get_next_buffer();
      break;
    } else {
      write_iovec.iov_base = static_cast<char*>(write_iovec.iov_base) + offset;
      write_iovec.iov_len -= offset;
      ret += write_iovec.iov_len;
      write_iovecs.push_back(write_iovec);
      next_buffer = next_buffer->get_next_buffer();
      break;
    }
  } while (next_buffer != NULL);

  while (next_buffer != NULL) {
    iovec write_iovec = next_buffer->as_write_iovec();
    ret += write_iovec.iov_len;
    write_iovecs.push_back(write_iovec);
    next_buffer = next_buffer->get_next_buffer();
  }

  return ret;
}

void Buffers::put(Buffer& buffers, const void* data, size_t size) {
  debug_assert_eq(data, NULL);

  Buffer* next_buffer = &buffers;
  do {
    size_t next_buffer_left
    = next_buffer->capacity() - next_buffer->size();

    if (next_buffer_left > 0) {
      if (size <= next_buffer_left) {
        next_buffer->put(NULL, size);
        break;
      } else {
        next_buffer->put(NULL, next_buffer_left);
        size -= next_buffer_left;
      }
    }

    next_buffer = next_buffer->get_next_buffer();
  } while (next_buffer != NULL);
}

size_t Buffers::size(const Buffer& buffers) {
  size_t size = 0;

  const Buffer* next_buffer = &buffers;
  do {
    size += next_buffer->size();
    next_buffer = next_buffer->get_next_buffer();
  } while (next_buffer != NULL);

  return size;
}
}
