// yield/http/http_message_parser.rl

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
#include "yield/http/http_message_body_chunk.hpp"
#include "yield/http/http_message_parser.hpp"
#include "yield/http/http_request.hpp"

#include <stdlib.h> // For strtol

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4702)
#endif

namespace yield {
namespace http {
HTTPMessageParser::HTTPMessageParser(Buffer& buffer, uint32_t connection_id)
  : buffer(buffer.inc_ref()),
    connection_id(connection_id) {
  debug_assert_false(buffer.empty());

  ps = p = buffer;
  eof = ps + buffer.size();
}

HTTPMessageParser::HTTPMessageParser(const string& buffer)
  : buffer(Buffer::copy(buffer)) {
  debug_assert_false(buffer.empty());

  connection_id = 0;
  ps = p = this->buffer;
  eof = ps + this->buffer.size();
}

HTTPMessageParser::~HTTPMessageParser() {
  Buffer::dec_ref(buffer);
}

bool
HTTPMessageParser::parse_body(
  size_t content_length,
  OUT YO_NEW_REF Object*& body
) {
  if (
    content_length == 0
    ||
    content_length == HTTPRequest::CONTENT_LENGTH_CHUNKED
 ) {
    body = NULL;
    return true;
  } else if (static_cast<size_t>(eof - p) >= content_length) {
    body = &Buffer::copy(p, content_length);
    p += content_length;
    return true;
  } else
    return false;
}

Object* HTTPMessageParser::parse_body_chunk() {
  const char* chunk_data_p = NULL;
  size_t chunk_size = 0;
  const char* chunk_size_p = NULL;
  int cs;
  iovec field_name = { 0 }, field_value = { 0 };
  size_t seen_chunk_size = 0;

  ps = p;

  %%{
    machine chunk_parser;
    alphtype unsigned char;

    include rfc2616 "rfc2616.rl";

    main := chunk
            @{ fbreak; }
            $err{ return NULL; };

    write data;
    write init;
    write exec noend;
  }%%

  if (cs != chunk_parser_error) {
    if (chunk_size > 0) {
      // Cut off the chunk size + extension + CRLF before
      // the chunk data and the CRLF after
      Buffer& chunk_data = Buffer::copy(chunk_data_p, p - chunk_data_p - 2);
      return new HTTPMessageBodyChunk(&chunk_data, connection_id);
    } else // Last chunk
      return new HTTPMessageBodyChunk(NULL, connection_id);
  } else if (p == eof && chunk_size != 0)
    return new Buffer(chunk_size + 2); // Assumes no trailers.
  else
    return NULL;
}

DateTime HTTPMessageParser::parse_date(const iovec& date) {
return parse_date(
          static_cast<const char*>(date.iov_base),
          static_cast<const char*>(date.iov_base) + date.iov_len
        );
}

DateTime HTTPMessageParser::parse_date(const char* ps, const char* pe) {
  int cs;
  const char* eof = pe;
  char* p = const_cast<char*>(ps);

  int hour = 0, minute = 0, second = 0;
  int day = 0, month = 0, year = 0;

  %%{
    machine date_parser;
    alphtype unsigned char;

    include rfc2616 "rfc2616.rl";

    main := date
            $err{ return DateTime::INVALID_DATE_TIME; };

    write data;
    write init;
    write exec;
  }%%

  if (cs != date_parser_error) {
    if (year < 100) year += 2000;
    year -= 1900;
    return DateTime(second, minute, hour, day, month - 1, year, false);
  } else
    return DateTime::INVALID_DATE_TIME;
}

bool
HTTPMessageParser::parse_field(
  const char* ps,
  const char* pe,
  const iovec& in_field_name,
  OUT iovec& out_field_value
) {
  int cs;
  char* p = const_cast<char*>(ps);

  iovec field_name = {0}, field_value = {0};

  // Don't look for the trailing CRLF before the body,
  // since it may not be present yet.
  %%{
    machine field_parser;
    alphtype unsigned char;

    include rfc2616 "rfc2616.rl";

    main := (
      field @ {
        if (
          field_name.iov_len == in_field_name.iov_len
          &&
          memcmp(
            field_name.iov_base,
            in_field_name.iov_base,
            in_field_name.iov_len
          ) == 0
        ) {
          out_field_value = field_value;
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

void
HTTPMessageParser::parse_fields(
  const char* ps,
  const char* pe,
  OUT vector< std::pair<iovec, iovec> >& fields
) {
  int cs;
  char* p = const_cast<char*>(ps);

  iovec field_name = {0}, field_value = {0};

  // Don't look for the trailing CRLF before the body,
  // since it may not be present yet.
  %%{
    machine static_fields_parser;
    alphtype unsigned char;

    include rfc2616 "rfc2616.rl";

    main := (
      field @ { fields.push_back(std::make_pair(field_name, field_value)); }
    )*;

    write data;
    write init;
    write exec;
  }%%
}

bool
HTTPMessageParser::parse_fields(
  OUT uint16_t& fields_offset,
  OUT size_t& content_length
) {
  fields_offset = static_cast<uint16_t>(p - ps);

  content_length = 0;

  int cs;
  iovec field_name = { 0 }, field_value = { 0 };

  %%{
    machine fields_parser;
    alphtype unsigned char;

    include rfc2616 "rfc2616.rl";

    main := (
      field % {
        if (
          field_name.iov_len == 14
          &&
          (
            memcmp(field_name.iov_base, "Content-Length", 14) == 0
            ||
            memcmp(field_name.iov_base, "Content-length", 14) == 0
          )
        ) {
          char* nptr = static_cast<char*>(field_value.iov_base);
          char* endptr = nptr + field_value.iov_len;
          content_length = static_cast<size_t>(strtol(nptr, &endptr, 10));
        }
        else if (
          field_name.iov_len == 17
          && (
            memcmp(field_name.iov_base, "Transfer-Encoding", 17) == 0
            ||
            memcmp(field_name.iov_base, "Transfer-encoding", 17) == 0
          )
          &&
          memcmp(field_value.iov_base, "chunked", 7) == 0
        )
          content_length = HTTPRequest::CONTENT_LENGTH_CHUNKED;
      }
    )* crlf
    @{ fbreak; }
    $err{ return false; };

    write data;
    write init;
    write exec noend;
  }%%

  return cs != fields_parser_error;
}
}
}

#ifdef _WIN32
#pragma warning(pop)
#endif
//
