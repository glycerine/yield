// yield/http/http_message.cpp

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
#include "yield/buffer.hpp"
#include "yield/date_time.hpp"
#include "yield/http/http_message.hpp"
#include "yield/http/http_message_parser.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_response.hpp"

#ifdef _WIN32
#include <Windows.h> // For SYSTEMTIME
#pragma warning(push)
#pragma warning(disable:4702)
#else
#include <stdio.h> // For snprintf
#include <stdlib.h> // For atoi
#endif

namespace yield {
namespace http {
template <class HTTPMessageType>
HTTPMessage<HTTPMessageType>::
HTTPMessage(
  YO_NEW_REF Object* body,
  uint32_t connection_id,
  float http_version
) : body(body),
    connection_id(connection_id),
    header(*new Buffer(Buffer::getpagesize(), Buffer::getpagesize())),
    http_version(http_version) {
  fields_offset = 0;
}

template <class HTTPMessageType>
HTTPMessage<HTTPMessageType>::
HTTPMessage(
  YO_NEW_REF Object* body,
  uint32_t connection_id,
  uint16_t fields_offset,
  Buffer& header,
  float http_version
) : body(body),
    connection_id(connection_id),
    fields_offset(fields_offset),
    header(header.inc_ref()),
    http_version(http_version) {
}

template <class HTTPMessageType>
HTTPMessage<HTTPMessageType>::~HTTPMessage() {
  Object::dec_ref(body);
  Buffer::dec_ref(header);
}

template <class HTTPMessageType>
void HTTPMessage<HTTPMessageType>::finalize() {
  if (body != NULL && body->get_type_id() == Buffer::TYPE_ID)
    set_field("Content-Length", 14, static_cast<Buffer*>(body)->size());
  header.put("\r\n", 2);
}

template <class HTTPMessageType>
DateTime 
HTTPMessage<HTTPMessageType>::
get_date_field(
  const char* name
) const {
  iovec value;
  if (get_field(name, value))
    return HTTPMessageParser::parse_date(value);
  else
    return DateTime::INVALID_DATE_TIME;
}

template <class HTTPMessageType>
bool
HTTPMessage<HTTPMessageType>::get_field(
  const char* name,
  size_t name_len,
  OUT iovec& value
) const {
  iovec name_iov;
  name_iov.iov_base = const_cast<char*>(name);
  name_iov.iov_len = name_len;
  return HTTPMessageParser::parse_field(
           static_cast<const char*>(header) + fields_offset,
           static_cast<const char*>(header) + header.size(),
           name_iov,
           value
         );
}

template <class HTTPMessageType>
void
HTTPMessage<HTTPMessageType>::get_fields(
  OUT vector< std::pair<iovec, iovec> >& fields
) const {
  return HTTPMessageParser::parse_fields(
           static_cast<const char*>(header) + fields_offset,
           static_cast<const char*>(header) + header.size(),
           fields
         );
}

template <class HTTPMessageType>
void HTTPMessage<HTTPMessageType>::set_body(YO_NEW_REF Object* body) {
  Object::dec_ref(this->body);
  this->body = body;
}

template <class HTTPMessageType>
HTTPMessageType&
HTTPMessage<HTTPMessageType>::
set_field(
  const char * name,
  size_t name_len,
  const DateTime & value
) {
  static const char* months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };

  static const char* week_days[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };

  char date[30];
  int date_len;

#ifdef _WIN32
  SYSTEMTIME utc_system_time = value.as_utc_SYSTEMTIME();

  date_len
  = _snprintf_s(
      date,
      30,
      _TRUNCATE,
      "%s, %02d %s %04d %02d:%02d:%02d GMT",
      week_days[utc_system_time.wDayOfWeek],
      utc_system_time.wDay,
      months[utc_system_time.wMonth - 1],
      utc_system_time.wYear,
      utc_system_time.wHour,
      utc_system_time.wMinute,
      utc_system_time.wSecond
    );
#else
  tm utc_tm = value.as_utc_tm();

  date_len
  = snprintf(
      date,
      30,
      "%s, %02d %s %04d %02d:%02d:%02d GMT",
      week_days[utc_tm.tm_wday],
      utc_tm.tm_mday,
      months[utc_tm.tm_mon],
      utc_tm.tm_year + 1900,
      utc_tm.tm_hour,
      utc_tm.tm_min,
      utc_tm.tm_sec
    );
#endif

  return set_field(name, name_len, date, date_len);
}

template <class HTTPMessageType>
HTTPMessageType&
HTTPMessage<HTTPMessageType>::
set_field(
  const char * name,
  size_t name_len,
  size_t value
) {
  char value_str[64];
  int value_str_len;

#ifdef _WIN32
  value_str_len = sprintf_s(value_str, 64, "%u", value);
#else
  value_str_len = snprintf(value_str, 64, "%zu", value);
#endif

  return set_field(name, name_len, value_str, value_str_len);
}

template <class HTTPMessageType>
HTTPMessageType&
HTTPMessage<HTTPMessageType>::
set_field(
  const char * name,
  size_t name_len,
  const void * value,
  size_t value_len
) {
  debug_assert_gt(fields_offset, 0);
  debug_assert_gt(name_len, 0);
  debug_assert_gt(value_len, 0);

  header.put(name, name_len);
  header.put(": ", 2);
  header.put(value, value_len);
  header.put("\r\n");

  return static_cast<HTTPMessageType&>(*this);
}

template class HTTPMessage<HTTPRequest>;
template class HTTPMessage<HTTPResponse>;
}
}

#ifdef _WIN32
#pragma warning(pop)
#endif
//
