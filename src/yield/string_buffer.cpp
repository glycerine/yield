// yield/string_buffer.cpp

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


#include "yield/string_buffer.hpp"


namespace yield {
StringBuffer::StringBuffer()
  : Buffer(0) {
  data_ = new uint8_t[0];
}

StringBuffer::StringBuffer(size_t capacity)
  : Buffer(capacity) {
  data_ = new uint8_t[capacity];
}

StringBuffer::StringBuffer(const string& data)
  : Buffer(0) {
  data_ = NULL;
  put(data);
}

StringBuffer::StringBuffer(const void* data, size_t size)
  : Buffer(0) {
  data_ = NULL;
  put(data, size);
}

StringBuffer::StringBuffer(size_t capacity, const void* data, size_t size)
  : Buffer(0) {
  data_ = NULL;
  reserve(capacity);
  put(data, size);
}

StringBuffer::StringBuffer(const Buffer& buffer)
  : Buffer(0) {
  data_ = NULL;
  put(buffer);
}

StringBuffer::StringBuffer(const StringBuffer& buffer)
  : Buffer(0) {
  data_ = NULL;
  put(buffer);
}

StringBuffer::~StringBuffer() {
  delete [] data_;
}

void StringBuffer::reserve(size_t new_capacity) {
  if (new_capacity > capacity()) {
    uint8_t* new_data = new uint8_t[new_capacity];
    memcpy_s(new_data, new_capacity, data_, size());
    delete [] data_;
    capacity_ = new_capacity;
    data_ = new_data;
  }
}
}
