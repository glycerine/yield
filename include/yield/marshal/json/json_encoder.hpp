// yield/marshal/json/json_encoder.hpp

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

#ifndef _YIELD_MARSHAL_JSON_JSON_ENCODER_HPP_
#define _YIELD_MARSHAL_JSON_JSON_ENCODER_HPP_

#include "yield/buffer.hpp"


struct yajl_gen_t;
typedef struct yajl_gen_t* yajl_gen;


namespace yield {
namespace marshal {
namespace json {
class JSONEncoder {
public:
  JSONEncoder();
  JSONEncoder(Buffer& buffer);
  ~JSONEncoder();

  void close_array();
  void close_object();

  Buffer& get_buffer() const {
    return buffer;
  }

  void open_array();
  void open_object();

  void write(bool value);
  void write(double value);
  void write(int64_t value);
  void write(const char* value);
  void write(const string& value);
  void write(const char* value, size_t value_len);
  void write(uint64_t value);

private:
  void flush();

private:
  Buffer& buffer;
  yajl_gen writer;
};
}
}
}

#endif
