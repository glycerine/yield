// yield/http/http_response_parser.hpp

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

#ifndef _YIELD_HTTP_HTTP_RESPONSE_PARSER_HPP_
#define _YIELD_HTTP_HTTP_RESPONSE_PARSER_HPP_

#include "yield/http/http_message_parser.hpp"
#include "yield/http/http_response.hpp"

namespace yield {
namespace http {
class HTTPResponseParser : public HTTPMessageParser {
public:
  HTTPResponseParser(Buffer& buffer)
    : HTTPMessageParser(buffer)
  { }

  HTTPResponseParser(const string& buffer)
    : HTTPMessageParser(buffer)
  { }

public:
  Object& parse();

protected:
  virtual HTTPResponse&
  create_http_response(
    uint16_t status_code,
    YO_NEW_REF Object* body = NULL,
    uint8_t http_version = HTTPResponse::HTTP_VERSION_DEFAULT
  ) {
    return *new HTTPResponse(status_code, body, http_version);
  }

  virtual HTTPResponse&
  create_http_response(
    YO_NEW_REF Object* body,
    uint16_t fields_offset,
    Buffer& header,
    uint8_t http_version,
    uint16_t status_code
  ) {
    return *new HTTPResponse(
             body,
             fields_offset,
             header,
             http_version,
             status_code
           );
  }

protected:
  bool parse_status_line(uint8_t& http_version, uint16_t& status_code);
};
}
}

#endif
