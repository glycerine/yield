// yield/http/http_message.rl

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

#include "yield/config.hpp"
#include "yield/assert.hpp"
#include "yield/page.hpp"
#include "yield/http/http_message.hpp"
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
  void* body,
  Buffer& buffer,
  size_t content_length,
  uint16_t fields_offset,
  float http_version
)
  : body(body),
    buffer(buffer.inc_ref()),
    content_length(content_length),
    fields_offset(fields_offset),
    http_version(http_version)
{ }

template <class HTTPMessageType>
HTTPMessage<HTTPMessageType>::
HTTPMessage(
  YO_NEW_REF Buffer* body,
  float http_version
)
  : buffer(*new Page),
    http_version(http_version) {
  if (body != NULL) {
    this->body = *body;
    buffer.set_next_buffer(body);
    content_length = body->size();
  } else {
    this->body = NULL;
    content_length = 0;
  }

  fields_offset = 0;
}

template <class HTTPMessageType>
HTTPMessage<HTTPMessageType>::~HTTPMessage() {
  Buffer::dec_ref(buffer);
}

template <class HTTPMessageType>
void HTTPMessage<HTTPMessageType>::finalize() {
  if (!is_finalized()) {
    set_field("Content-Length", 14, content_length);
    buffer.put("\r\n", 2);
  }
}

template <class HTTPMessageType>
DateTime HTTPMessage<HTTPMessageType>::get_date_field(const char* name) const {
  iovec value;
  if (get_field(name, value)) {
    int cs;
    const char* eof = static_cast<char*>(value.iov_base) + value.iov_len;
    char* p = static_cast<char*>(value.iov_base);

    int hour = 0, minute = 0, second = 0;
    int day = 0, month = 0, year = 0;

    %%{
      machine date_parser;
      alphtype unsigned char;

      include date "date.rl";

      main := date
              $err{ return DateTime::INVALID_DATE_TIME; };

      write data;
      write init;
      write exec noend;
    }%%

    if (cs != date_parser_error) {
      if (year < 100) year += 2000;
      year -= 1900;
      return DateTime(second, minute, hour, day, month - 1, year, false);
    } else
      return DateTime::INVALID_DATE_TIME;
  } else
    return DateTime::INVALID_DATE_TIME;
}

template <class HTTPMessageType>
bool
HTTPMessage<HTTPMessageType>::
get_field(
  const char* name,
  size_t name_len,
  OUT iovec& value
) const {
  int cs;
  const char* eof = static_cast<char*>(buffer) + buffer.size();
  char* p = static_cast<char*>(buffer) + fields_offset;
  const char* pe = eof;

  iovec field_name = {0}, field_value = {0};

  // Don't look for the trailing CRLF before the body,
  // since it may not be present yet.
  %%{
    machine field_parser;
    include field "field.rl";

    main := (
      field % {
        if (
          field_name.iov_len == name_len
          &&
          memcmp(field_name.iov_base, name, name_len) == 0
        ) {
          value = field_value;
          return true;
        }
      }
    )*;

    write data;
    write init;
    write exec;
  }%%

  return false;
}

template <class HTTPMessageType>
void
HTTPMessage<HTTPMessageType>::
get_fields(
  OUT vector<pair<iovec, iovec> >& fields
) const {
  int cs;
  const char* eof = static_cast<char*>(buffer) + buffer.size();
  char* p = static_cast<char*>(buffer) + fields_offset;
  const char* pe = eof;

  iovec field_name = {0}, field_value = {0};

  // Don't look for the trailing CRLF before the body,
  // since it may not be present yet.
  %%{
    machine fields_parser;
    include field "field.rl";

    main := (
      field % { fields.push_back(make_pair(field_name, field_value)); }
    )*
    @{ fbreak; }
    $err{ return; };

    write data;
    write init;
    write exec;
  }%%
}

template <class HTTPMessageType>
bool HTTPMessage<HTTPMessageType>::is_finalized() const {
  debug_assert_gt(buffer.size(), 4);

  return strncmp(
           static_cast<char*>(buffer) + buffer.size() - 4,
           "\r\n\r\n",
           4
       )
         == 0;
}

template <class HTTPMessageType>
void HTTPMessage<HTTPMessageType>::mark_fields_offset() {
  fields_offset = static_cast<uint16_t>(buffer.size());
}

template <class HTTPMessageType>
HTTPMessage<HTTPMessageType>::operator Buffer& () {
  finalize();
  return buffer;
}

template <class HTTPMessageType>
HTTPMessageType&
HTTPMessage<HTTPMessageType>::
set_field(
  const char* name,
  size_t name_len,
  const DateTime& value
) {
  static const char* HTTPWeekDays[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };

  static const char* HTTPMonths[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };

  char date[30];
  int date_len;

#ifdef _WIN32
  SYSTEMTIME utc_system_time = value.as_utc_SYSTEMTIME();

  date_len
  = _snprintf_s
    (
      date,
      30,
      _TRUNCATE,
      "%s, %02d %s %04d %02d:%02d:%02d GMT",
      HTTPWeekDays[utc_system_time.wDayOfWeek],
      utc_system_time.wDay,
      HTTPMonths[utc_system_time.wMonth - 1],
      utc_system_time.wYear,
      utc_system_time.wHour,
      utc_system_time.wMinute,
      utc_system_time.wSecond
  );
#else
  tm utc_tm = value.as_utc_tm();

  date_len
  = snprintf
    (
      date,
      30,
      "%s, %02d %s %04d %02d:%02d:%02d GMT",
      HTTPWeekDays[utc_tm.tm_wday],
      utc_tm.tm_mday,
      HTTPMonths[utc_tm.tm_mon],
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
  const char* name,
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
  const char* name,
  size_t name_len,
  const void* value,
  size_t value_len
) {
  debug_assert_gt(fields_offset, 0);
  debug_assert_gt(name_len, 0);
  debug_assert_gt(value_len, 0);

  if (!is_finalized()) {
    buffer.put(name, name_len);
    buffer.put(": ", 2);
    buffer.put(value, value_len);
    buffer.put("\r\n");
  }

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
