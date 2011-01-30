// yield/marshal/json/json_decoder.cpp

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

#include "object_map.hpp"
#include "object_vector.hpp"

#ifdef _WIN32
extern "C"
{
#endif
#include "yajl.h"
#ifdef _WIN32
};
#endif

#include "yield/assert.hpp"
#include "yield/buffer.hpp"
#include "yield/marshal/boolean.hpp"
#include "yield/marshal/double.hpp"
#include "yield/marshal/integer.hpp"
#include "yield/marshal/null.hpp"
#include "yield/marshal/string.hpp"
#include "yield/marshal/string_literal.hpp"
#include "yield/marshal/json/json_decoder.hpp"


namespace yield {
namespace marshal {
namespace json {
JSONDecoder::JSONDecoder() {
  JSONfalse = new Boolean(false);
  JSONnull = new Null;
  JSONtrue = new Boolean(true);

  static yajl_callbacks JSONDecoder_yajl_callbacks
  = {
    yajl_null,
    yajl_boolean,
    yajl_integer,
    yajl_double,
    NULL,
    yajl_string,
    yajl_start_map,
    yajl_map_key,
    yajl_end_map,
    yajl_start_array,
    yajl_end_array
  };

  reader = yajl_alloc(&JSONDecoder_yajl_callbacks, NULL, this);
}

JSONDecoder::~JSONDecoder() {
  Object::dec_ref(*JSONfalse);
  Object::dec_ref(*JSONnull);
  Object::dec_ref(*JSONtrue);
  yajl_free(reader);
}

YO_NEW_REF Object* JSONDecoder::decode(const Buffer& buffer) {
  return decode(buffer, buffer.size());
}

YO_NEW_REF Object*
JSONDecoder::decode
(
  const char* json_text,
  size_t json_text_len
) {
  next_map_key = NULL;
  yajl_status yajl_parse_status
  = yajl_parse
    (
      reader,
      reinterpret_cast<const uint8_t*>(json_text),
      json_text_len
    );

  if (yajl_parse_status == yajl_status_ok) {
    if (!object_stack.empty()) {
      debug_assert_eq(object_stack.size(), 1);
      Object* object = object_stack.top();
      object_stack.pop();
      return object;
    } else
      return new ObjectMap;
  } else if (yajl_parse_status == yajl_status_insufficient_data) {
    DebugBreak();
    return NULL;
  } else {
    //uint8_t* yajl_error_str
    //  = yajl_get_error( reader, 1, json_text, json_text_len );
    //ostringstream what;
    //what << __FILE__ << ":" << __LINE__ << ": JSON parsing error: "
    //  << reinterpret_cast<char*>( yajl_error_str ) << endl;
    //yajl_free_error( yajl_error_str );
    return NULL;
  }
}

void JSONDecoder::handle(Object& object) {
  if (!object_stack.empty()) {
    switch (object_stack.top()->get_type_id()) {
    case ObjectMap::TYPE_ID: {
      debug_assert_ne(next_map_key, NULL);
      static_cast<ObjectMap*>(object_stack.top())->
      insert(std::make_pair(next_map_key, &object));
      next_map_key = NULL;
    }
    break;

    case ObjectVector::TYPE_ID: {
      static_cast<ObjectVector*>(object_stack.top())->
      push_back(&object);
    }
    break;

    default:
      DebugBreak();
    }
  } else
    object_stack.push(&object);
}

int JSONDecoder::yajl_boolean(void* this_, int value) {
  return static_cast<JSONDecoder*>(this_)->yajl_boolean(value);
}

int JSONDecoder::yajl_boolean(int value) {
  if (value)
    handle(JSONtrue->inc_ref());
  else
    handle(JSONfalse->inc_ref());

  return 1;
}

int JSONDecoder::yajl_double(void* this_, double value) {
  return static_cast<JSONDecoder*>(this_)->yajl_double(value);
}

int JSONDecoder::yajl_double(double value) {
  handle(*new Double(value));
  return 1;
}

int JSONDecoder::yajl_end_array(void* this_) {
  return static_cast<JSONDecoder*>(this_)->yajl_end_array();
}

int JSONDecoder::yajl_end_array() {
  debug_assert_eq
  (
    object_stack.top()->get_type_id(),
    ObjectVector::TYPE_ID
  );
  object_stack.pop();
  return 1;
}

int JSONDecoder::yajl_end_map(void* this_) {
  return static_cast<JSONDecoder*>(this_)->yajl_end_map();
}

int JSONDecoder::yajl_end_map() {
  debug_assert_eq
  (
    object_stack.top()->get_type_id(),
    ObjectMap::TYPE_ID
  );
  object_stack.pop();
  return 1;
}

int JSONDecoder::yajl_integer(void* this_, long value) {
  return static_cast<JSONDecoder*>(this_)
         ->yajl_integer(value);
}

int JSONDecoder::yajl_integer(long value) {
  handle(*new Integer(value));
  return 1;
}

int JSONDecoder::yajl_map_key
(
  void* this_,
  const uint8_t* map_key,
  unsigned int map_key_len
) {
  return static_cast<JSONDecoder*>(this_)
         ->yajl_map_key(map_key, map_key_len);
}


int JSONDecoder::yajl_map_key
(
  const uint8_t* map_key,
  unsigned int map_key_len
) {
  next_map_key
  = new String(reinterpret_cast<const char*>(map_key), map_key_len);
  return 1;
}

int JSONDecoder::yajl_null(void* this_) {
  return static_cast<JSONDecoder*>(this_)->yajl_null();
}

int JSONDecoder::yajl_null() {
  handle(JSONnull->inc_ref());
  return 1;
}

int JSONDecoder::yajl_start_array(void* this_) {
  return static_cast<JSONDecoder*>(this_)->yajl_start_array();
}

int JSONDecoder::yajl_start_array() {
  ObjectVector* json_array = new ObjectVector;
  handle(*json_array);
  object_stack.push(json_array);
  return 1;
}

int JSONDecoder::yajl_start_map(void* this_) {
  return static_cast<JSONDecoder*>(this_)->yajl_start_map();
}

int JSONDecoder::yajl_start_map() {
  ObjectMap* json_object = new ObjectMap;
  handle(*json_object);
  object_stack.push(json_object);
  return 1;
}

int JSONDecoder::yajl_string
(
  void* this_,
  const uint8_t* buffer,
  unsigned int len
) {
  return static_cast<JSONDecoder*>(this_)
         ->yajl_string(buffer, len);
}

int JSONDecoder::yajl_string
(
  const uint8_t* buffer,
  unsigned int len
) {
  handle(*new String(reinterpret_cast<const char*>(buffer), len));
  return 1;
}
}
}
}
