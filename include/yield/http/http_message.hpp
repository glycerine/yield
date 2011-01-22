// yield/http/http_message.hpp

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


#ifndef _YIELD_HTTP_HTTP_MESSAGE_HPP_
#define _YIELD_HTTP_HTTP_MESSAGE_HPP_


#include "yield/buffer.hpp"
#include "yield/date_time.hpp"

#include <utility> // for std::pair


namespace yield {
namespace http {
template <class HTTPMessageType>
class HTTPMessage {
public:
  const static size_t CONTENT_LENGTH_CHUNKED = SIZE_MAX;

public:
  void* get_body() const {
    return body;
  }
  size_t get_content_length() const {
    return content_length;
  }
  DateTime get_date_field(const char* name = "Date") const;
  bool get_field(const char* name, OUT iovec& value) const;
  string get_field(const char* name, const char* default_val = "") const;
  float get_http_version() const {
    return http_version;
  }
  bool has_field(const char* name) const;

  operator Buffer& ();

  string operator[](const char* name) {
    return get_field(name);
  }

  HTTPMessageType& set_field(const char* name, const char* value);
  HTTPMessageType& set_field(const string& name, const string& value);
  HTTPMessageType& set_field(const char* name, const iovec& value);
  HTTPMessageType& set_field(const char* name, const DateTime& value);
  HTTPMessageType& set_field(const char* name, size_t value);

  HTTPMessageType&
  set_field
  (
    const char* name,
    const char* value,
    size_t value_len
  );

  HTTPMessageType&
  set_field
  (
    const char* name,
    size_t name_len,
    const char* value,
    size_t value_len
  );

protected:
  HTTPMessage
  (
    void* body,
    Buffer& buffer,
    size_t content_length,
    uint16_t fields_offset,
    float http_version
  );

  HTTPMessage
  (
    YO_NEW_REF Buffer* body,
    float http_version
  );

  virtual ~HTTPMessage();

  void finalize();
  Buffer& get_buffer() {
    return buffer;
  }
  bool is_finalized() const;
  void mark_fields_offset();

private:
  void* body;
  Buffer& buffer;
  size_t content_length;
  uint16_t fields_offset;
  float http_version;
};
}
}


#endif
