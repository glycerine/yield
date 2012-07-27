// yield/http/server/http_request.hpp

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

/**
  A <code>yield::http::HTTPRequest</code> that's bound to a server
    <code>HTTPConnection</code>.
  These <code>HTTPRequest</code>s are usually created by
    <code>HTTPRequestQueue</code>.
*/
class HTTPRequest : public ::yield::http::HTTPRequest {
public:
  const static uint32_t TYPE_ID = 2792000307UL;

public:
  /**
    Construct an HTTPRequest that originates from the given server connection.
    @param connection the server connection
    @param method the HTTP request method e.g., HTTPRequest::Method::GET
    @param uri the HTTP request URI e.g., /
    @param body an optional body, usually a Buffer
    @param http_version the HTTP version as a single byte (0 or 1 for HTTP/1.0
      and HTTP/1.1, respectively)
  */
  HTTPRequest(
    HTTPConnection& connection,
    Method method,
    const yield::uri::URI& uri,
    YO_NEW_REF Object* body = NULL,
    uint8_t http_version = HTTP_VERSION_DEFAULT
  );

  virtual ~HTTPRequest();

public:
  /**
    Get the server connection from which this HTTP request originated.
    @return the server connection from which this HTTP request originated
  */
  const HTTPConnection& get_connection() const {
    return connection;
  }

  /**
    Get the date-time this HTTPRequest was constructed.
    Used in logging.
    @return the date-time this HTTPRequest was constructed
  */
  const DateTime& get_creation_date_time() const {
    return creation_date_time;
  }

public:
  /**
    Respond to the HTTP request with a chunked body.
    respond(HTTPResponse&) or respond(status_code) must be called first.
    @param chunk response body chunk
  */
  void respond(YO_NEW_REF ::yield::http::HTTPMessageBodyChunk& chunk);

  /**
    Respond to the HTTP request.
    Steals the reference to http_response, which should not be modified
      after this method is called.
    @param http_response HTTP response
  */
  void respond(YO_NEW_REF ::yield::http::HTTPResponse& http_response);

  /**
    Respond to the HTTP request.
    @param status_code response status code e.g, 200
  */
  void respond(uint16_t status_code);

  /**
    Respond to the HTTP request.
    This method should only be called once.
    @param status_code response status code e.g., 200
    @param body response body
  */
  void respond(uint16_t status_code, YO_NEW_REF Buffer* body);

  /**
    Respond to the HTTP request.
    This method should only be called once.
    @param status_code response status code e.g., 200
    @param body response body
  */
  void respond(uint16_t status_code, YO_NEW_REF Buffer& body);

  /**
    Respond to the HTTP request.
    This method should only be called once.
    @param status_code response status code e.g., 200
    @param body response body
  */
  void respond(uint16_t status_code, const char* body);

  /**
    Respond to the HTTP request.
    This method should only be called once.
    @param status_code response status code e.g., 200
    @param body response body
  */
  void respond(uint16_t status_code, const Exception& body);

  /**
    Respond to the HTTP request.
    This method should only be called once.
    @param status_code response status code e.g., 200
    @param body response body
  */
  void respond(uint16_t status_code, YO_NEW_REF Object* body);

  /**
    Respond to the HTTP request.
    This method should only be called once.
    @param status_code response status code e.g., 200
    @param body response body
  */
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
