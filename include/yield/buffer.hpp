// yield/buffer.hpp

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

#ifndef _YIELD_BUFFER_HPP_
#define _YIELD_BUFFER_HPP_

#include "yield/object.hpp"


namespace yield {
class Buffer : public Object {
public:
  const static size_t ALIGNMENT_DEFAULT = 2;
  const static uint32_t TYPE_ID = 4078143893UL;

public:
  Buffer(size_t capacity);
  Buffer(size_t alignment, size_t capacity);

  virtual ~Buffer();

public:
  Buffer& copy() const {
    return copy(*this);
  }

  static Buffer& copy(const Buffer& data) {
    return copy(data, data.capacity());
  }

  static Buffer& copy(const string& data) {
    return copy(data.data(), data.size());
  }

  static Buffer& copy(const char* data) {
    return copy(data, strlen(data));
  }

  static Buffer& copy(const void* data, size_t size) {
    return copy(size, data, size);
  }

  static Buffer& copy(size_t capacity, const void* data, size_t size) {
    return copy(ALIGNMENT_DEFAULT, capacity, data, size);
  }

  static Buffer&
  copy(
    size_t alignment,
    size_t capacity,
    const void* data,
    size_t size
  );

public:
  size_t capacity() const {
    return capacity_;
  }

  void* data() {
    return data_;
  }

  bool empty() const {
    return size() == 0;
  }

  const void* data() const {
    return data_;
  }

  size_t size() const {
    return size_;
  }

public:
  Buffer* get_next_buffer() {
    return next_buffer;
  }

public:
  static size_t getpagesize();

  bool is_page_aligned() const {
    return is_page_aligned(capacity(), data());
  }

  static bool is_page_aligned(const iovec& iov) {
    return is_page_aligned(iov.iov_len, iov.iov_base);
  }

  static bool is_page_aligned(const void* data);
  static bool is_page_aligned(size_t capacity, const void* data);

public:
  operator char* () {
    return static_cast<char*>(data());
  }

  operator const char* () const {
    return static_cast<const char*>(data());
  }

  operator uint8_t* () {
    return static_cast<uint8_t*>(data());
  }

  operator const uint8_t* () const {
    return static_cast<const uint8_t*>(data());
  }

  operator void* () {
    return data();
  }

  operator const void* () const {
    return data();
  }

  char operator[](int n) {
    return static_cast<char*>(*this)[n];
  }

  bool operator==(const Buffer&) const;

 	bool operator!=(const Buffer& other) const {
    return !operator==(other);
  }

public:
  void put(const Buffer& data) {
    put(data, data.size());
  }

 	void put(const char* data) {
    put(data, strlen(data));
  }

 	void put(const string& data) {
    put(data.c_str(), data.size());
  }

 	void put(char data, size_t repeat_count);
 	void put(const void* data, size_t size);

public:
  void resize(size_t size) {
    if (size < capacity())
      this->size_ = size;
  }

  void set_next_buffer(YO_NEW_REF Buffer*);
  void set_next_buffer(YO_NEW_REF Buffer&);

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::Buffer";
  }

  Buffer& inc_ref() {
    return Object::inc_ref(*this);
  }

protected:
  Buffer(size_t capacity, void* data);

protected:
  void* data_;

private:
  void alloc(size_t alignment, size_t capacity);

private:
  size_t capacity_;
  static size_t pagesize;
  Buffer* next_buffer;
  size_t size_;
};
}

#endif
