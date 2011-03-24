// yield/fixed_buffer.hpp

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

#ifndef _YIELD_FIXED_BUFFER_HPP_
#define _YIELD_FIXED_BUFFER_HPP_

#include "yield/assert.hpp"
#include "yield/buffer.hpp"


namespace yield {
template <size_t Capacity>
class FixedBuffer : public Buffer {
public:
  FixedBuffer()
    : Buffer(Capacity)
  { }

  FixedBuffer(const string& data)
    : Buffer(Capacity) {
    put(data);
  }

  FixedBuffer(const void* data, size_t size)
    : Buffer(Capacity) {
    put(data, size);
  }

  FixedBuffer(const Buffer& buffer)
    : Buffer(Capacity) {
    put(buffer);
  }

  FixedBuffer(const FixedBuffer<Capacity>& buffer)
    : Buffer(Capacity) {
    put(buffer);
  }

  virtual ~FixedBuffer() { }

public:
  // yield::Buffer
  void* data() {
    return &data_[0];
  }

  const void* data() const {
    return &data_[0];
  }

  void reserve(size_t new_capacity) {
    debug_assert_le(new_capacity, Capacity);
  }

private:
  uint8_t data_[Capacity];
};
}

#endif
