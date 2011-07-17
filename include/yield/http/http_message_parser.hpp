// yield/http/http_message_parser.hpp

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

#ifndef _YIELD_HTTP_HTTP_MESSAGE_PARSER_HPP_
#define _YIELD_HTTP_HTTP_MESSAGE_PARSER_HPP_

#include "yield/http/http_message.hpp"
#include "yield/http/http_message_body_chunk.hpp"

namespace yield {
class DateTime;

namespace http {
class HTTPBodyChunk;

class HTTPMessageParser {
public:
  static bool
  parse_content_length_field(
    const char* ps,
    const char* pe,
    size_t& content_length
  );

  static DateTime parse_date(const iovec& date);
  static DateTime parse_date(const char* ps, const char* pe);

  static bool
  parse_field(
    const char* ps,
    const char* pe,
    const iovec& field_name,
    iovec& field_value
  );

  static void
  parse_fields(
    const char* ps,
    const char* pe,
    vector< std::pair<iovec, iovec> >& fields
  );

protected:
  HTTPMessageParser(Buffer& buffer);
  HTTPMessageParser(const string& buffer); // For testing
  ~HTTPMessageParser();

protected:
  virtual YO_NEW_REF HTTPMessageBodyChunk&
  create_http_message_body_chunk(
    YO_NEW_REF Buffer* data
  ) {
    return *new HTTPMessageBodyChunk(data);
  }

protected:
  bool parse_body(size_t content_length, YO_NEW_REF Object*& body);
  Object* parse_body_chunk();
  bool parse_fields(uint16_t& fields_offset, size_t& content_length);

protected:
  Buffer& buffer;
  const char* eof;
  char *p, *ps;

private:
  static bool
  parse_content_length_field(
    const iovec& field_name,
    const iovec& field_value,
    size_t& content_length
  );
};
}
}

#endif
