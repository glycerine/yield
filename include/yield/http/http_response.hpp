// yield/http/http_response.hpp

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

#ifndef _YIELD_HTTP_HTTP_RESPONSE_HPP_
#define _YIELD_HTTP_HTTP_RESPONSE_HPP_

#include "yield/http/http_message.hpp"

namespace yield {
namespace http {
/**
  An RFC 2616 HTTP response.
  Unlike its counterparts in yield.http.client and yield.http.server, this
    HTTPResponse is not tied to a particular connection.
*/
class HTTPResponse : public HTTPMessage<HTTPResponse> {
public:
  const static uint32_t TYPE_ID = 654743796;

public:
  /**
    Construct an HTTPResponse from its constituent parts.
    @param status_code numeric status code e.g., 200
    @param body optional body, usually a Buffer
    @param http_version HTTP version as a single byte (0 or 1 for HTTP/1.0 or
      HTTP/1.1, respectively.)
  */
  HTTPResponse(
    uint16_t status_code,
    YO_NEW_REF Object* body = NULL,
    uint8_t http_version = HTTP_VERSION_DEFAULT
  );

  /**
    Empty virtual destructor.
  */
  virtual ~HTTPResponse() { }

public:
  /**
    Get the numeric status code e.g., 200.
    @return the numeric status code
  */
  uint16_t get_status_code() const {
    return status_code;
  }

public:
  // yield::Object
  HTTPResponse& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::Event
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::http::HTTPResponse";
  }

protected:
  friend class HTTPResponseParser;

  HTTPResponse(
    YO_NEW_REF Object* body,
    uint16_t fields_offset,
    Buffer& header,
    uint8_t http_version,
    uint16_t status_code
  );

private:
  uint16_t status_code;
};

std::ostream& operator<<(std::ostream&, const HTTPResponse&);
}
}

#endif
