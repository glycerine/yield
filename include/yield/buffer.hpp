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
  const static uint32_t TYPE_ID = 4078143893UL;

public:
  virtual ~Buffer();

public:
  virtual size_t capacity() const;

  virtual void* data() = 0;

  virtual const void* data() const = 0;

  bool empty() const {
    return size() == 0;
  }

  Buffer* get_next_buffer() const {
    return next_buffer;
  }

public:
  operator char* () {
    return static_cast<char*>(data());
  }

  operator uint8_t* () {
    return static_cast<uint8_t*>(data());
  }

  operator void* () {
    return data();
  }

  operator const char* () const;

  operator const uint8_t* () const;

  operator const void* () const {
    return data();
  }

  char operator[](int n) {
    return static_cast<char*>(*this)[n];
  }

  bool operator==(const Buffer&) const;
  bool operator!=(const Buffer& other) const;

public:
  void put(const Buffer&);
  void put(char buf, size_t repeat_count);
  void put(const iovec& iov);
  void put(const char* buf);
  void put(const string& buf);
  virtual void put(const void* buf, size_t len);

public:
  virtual void reserve(size_t capacity) = 0;
  void resize(size_t new_size);

public:
  void set_next_buffer(YO_NEW_REF Buffer* next_buffer);
  void set_next_buffer(YO_NEW_REF Buffer& next_buffer);

public:
  size_t size() const {
    return size_;
  }

public:
  // yield::Object
  virtual uint32_t get_type_id() const {
    return TYPE_ID;
  }

  virtual const char* get_type_name() const {
    return "yield::Buffer";
  }

  Buffer& inc_ref() {
    return Object::inc_ref(*this);
  }

private:
  Buffer* next_buffer;
  size_t size_;
};
}

#endif
