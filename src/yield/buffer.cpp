// yield/buffer.cpp

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

#include "yield/buffer.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <stdlib.h>
#endif

namespace yield {
size_t Buffer::pagesize = 0;

Buffer::Buffer(size_t capacity) {
  alloc(2, capacity);
}

Buffer::Buffer(size_t alignment, size_t capacity) {
  alloc(alignment, capacity);
}

Buffer::Buffer(size_t capacity, void* data)
  : capacity_(capacity),
    data_(data)
{ }

Buffer::~Buffer() {
#ifdef _WIN32
  _aligned_free(data_);
#else
  free(data_);
#endif
}

void Buffer::alloc(size_t alignment, size_t capacity) {
  if (alignment % 2 == 0) {
    capacity_ = (capacity + alignment - 1) / alignment * alignment;
#ifdef _WIN32
    if ((data_ = _aligned_malloc(capacity_, alignment)) == NULL)
#else
    if (posix_memalign(&data_, alignment, capacity_) != 0)
#endif
      throw std::bad_alloc();
  } else
    throw std::bad_alloc();
}

Buffer& Buffer::copy(const string& data) {
  return copy(data.data(), data.size());
}

Buffer& Buffer::copy(const char* data) {
  return copy(data, strlen(data));
}

Buffer& Buffer::copy(const void* data, size_t size) {
  Buffer* buffer = new Buffer(getpagesize(), size);
  memcpy_s(*buffer, buffer->capacity(), data, size);
  return *buffer;
}

Buffer& Buffer::copy(const Buffer& data) {
  return copy(data, data.capacity());
}

size_t Buffer::getpagesize() {
  if (pagesize != 0)
    return pagesize;
  else {
#ifdef _WIN32
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    pagesize = system_info.dwPageSize;
#else
    pagesize = ::getpagesize();
#endif
    return pagesize;
  }
}

bool Buffer::is_page_aligned() const {
  return is_page_aligned(data());
}

bool Buffer::is_page_aligned(const void* ptr) {
  return (
           reinterpret_cast<const uintptr_t>(ptr)
           &
           (getpagesize() - 1)
         ) == 0;
}

bool Buffer::is_page_aligned(const iovec& iov) {
  return is_page_aligned(iov.iov_base)
         &&
         (iov.iov_len & (getpagesize() - 1)) == 0;
}
}
