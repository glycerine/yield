// yield/marshal/xdr/xdr_marshaller.cpp

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


#include "yield/assert.hpp"
#include "yield/marshal/double.hpp"
#include "yield/marshal/integer.hpp"
#include "yield/marshal/string.hpp"
#include "yield/marshal/string_literal.hpp"
#include "yield/marshal/xdr/xdr_marshaller.hpp"


namespace yield {
namespace marshal {
namespace xdr {
void XDRMarshaller::write(const Object& key, bool value) {
  write_key(key);
  XDREncoder::write(value);
}

void XDRMarshaller::write(const Object& key, double value) {
  write_key(key);
  XDREncoder::write(value);
}

void XDRMarshaller::write(const Object& key, float value) {
  write_key(key);
  XDREncoder::write(value);
}


void XDRMarshaller::write(const Object& key, int32_t value) {
  write_key(key);
  XDREncoder::write(value);
}

void XDRMarshaller::write(const Object& key, int64_t value) {
  write_key(key);
  XDREncoder::write(value);
}

void XDRMarshaller::write(const Object& key, const Map& value) {
  write_key(key);
  XDREncoder::write(static_cast<uint32_t>(value.get_size()));
  in_map_stack.push(true);
  value.marshal(*this);
  in_map_stack.pop();
}

void XDRMarshaller::write(const Object& key, const Null& value) {
  write_key(key);
  XDREncoder::write(static_cast<uint32_t>(0));
}

void XDRMarshaller::write(const Object& key, const Object& value) {
  write_key(key);
  value.marshal(*this);
}

void XDRMarshaller::write(const Object& key, const Sequence& value) {
  write_key(key);
  XDREncoder::write(static_cast<uint32_t>(value.get_size()));
  value.marshal(*this);
}

void XDRMarshaller::write(const Object& key, const char* value, size_t value_len) {
  write_key(key);
  XDREncoder::write(value, value_len);
}

void XDRMarshaller::write(const Object& key, uint32_t value) {
  write_key(key);
  XDREncoder::write(value);
}

void XDRMarshaller::write(const Object& key, uint64_t value) {
  write_key(key);
  XDREncoder::write(value);
}

void XDRMarshaller::write_key(const Object& key) {
  if (!in_map_stack.empty() && in_map_stack.top()) {
    switch (key.get_type_id()) {
    case Double::TYPE_ID: {
      XDREncoder::write(static_cast<const Double&>(key));
    }
    break;

    case Integer::TYPE_ID: {
      XDREncoder::write(static_cast<const Integer&>(key));
    }
    break;

    case String::TYPE_ID: {
      XDREncoder::write
      (
        static_cast<const String&>(key).data(),
        static_cast<const String&>(key).size()
      );
    }
    break;

    case StringLiteral::TYPE_ID: {
      XDREncoder::write
      (
        static_cast<const StringLiteral&>(key),
        strlen(static_cast<const StringLiteral&>(key))
      );
    }
    break;

    default:
      DebugBreak();
      break;
    }
  }
}
}
}
}
