// yield/marshal/json/json_marshaller.cpp

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
#include "yield/marshal/null.hpp"
#include "yield/marshal/string.hpp"
#include "yield/marshal/string_literal.hpp"
#include "yield/marshal/json/json_marshaller.hpp"

#include <sstream> // For std::ostringstream


namespace yield {
namespace marshal {
namespace json {
void JSONMarshaller::write(const Object& key, bool value) {
  write_key(key);
  JSONEncoder::write(value);
}

void JSONMarshaller::write(const Object& key, double value) {
  write_key(key);
  JSONEncoder::write(value);
}

void JSONMarshaller::write(const Object& key, int64_t value) {
  write_key(key);
  JSONEncoder::write(value);
}

void JSONMarshaller::write(const Object& key, const Null& value) {
  write_key(key);
  JSONEncoder::write(static_cast<const char*>(NULL));
}

void JSONMarshaller::write(const Object& key, const Object& value) {
  write_key(key);

  JSONEncoder::open_object();
  in_object_stack.push(true);

  value.marshal(*this);

  in_object_stack.pop();
  JSONEncoder::close_object();
}

void JSONMarshaller::write(const Object& key, const Sequence& value) {
  write_key(key);

  JSONEncoder::open_array();
  in_object_stack.push(false);

  value.marshal(*this);

  in_object_stack.pop();
  JSONEncoder::close_array();
}

void
JSONMarshaller::write
(
  const Object& key,
  const char* value,
  size_t value_len
) {
  write_key(key);
  JSONEncoder::write(value, value_len);
}

void JSONMarshaller::write(const Object& key, uint64_t value) {
  write_key(key);
  JSONEncoder::write(value);
}

void JSONMarshaller::write_key(const Object& key) {
  if (!in_object_stack.empty() && in_object_stack.top()) {
    switch (key.get_type_id()) {
    case Double::TYPE_ID: {
      std::ostringstream json_key;
      json_key << static_cast<const Double&>(key);
      JSONEncoder::write(json_key.str());
    }
    break;

    case Integer::TYPE_ID: {
      std::ostringstream json_key;
      json_key << static_cast<const Double&>(key);
      JSONEncoder::write(json_key.str());
    }
    break;

    case Null::TYPE_ID:
      break;

    case String::TYPE_ID: {
      JSONEncoder::write(static_cast<const String&>(key));
    }
    break;

    case StringLiteral::TYPE_ID: {
      JSONEncoder::write(static_cast<const StringLiteral&>(key));
    }
    break;
    }
  }
}
}
}
}
