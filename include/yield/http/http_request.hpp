// yield/http/http_request.hpp

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

#ifndef _YIELD_HTTP_HTTP_REQUEST_HPP_
#define _YIELD_HTTP_HTTP_REQUEST_HPP_

#include "yield/date_time.hpp"
#include "yield/request.hpp"
#include "yield/http/http_message.hpp"
#include "yield/uri/uri.hpp"

namespace yield {
class Exception;


namespace http {
class HTTPResponse;

class HTTPRequest : public Request, public HTTPMessage<HTTPRequest> {
public:
  enum Method {
    METHOD_CONNECT,
    METHOD_COPY,
    METHOD_DELETE,
    METHOD_GET,
    METHOD_HEAD,
    METHOD_MKCOL,
    METHOD_LOCK,
    METHOD_MOVE,
    METHOD_OPTIONS,
    METHOD_PATCH,
    METHOD_POST,
    METHOD_PROPFIND,
    METHOD_PROPPATCH,
    METHOD_PUT,
    METHOD_TRACE,
    METHOD_UNLOCK
  };

  const static uint32_t TYPE_ID = 707981577;

public:
  HTTPRequest(
    Method method,
    const yield::uri::URI& uri,
    YO_NEW_REF Buffer* body = NULL,
    uint32_t connection_id = 0,
    float http_version = 1.1f
  );

public:
  const DateTime& get_creation_date_time() const {
    return creation_date_time;
  }

  Method get_method() const {
    return method;
  }

  const yield::uri::URI& get_uri() const {
    return uri;
  }

public:
  void respond(YO_NEW_REF HTTPResponse& http_response);
  void respond(uint16_t status_code);
  void respond(uint16_t status_code, const char* body);
  void respond(uint16_t status_code, YO_NEW_REF Buffer* body);
  void respond(uint16_t status_code, YO_NEW_REF Buffer& body);
  void respond(YO_NEW_REF Exception& exception);

public:
  // yield::Object
  HTTPRequest& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::Event
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::http::HTTPRequest";
  }

private:
  friend class HTTPRequestParser;

  HTTPRequest(
    void* body,
    Buffer& buffer,
    uint32_t connection_id,
    size_t content_length,
    uint16_t fields_offset,
    float http_version,
    Method method,
    const yield::uri::URI& uri
  );

private:
  DateTime creation_date_time;
  Method method;
  yield::uri::URI uri;
};
}
}

#endif
