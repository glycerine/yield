// yield/http/server/http_request.hpp

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

#ifndef _YIELD_HTTP_SERVER_HTTP_REQUEST_HPP_
#define _YIELD_HTTP_SERVER_HTTP_REQUEST_HPP_

#include "yield/http/http_request.hpp"

namespace yield {
namespace http {
class HTTPMessageBodyChunk;
class HTTPResponse;

namespace server {
class HTTPConnection;
class HTTPRequestParser;

class HTTPRequest : public ::yield::http::HTTPRequest {
public:
  const static uint32_t TYPE_ID = 2792000307UL;

public:
  HTTPRequest(
    HTTPConnection& connection,
    Method method,
    const yield::uri::URI& uri,
    YO_NEW_REF Object* body = NULL,
    uint8_t http_version = HTTP_VERSION_DEFAULT
  );

  virtual ~HTTPRequest();

public:
  const HTTPConnection& get_connection() const {
    return connection;
  }

  /**
    Get the date-time this HTTPRequest object was constructed.
    Used in logging.
  */
  const DateTime& get_creation_date_time() const {
    return creation_date_time;
  }

public:
  void respond(YO_NEW_REF ::yield::http::HTTPMessageBodyChunk& chunk);
  void respond(YO_NEW_REF ::yield::http::HTTPResponse& http_response);
  void respond(uint16_t status_code);
  void respond(uint16_t status_code, YO_NEW_REF Buffer* body);
  void respond(uint16_t status_code, YO_NEW_REF Buffer& body);
  void respond(uint16_t status_code, const char* body);
  void respond(uint16_t status_code, const Exception& body);
  void respond(uint16_t status_code, YO_NEW_REF Object* body);
  void respond(uint16_t status_code, YO_NEW_REF Object& body);

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::http::server::HTTPRequest";
  }

  HTTPRequest& inc_ref() {
    return Object::inc_ref(*this);
  }

protected:
  friend class HTTPRequestParser;

  HTTPRequest(
    YO_NEW_REF Object* body,
    HTTPConnection& connection,
    uint16_t fields_offset,
    Buffer& header,
    uint8_t http_version,
    Method method,
    const yield::uri::URI& uri
  );

private:
  HTTPConnection& connection;
  DateTime creation_date_time;
};
}
}
}

#endif
