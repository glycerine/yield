// yield/marshal/json/json_encoder.cpp

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


#ifdef _WIN32
extern "C"
{
#endif
#include "yajl.h"
#ifdef _WIN32
};
#endif

#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/string_buffer.hpp"
#include "yield/marshal/json/json_encoder.hpp"


namespace yield {
namespace marshal {
namespace json {
JSONEncoder::JSONEncoder()
  : buffer(*new StringBuffer) {
  writer = yajl_gen_alloc(NULL);
  if (writer == NULL)
    throw Exception();
}

JSONEncoder::JSONEncoder(Buffer& buffer)
  : buffer(buffer.inc_ref()) {
  writer = yajl_gen_alloc(NULL);
}

JSONEncoder::~JSONEncoder() {
  Buffer::dec_ref(buffer);
}

void JSONEncoder::close_array() {
  yajl_gen_array_close(writer);
  flush();
}

void JSONEncoder::close_object() {
  yajl_gen_map_close(writer);
  flush();
}

void JSONEncoder::flush() {
  const unsigned char* buffer;
  unsigned int len;
  yajl_gen_get_buf(writer, &buffer, &len);
  this->buffer.put(buffer, len);
  yajl_gen_clear(writer);
}

void JSONEncoder::open_array() {
  yajl_gen_array_open(writer);
}

void JSONEncoder::open_object() {
  yajl_gen_map_open(writer);
}

void JSONEncoder::write(bool value) {
  yajl_gen_bool(writer, static_cast<int>(value));
  flush();
}

void JSONEncoder::write(double value) {
  yajl_gen_double(writer, value);
  flush();
}

void JSONEncoder::write(int64_t value) {
  yajl_gen_integer(writer, static_cast<long>(value));
}

void JSONEncoder::write(const char* value) {
  if (value != NULL)
    write(value, strnlen(value, UINT16_MAX));
  else {
    yajl_gen_null(writer);
    flush();
  }
}

void JSONEncoder::write(const string& value) {
  write(value.data(), value.size());
}

void JSONEncoder::write(const char* value, size_t value_len) {
  yajl_gen_string
  (
    writer,
    reinterpret_cast<const unsigned char*>(value),
    strnlen(value, UINT16_MAX)
  );

  flush();
}

void JSONEncoder::write(uint64_t value) {
  yajl_gen_integer(writer, static_cast<long>(value));
}
}
}
}
