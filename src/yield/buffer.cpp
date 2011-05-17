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
#include <ostream>
#include <stdlib.h>
#endif

namespace yield {
size_t Buffer::pagesize = 0;

Buffer::Buffer(size_t capacity) {
  alloc(ALIGNMENT_DEFAULT, capacity);
  next_buffer = NULL;
  size_ = 0;
}

Buffer::Buffer(size_t alignment, size_t capacity) {
  alloc(alignment, capacity);
  next_buffer = NULL;
  size_ = 0;
}

Buffer::Buffer(size_t capacity, void* data)
  : capacity_(capacity),
    data_(data)
{
  next_buffer = NULL;
}

Buffer::~Buffer() {
#ifdef _WIN32
  _aligned_free(data_);
#else
  free(data_);
#endif
  Buffer::dec_ref(next_buffer);
}

void Buffer::alloc(size_t alignment, size_t capacity) {
  capacity_ = capacity;
#ifdef _WIN32
  if ((data_ = _aligned_malloc(capacity_, alignment)) == NULL)
#else
  if (posix_memalign(&data_, alignment, capacity_) != 0)
#endif
    throw std::bad_alloc();
}

iovec Buffer::as_read_iovec() {
  iovec read_iovec;
  read_iovec.iov_base = static_cast<char*>(data_) + size();
  read_iovec.iov_len = capacity_ - size_;
  return read_iovec;
}

iovec Buffer::as_write_iovec() const {
  iovec write_iovec;
  write_iovec.iov_base = data_;
  write_iovec.iov_len = size_;
  return write_iovec;
}

Buffer&
Buffer::copy(
  size_t alignment,
  size_t capacity,
  const void* data,
  size_t size
) {
  Buffer* buffer = new Buffer(alignment, capacity);
  memcpy_s(*buffer, buffer->capacity(), data, size);
  buffer->size_ = size;
  return *buffer;
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

bool Buffer::is_page_aligned(const void* ptr) {
  return (
           reinterpret_cast<const uintptr_t>(ptr)
           &
           (getpagesize() - 1)
         ) == 0;
}

std::ostream& operator<<(std::ostream& os, const Buffer& buffer) {
  string data_str(1, '\"');
  if (buffer.size() > 0) {
    const uint8_t* data = static_cast<const uint8_t*>(buffer.data());
    
    for (uint8_t data_range_i = 0; data_range_i < 2; ++data_range_i) {
      size_t data_i_min, data_i_max;
      if (buffer.size() <= 131) {
        if (data_range_i == 0) {
          data_i_min = 0; data_i_max = buffer.size();
        } else
          break;
      } else {
        if (data_range_i == 0) {
          data_i_min = 0; data_i_max = 64;
        } else {
          data_str.append("...");
          data_i_min = buffer.size() - 64; data_i_max = buffer.size();
        }
      }

      for (size_t data_i = data_i_min; data_i < data_i_max; ++data_i) {
        if (
          data[data_i] == '\r'
          ||
          data[data_i] == '\n'
          ||
          (data[data_i] >= 32 && data[data_i] <= 126)
        )
          data_str.append(1, static_cast<char>(data[data_i]));
        else
          data_str.append(1, '?');
      }
    }
  }
  data_str.append(1, '\"');

  os <<
    buffer.get_type_name() <<
    "(" <<
      "capacity=" << buffer.capacity() <<
      ", " <<
      "data=" << data_str <<
      ", " <<
      "next_buffer=" << buffer.get_next_buffer() <<
      ", " <<
      "size=" << buffer.size() <<
    ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Buffer* buffer) {
  if (buffer != NULL)
    return operator<<(os, *buffer);
  else {
    os << "NULL";
    return os;
  }
}

bool Buffer::operator==(const Buffer& other) const {
  if (size() == other.size())
    return memcmp(data(), other.data(), size()) == 0;
  else
    return false;
}

bool Buffer::operator==(const char* other) const {
  if (size() == strlen(other))
    return memcmp(data(), other, size()) == 0;
  else
    return 0;
}

bool Buffer::operator==(const string& other) const {
  if (size() == other.size())
    return memcmp(data(), other.data(), size()) == 0;
  else
    return false;
}

void Buffer::put(const void* data, size_t size) {
  if (size_ + size <= capacity_) {
    if (
      data != NULL
      &&
      (data < data_ || data > static_cast<char*>(data_) + capacity_)
    ) {
      memcpy_s(
        static_cast<char*>(data_) + size_,
        capacity_ - size_,
        data,
        size
      );
    }

    size_ += size;
  }
}

void Buffer::set_next_buffer(YO_NEW_REF Buffer* next_buffer) {
  if (next_buffer != NULL)
    set_next_buffer(*next_buffer);
  else if (this->next_buffer != NULL) {
    Buffer::dec_ref(*this->next_buffer);
    this->next_buffer = NULL;
  }
}

void Buffer::set_next_buffer(YO_NEW_REF Buffer& next_buffer) {
  Buffer::dec_ref(this->next_buffer);
  this->next_buffer = &next_buffer;
}
}
