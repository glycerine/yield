// yield/http/http_request.cpp

// Copyright (c) 2010 Minor Gordon
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


#include <sstream> // for std::ostringstream
#include "yield/exception.hpp"
#include "yield/string_buffer.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_response.hpp"


namespace yield {
namespace http {
HTTPRequest::HTTPRequest
(
  void* body,
  Buffer& buffer,
  size_t content_length,
  uint16_t fields_offset,
  float http_version,
  Method method,
  const yield::net::URI& uri
)
  : HTTPMessage<HTTPRequest>
  (
    body,
    buffer,
    content_length,
    fields_offset,
    http_version
  ),
  creation_date_time(DateTime::now()),
  method(method),
  uri(uri)
{ }

HTTPRequest::HTTPRequest
(
  Method method,
  const yield::net::URI& uri,
  YO_NEW_REF Buffer* body
)
  : HTTPMessage<HTTPRequest>(body, 1.1f),
    creation_date_time(DateTime::now()),
    method(method),
    uri(uri) {
  switch (method) {
  case METHOD_CONNECT:
    get_buffer().put("CONNECT ", 8);
    break;
  case METHOD_COPY:
    get_buffer().put("COPY ", 5);
    break;
  case METHOD_DELETE:
    get_buffer().put("DELETE ", 7);
    break;
  case METHOD_GET:
    get_buffer().put("GET ", 4);
    break;
  case METHOD_HEAD:
    get_buffer().put("HEAD ", 5);
    break;
  case METHOD_LOCK:
    get_buffer().put("LOCK ", 5);
    break;
  case METHOD_MKCOL:
    get_buffer().put("MKCOL ", 6);
    break;
  case METHOD_MOVE:
    get_buffer().put("MOVE ", 5);
    break;
  case METHOD_OPTIONS:
    get_buffer().put("OPTIONS ", 8);
    break;
  case METHOD_PATCH:
    get_buffer().put("PATCH ", 6);
    break;
  case METHOD_POST:
    get_buffer().put("POST ", 5);
    break;
  case METHOD_PROPFIND:
    get_buffer().put("PROPFIND ", 9);
    break;
  case METHOD_PROPPATCH:
    get_buffer().put("PROPPATCH ", 10);
    break;
  case METHOD_PUT:
    get_buffer().put("PUT ", 4);
    break;
  case METHOD_TRACE:
    get_buffer().put("TRACE ", 6);
    break;
  case METHOD_UNLOCK:
    get_buffer().put("UNLOCK ", 7);
    break;
  }

  iovec uri_path;
  uri.get_path(uri_path);
  get_buffer().put(uri_path);

  get_buffer().put(" HTTP/1.1\r\n", 11);

  mark_fields_offset();

  iovec uri_host;
  uri.get_host(uri_host);
  if (uri.get_port() == 80)
    set_field("Host", uri_host);
  else {
    const char* uri_port_p
    = static_cast<char*>(uri_host.iov_base) + uri_host.iov_len;
    if
    (
      uri_port_p >= static_cast<char*>(uri_path.iov_base) - 6
      &&
      uri_port_p < uri_path.iov_base
      &&
      *uri_port_p == ':'
    ) {
      const char* uri_port_ps = uri_port_p;
      uri_port_p++;
      while (uri_port_p < uri_path.iov_base && isdigit(*uri_port_p))
        uri_port_p++;
      uri_host.iov_len += uri_port_p - uri_port_ps;

      set_field("Host", uri_host);
    } else {
      std::ostringstream host;
      host.write
      (
        static_cast<char*>(uri_host.iov_base),
        uri_host.iov_len
      );
      host << ':';
      host << uri.get_port();

      set_field("Host", host.str());
    }
  }
}

const DateTime& HTTPRequest::get_creation_date_time() const {
  return creation_date_time;
}

void HTTPRequest::respond(HTTPResponse& http_response) {
  Request::respond(http_response);
}

void HTTPRequest::respond(uint16_t status_code) {
  respond(*new HTTPResponse(NULL, get_http_version(), status_code));
}

void HTTPRequest::respond(uint16_t status_code, const char* body) {
  respond(status_code, *new StringBuffer(body));
}

void HTTPRequest::respond(uint16_t status_code, Buffer& body) {
  respond(*new HTTPResponse(&body, get_http_version(), status_code));
}

void HTTPRequest::respond(Exception& exception) {
  Request::respond(exception);
}
}
}
