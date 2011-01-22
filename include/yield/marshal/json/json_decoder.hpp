// yield/marshal/json/json_decoder.hpp

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


#ifndef _YIELD_MARSHAL_JSON_JSON_DECODER_HPP_
#define _YIELD_MARSHAL_JSON_JSON_DECODER_HPP_


#include "yield/marshal/object.hpp"

#include <stack>


struct yajl_handle_t;
typedef struct yajl_handle_t* yajl_handle;


namespace yield {
class Buffer;


namespace marshal {
class String;


namespace json {
class JSONDecoder {
public:
  JSONDecoder();
  ~JSONDecoder();

  YO_NEW_REF Object* decode(const Buffer& json);
  YO_NEW_REF Object* decode(const char* json, size_t json_len);

private:
  void handle(Object&);

  // yajl callbacks
  static int yajl_boolean(void*, int value);
  int yajl_boolean(int value);
  static int yajl_double(void*, double value);
  int yajl_double(double value);
  static int yajl_end_array(void*);
  int yajl_end_array();
  static int yajl_end_map(void*);
  int yajl_end_map();
  static int yajl_integer(void*, long value);
  int yajl_integer(long value);
  static int yajl_map_key(void*, const uint8_t*, unsigned int);
  int yajl_map_key(const uint8_t*, unsigned int);
  static int yajl_null(void*);
  int yajl_null();
  static int yajl_start_array(void*);
  int yajl_start_array();
  static int yajl_start_map(void*);
  int yajl_start_map();
  static int yajl_string(void*, const uint8_t*, unsigned int);
  int yajl_string(const uint8_t*, unsigned int);

private:
  Object* JSONfalse, *JSONnull, *JSONtrue;
  std::stack<Object*> object_stack;
  String* next_map_key;
  yajl_handle reader;
};
};
};
};


#endif
