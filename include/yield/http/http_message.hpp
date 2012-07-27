// yield/http/http_message.hpp

// Copyright (c) 2012 Minor Gordon
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

#include "yield/event.hpp"

#include <utility> // for std::pair

namespace yield {
class Buffer;
class DateTime;

namespace http {
/**
  An RFC 2616 HTTP message, the parent class of HTTPRequest and HTTPResponse.
*/
template <class HTTPMessageType>
class HTTPMessage : public Event {
public:
  /**
    The default HTTP version to use for outgoing HTTP messages, as a single
      byte (0 or 1, for HTTP/1.0 and HTTP/1.0, respectively).
  */
  const static uint8_t HTTP_VERSION_DEFAULT = 1;

  /**
    A constant returned by HTTPMessage::get_content_length that indicates
      that the HTTP message does not contain a Content-Length header,
      but has an unbounded Transfer-Encoding: chunked body.
  */
  const static size_t CONTENT_LENGTH_CHUNKED = SIZE_MAX;

public:
  /**
    Finalize an outgoing HTTP message, adding a trailing CRLF to its header.
    No more fields can be added to the header after this call is made.
  */
  void finalize();

public:
  /**
    Get the body of this HTTP message.
    This may be a Buffer*. The caller should first check if get_body is NULL,
      then switch on get_body()->get_type_id() to determine whether the body
      is a Buffer or some other object type.
    @return the body of this HTTP messag, or NULL if no body is present
  */
  Object* get_body() const {
    return body;
  }

public:
  /**
    Get the value of the Content-Length field of this HTTP message, or its
      equivalent.
    Returns CONTENT_LENGTH_CHUNKED if the Transfer-Encoding: chunked field is
      present in lieu of a fixed Content-Length.
    Returns 0 if no Content-Length and no Transfer-Encoding: chunked fields
      are present.
    @return the value of the Content-Length field of this HTTP message,
      or its equivalent
  */
  size_t get_content_length() const;

  /**
    Get a field value as a DateTime. HTTP has various date fields, the
      prototypical one being the mandatory "Date" timestamp on all server
      responses.
    This method will parse the various date-time formats mandated by RFC 2616.
    All HTTP dates-times are in UTC.
    @param name the field name
    @return the field value as a date time
  */
  DateTime get_date_field(const char* name = "Date") const;

  /**
    Get a field value, copying into a temporary string.
    @param name the field name
    @param default_value a default value to return if the field is not present
    @return the field value or default_value if the field is not present
  */
  string get_field(const char* name, const char* default_value = "") const {
    iovec value_iov;
    if (get_field(name, value_iov)) {
      return string(
               static_cast<char*>(value_iov.iov_base),
               value_iov.iov_len
             );
    } else {
      return default_value;
    }
  }

  /**
    Get a field value.
    @param name the field name
    @param[out] value a pointer and length to the field value in the
      HTTPMessage's underlying buffer
    @return true if the field is present, false if not
  */
  bool get_field(const char* name, iovec& value) const {
    return get_field(name, strlen(name), value);
  }

  /**
    Get a field value.
    @param name the field name
    @param name_len length in bytes of name
    @param[out] value a pointer and length to the field value in the
      HTTPMessage's underlying buffer
    @return true if the field is present, false if not
  */
  bool get_field(const char* name, size_t name_len, iovec& value) const;

  /**
    Get all field name-value pairs.
    @param[out] fields growable vector of name-value pairs as pointers
      into the HTTPMessage's underlying buffer
  */
  void get_fields(vector<std::pair<iovec, iovec> >& fields) const;

public:
  /**
    Get the buffer underlying the HTTP message's header
      (request or response line, fields).
  */
  Buffer& get_header() const {
    return header;
  }

public:
  /**
    Get the HTTP version of this message as a single byte
    (0 or 1, for HTTP/1.0 and HTTP/1.0, respectively).
    @return the HTTP version
  */
  uint8_t get_http_version() const {
    return http_version;
  }

public:
  /**
    Check if a field is present.
    @param name the field name
    @return true if the field is present, false if not
  */
  bool has_field(const char* name) const {
    return has_field(name, strlen(name));
  }

  /**
    Check if a field is present.
    @param name the field name
    @param name_len length in bytes of name
    @return true if the field is present, false if not
  */
  bool has_field(const char* name, size_t name_len) const {
    iovec value;
    return get_field(name, name_len, value);
  }

public:
  /**
    Get a field value, copying into a temporary string.
    @param name the field name
    @return the field value or an empty string if the field is not present
  */
  string operator[](const char* name) {
    return get_field(name);
  }

public:
  /**
    Set the body of an HTTP message.
    Steals the reference to body and decrements the existing body, if present.
  */
  void set_body(YO_NEW_REF Object* body);

public:
  /**
    Set a string field (set_field(..., const char* value) form).
    @param name the field name
    @param value the field value
    @return *this
  */
  HTTPMessageType& set_field(const char* name, const char* value) {
    return set_field(name, strlen(name), value);
  }

  /**
    Set a string field (set_field(..., const char* value) form).
    @param name the field name
    @param value the field value
    @return *this
  */
  HTTPMessageType& set_field(const string& name, const char* value) {
    return set_field(name.data(), name.size(), value);
  }

  /**
    Set a string field (set_field(..., const char* value) form).
    @param name the field name
    @param name_len the length of name in bytes
    @param value the field value
    @return *this
  */
  HTTPMessageType&
  set_field(
    const char* name,
    size_t name_len,
    const char* value
  ) {
    return set_field(name, name_len, value, strlen(value));
  }

  /**
    Set a string field (set_field(..., const iovec& value) form).
    @param name the field name
    @param value the field value
    @return *this
  */
  HTTPMessageType& set_field(const char* name, const iovec& value) {
    return set_field(name, strlen(name), value);
  }

  /**
    Set a string field (set_field(..., const iovec& value) form).
    @param name the field name
    @param value the field value
    @return *this
  */
  HTTPMessageType& set_field(const string& name, const iovec& value) {
    return set_field(name.data(), name.size(), value);
  }

  /**
    Set a string field (set_field(..., const iovec& value) form).
    @param name the field name
    @param name_len the length of name in bytes
    @param value the field value
    @return *this
  */
  HTTPMessageType&
  set_field(
    const char* name,
    size_t name_len,
    const iovec& value
  ) {
    return set_field(
             name,
             name_len,
             static_cast<char*>(value.iov_base),
             value.iov_len
           );
  }

  /**
    Set a string field (set_field(..., const string& value) form).
    @param name the field name
    @param value the field value
    @return *this
  */
  HTTPMessageType&
  set_field(
    const char* name,
    const string& value
  ) {
    return set_field(name, strlen(name), value);
  }

  /**
    Set a string field (set_field(..., const string& value) form).
    @param name the field name
    @param value the field value
    @return *this
  */
  HTTPMessageType& set_field(const string& name, const string& value) {
    return set_field(name.data(), name.size(), value);
  }

  /**
    Set a string field (set_field(..., const string& value) form).
    @param name the field name
    @param name_len the length of name in bytes
    @param value the field value
    @return *this
  */
  HTTPMessageType&
  set_field(
    const char* name,
    size_t name_len,
    const string& value
  ) {
    return set_field(name, name_len, value.data(), value.size());
  }

  /**
    Set a numeric field.
    @param name the field name
    @param value the field value
    @return *this
  */
  HTTPMessageType& set_field(const char* name, size_t value) {
    return set_field(name, strlen(name), value);
  }

  /**
    Set a numeric field.
    @param name the field name
    @param value the field value
    @return *this
  */
  HTTPMessageType& set_field(const string& name, size_t value) {
    return set_field(name.data(), name.size(), value);
  }

  /**
    Set a numeric field.
    @param name the field name
    @param name_len the length of name in bytes
    @param value the field value
    @return *this
  */
  HTTPMessageType& set_field(const char* name, size_t name_len, size_t value);

  /**
    Set a date-time field.
    @param name the field name
    @param value the field value
    @return *this
  */
  HTTPMessageType& set_field(const char* name, const DateTime& value) {
    return set_field(name, strlen(name), value);
  }

  /**
    Set a date-time field.
    @param name the field name
    @param value the field value
    @return *this
  */
  HTTPMessageType& set_field(const string& name, const DateTime& value) {
    return set_field(name.data(), name.size(), value);
  }

  /**
    Set a date-time field.
    @param name the field name
    @param name_len the length of name in bytes
    @param value the field value
    @return *this
  */
  HTTPMessageType&
  set_field(
    const char* name,
    size_t name_len,
    const DateTime& value
  );


  /**
    Set a string field (set_field(..., const void* value, size_t value_len)
      form).
    @param name the field name
    @param value the field value
    @param value_len the length of value in bytes
    @return *this
  */
  HTTPMessageType&
  set_field(
    const char* name,
    const void* value,
    size_t value_len
  ) {
    return set_field(name, strlen(name), value, value_len);
  }

  /**
    Set a string field (set_field(..., const void* value, size_t value_len)
      form).
    @param name the field name
    @param value the field value
    @param value_len the length of value in bytes
    @return *this
  */
  HTTPMessageType& set_field(
    const string& name,
    const void* value,
    size_t value_len
  ) {
    return set_field(name.data(), name.size(), value, value_len);
  }

  /**
    Set a string field (set_field(..., const void* value, size_t value_len)
      form).
    @param name the field name
    @param name_len the length of name in bytes
    @param value the field value
    @param value_len the length of value in bytes
    @return *this
  */
  HTTPMessageType&
  set_field(
    const char* name,
    size_t name_len,
    const void* value,
    size_t value_len
  );

protected:
  HTTPMessage(YO_NEW_REF Object* body, uint8_t http_version);

  HTTPMessage(
    YO_NEW_REF Object* body,
    uint16_t fields_offset,
    Buffer& header,
    uint8_t http_version
  );

  virtual ~HTTPMessage();

protected:
  void set_fields_offset(uint16_t fields_offset) {
    this->fields_offset = fields_offset;
  }

private:
  Object* body;
  uint16_t fields_offset;
  Buffer& header;
  uint8_t http_version;
};
}
}

#endif
