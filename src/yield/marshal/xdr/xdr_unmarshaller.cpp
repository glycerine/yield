// yield/marshal/xdr/xdr_unmarshaller.cpp

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

#include "yield/assert.hpp"
#include "yield/marshal/double.hpp"
#include "yield/marshal/integer.hpp"
#include "yield/marshal/map.hpp"
#include "yield/marshal/null.hpp"
#include "yield/marshal/sequence.hpp"
#include "yield/marshal/string.hpp"
#include "yield/marshal/xdr/xdr_unmarshaller.hpp"


namespace yield {
namespace marshal {
namespace xdr {
XDRUnmarshaller::XDRUnmarshaller(const Buffer& xdr)
  : XDRDecoder(xdr)
{ }

XDRUnmarshaller::XDRUnmarshaller(const uint8_t* xdr, size_t xdr_len)
  : XDRDecoder(xdr, xdr_len)
{ }

bool XDRUnmarshaller::read_bool(const Object& key) {
  return XDRDecoder::read_bool();
}

double XDRUnmarshaller::read_double(const Object& key) {
  return XDRDecoder::read_double();
}

float XDRUnmarshaller::read_float(const Object& key) {
  return XDRDecoder::read_float();
}

int32_t XDRUnmarshaller::read_int32(const Object& key) {
  return XDRDecoder::read_int32();
}

int64_t XDRUnmarshaller::read_int64(const Object& key) {
  return XDRDecoder::read_int64();
}

void XDRUnmarshaller::read_key(Object& key) {
  switch (key.get_type_id()) {
  case Double::TYPE_ID: {
    static_cast<Double&>(key) = read_double(Null());
  }
  break;

  case Integer::TYPE_ID: {
    DebugBreak(); // int32 or int64?
    static_cast<Integer&>(key) = read_int64(Null());
  }
  break;

  case String::TYPE_ID: {
    read_string(Null(), static_cast<String&>(key));
  }
  break;

  default:
    DebugBreak();
    break;
  }
}

void XDRUnmarshaller::read_map(const Object& key, Map& value) {
  size_t size = XDRDecoder::read_uint32();
  if (size < UINT16_MAX) {
    for (size_t i = 0; i < size; i++)
      value.unmarshal(*this);
  }
}

void XDRUnmarshaller::read_object(const Object& key, Object& value) {
  value.unmarshal(*this);
}

void XDRUnmarshaller::read_sequence(const Object& key, Sequence& value) {
  size_t size = XDRDecoder::read_uint32();
  if (size <= UINT16_MAX) {
    for (size_t i = 0; i < size; i++)
      value.unmarshal(*this);
  }
}

void XDRUnmarshaller::read_string(const Object& key, string& value) {
  XDRDecoder::read_string(value);
}

uint32_t XDRUnmarshaller::read_uint32(const Object& key) {
  return XDRDecoder::read_uint32();
}

uint64_t XDRUnmarshaller::read_uint64(const Object& key) {
  return XDRDecoder::read_uint64();
}
}
}
}
