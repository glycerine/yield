// yield/marshal/xdr/xdr_encoder.cpp

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
#include "yield/marshal/xdr/xdr_encoder.hpp"


namespace yield {
namespace marshal {
namespace xdr {
#if !defined( htonl ) && !defined( _WINSOCKAPI_) && !defined(_WINSOCK2API_)
static inline uint32_t htonl(uint32_t x) {
#ifdef __BIG_ENDIAN__
  return x;
#else
  return (x >> 24) |
         ((x << 8) & 0x00FF0000) |
         ((x >> 8) & 0x0000FF00) |
         (x << 24);
#endif
}
#endif

static inline uint64_t htonll(uint64_t x) {
#ifdef __BIG_ENDIAN__
  return x;
#else
  return (x >> 56) |
         ((x << 40) & 0x00FF000000000000ULL) |
         ((x << 24) & 0x0000FF0000000000ULL) |
         ((x << 8)  & 0x000000FF00000000ULL) |
         ((x >> 8)  & 0x00000000FF000000ULL) |
         ((x >> 24) & 0x0000000000FF0000ULL) |
         ((x >> 40) & 0x000000000000FF00ULL) |
         (x << 56);
#endif
}


XDREncoder::XDREncoder()
  : buffer(*new StringBuffer)
{ }

XDREncoder::XDREncoder(Buffer& buffer)
  : buffer(buffer.inc_ref())
{ }

XDREncoder::~XDREncoder() {
  Buffer::dec_ref(buffer);
}

void XDREncoder::write(bool value) {
  write(value ? 1 : 0);
}

void XDREncoder::write(double value) {
  uint64_t uint64_value;

  memcpy_s
  (
    &uint64_value,
    sizeof(uint64_value),
    &value,
    sizeof(value)
  );

  write(uint64_value);
}

void XDREncoder::write(float value) {
  uint32_t uint32_value;

  memcpy_s
  (
    &uint32_value,
    sizeof(uint32_value),
    &value,
    sizeof(value)
  );

  write(uint32_value);
}

void XDREncoder::write(int32_t value) {
  write(static_cast<uint32_t>(value));
}

void XDREncoder::write(int64_t value) {
  write(static_cast<uint64_t>(value));
}

void XDREncoder::write(const char* value, size_t value_len) {
  write(static_cast<int32_t>(value_len));

  write(static_cast<const void*>(value), value_len);

  if (value_len % 4 != 0) {
    static char zeros[] = { 0, 0, 0 };
    write
    (
      static_cast<const void*>(zeros),
      4 - (value_len % 4)
    );
  }
}

void XDREncoder::write(uint32_t value) {
  value = htonl(value);
  write(&value, sizeof(value));
}

void XDREncoder::write(uint64_t value) {
  value = htonll(value);
  write(&value, sizeof(value));
}

void XDREncoder::write(const void* value, size_t value_len) {
  buffer.put(value, value_len);
}
}
}
}
