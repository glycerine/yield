// yield/marshal/xdr/xdr_decoder.cpp

// Copyright (c) 2010 Minor Gordon
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
#include "yield/marshal/xdr/xdr_decoder.hpp"


namespace yield {
namespace marshal {
namespace xdr {
#if !defined( ntohl ) && !defined( _WINSOCKAPI_ ) && !defined( _WINSOCK2API_ )
static inline uint32_t ntohl(uint32_t x) {
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

static inline uint64_t ntohll(uint64_t x) {
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


XDRDecoder::XDRDecoder(const Buffer& xdr) {
  xdr_p = xdr;
  xdr_pe = xdr_p + xdr.size();
}

XDRDecoder::XDRDecoder(const uint8_t* xdr, size_t xdr_len) {
  xdr_p = xdr;
  xdr_pe = xdr_p + xdr_len;
}

void XDRDecoder::read(void* buf, size_t len) {
  if (xdr_p + len <= xdr_pe) {
    memcpy_s(buf, len, xdr_p, len);
    xdr_p += len;
  }
}

bool XDRDecoder::read_bool() {
  return read_uint32() == 1;
}

//void XDRDecoder::read_buffer( Buffer& value )
//{
//  size_t size = read_int32();
//  if ( value.capacity() - value.size() < size )
//    size = value.capacity() - value.size();
//
//  if ( size > 0 )
//  {
//    read( static_cast<char*>( value ) + value.size(), size );
//    value.resize( value.size() + size );
//
//    if ( size % 4 != 0 )
//    {
//      char zeros[3];
//      read( zeros, 4 - ( size % 4 ) );
//    }
//  }
//}
//
//Buffer* XDRDecoder::read_buffer()
//{
//  size_t size = read_int32();
//
//  StringBuffer* value = new StringBuffer( size );
//
//  read( static_cast<void*>( value ), size );
//  value->resize( size );
//
//  if ( size % 4 != 0 )
//  {
//    char zeros[3];
//    read( zeros, 4 - ( size % 4 ) );
//  }
//
//  return value;
//}

double XDRDecoder::read_double() {
  uint64_t uint64_value = read_uint64();

  double value;
  memcpy_s
  (
    &value,
    sizeof(value),
    &uint64_value,
    sizeof(uint64_value)
  );

  return value;
}

float XDRDecoder::read_float() {
  uint32_t uint32_value = read_uint32();

  float value;
  memcpy_s
  (
    &value,
    sizeof(value),
    &uint32_value,
    sizeof(uint32_value)
  );

  return value;
}

int32_t XDRDecoder::read_int32() {
  return static_cast<int32_t>(read_uint32());
}

int64_t XDRDecoder::read_int64() {
  return static_cast<int64_t>(read_uint64());
}

void XDRDecoder::read_string(string& value) {
  size_t size = read_uint32();
  if (size > 0 && size < UINT16_MAX) {
    size_t padded_size = size % 4;
    if (padded_size == 0)
      padded_size = size;
    else
      padded_size = size + 4 - padded_size;

    value.resize(padded_size);
    read(const_cast<char*>(value.data()), padded_size);
    value.resize(size);
  }
}

uint32_t XDRDecoder::read_uint32() {
  uint32_t value;
  read(&value, sizeof(value));
  return ntohl(value);
}

uint64_t XDRDecoder::read_uint64() {
  uint64_t value;
  read(&value, sizeof(value));
  return ntohll(value);
}
}
}
}
