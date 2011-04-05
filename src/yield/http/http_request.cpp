// yield/http/http_request.cpp

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

#include <sstream> // for std::ostringstream
#include "yield/assert.hpp"
#include "yield/buffer.hpp"
#include "yield/exception.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_response.hpp"

namespace yield {
namespace http {
HTTPRequest::Method HTTPRequest::Method::CONNECT(1, "CONNECT", 7);
HTTPRequest::Method HTTPRequest::Method::COPY(2, "COPY", 4);
HTTPRequest::Method HTTPRequest::Method::DELETE(3, "DELETE", 7);
HTTPRequest::Method HTTPRequest::Method::GET(4, "GET", 3);
HTTPRequest::Method HTTPRequest::Method::HEAD(5, "HEAD", 4);
HTTPRequest::Method HTTPRequest::Method::MKCOL(6, "MKCOL", 5);
HTTPRequest::Method HTTPRequest::Method::LOCK(7, "LOCK", 4);
HTTPRequest::Method HTTPRequest::Method::MOVE(8, "MOVE", 4);
HTTPRequest::Method HTTPRequest::Method::OPTIONS(9, "OPTIONS", 7);
HTTPRequest::Method HTTPRequest::Method::PATCH(10, "PATCH", 5);
HTTPRequest::Method HTTPRequest::Method::POST(11, "POST", 4);
HTTPRequest::Method HTTPRequest::Method::PROPFIND(12, "PROPFIND", 8);
HTTPRequest::Method HTTPRequest::Method::PROPPATCH(13, "PROPPATCH", 9);
HTTPRequest::Method HTTPRequest::Method::PUT(14, "PUT", 3);
HTTPRequest::Method HTTPRequest::Method::TRACE(15, "TRACE", 5);
HTTPRequest::Method HTTPRequest::Method::UNLOCK(16, "UNLOCK", 6);


HTTPRequest::HTTPRequest(
  void* body,
  Buffer& buffer,
  uint32_t connection_id,
  size_t content_length,
  uint16_t fields_offset,
  float http_version,
  Method method,
  const yield::uri::URI& uri
)
  : HTTPMessage<HTTPRequest>(
      body,
      buffer,
      connection_id,
      content_length,
      fields_offset,
      http_version
  ),
  creation_date_time(DateTime::now()),
  method(method),
  uri(uri)
{ }

HTTPRequest::HTTPRequest(
  Method method,
  const yield::uri::URI& uri,
  YO_NEW_REF Buffer* body,
  uint32_t connection_id,
  float http_version
)
  : HTTPMessage<HTTPRequest>(body, connection_id, http_version),
    creation_date_time(DateTime::now()),
    method(method),
    uri(uri) {
  get_buffer().put(method.get_name(), method.get_name_len());

  get_buffer().put(' ');

  iovec uri_path;
  uri.get_path(uri_path);
  get_buffer().put(uri_path);

  if (http_version == 1.1f)
    get_buffer().put(" HTTP/1.1\r\n", 11);
  else if (http_version == 1.0f)
    get_buffer().put(" HTTP/1.0\r\n", 11);
  else
    DebugBreak();

  mark_fields_offset();

  if (uri.has_host()) {
    iovec uri_host;
    uri.get_host(uri_host);
    if (uri.get_port() == 80)
      set_field("Host", 4, uri_host);
    else {
      const char* uri_port_p
      = static_cast<char*>(uri_host.iov_base) + uri_host.iov_len;
      if(
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

        set_field("Host", 4, uri_host);
      } else {
        std::ostringstream host;
        host.write(
          static_cast<char*>(uri_host.iov_base),
          uri_host.iov_len
        );
        host << ':';
        host << uri.get_port();

        set_field("Host", 4, host.str());
      }
      }
  }
}

void HTTPRequest::respond(HTTPResponse& http_response) {
  Request::respond(http_response);
}

void HTTPRequest::respond(uint16_t status_code) {
  respond(status_code, static_cast<Buffer*>(NULL));
}

void HTTPRequest::respond(uint16_t status_code, const char* body) {
  respond(status_code, Buffer::copy(body));
}

void HTTPRequest::respond(uint16_t status_code, YO_NEW_REF Buffer* body) {
  respond(
    *new HTTPResponse(
           status_code,
           body,
           get_connection_id(),
           get_http_version()
         )
  );
}

void HTTPRequest::respond(uint16_t status_code, YO_NEW_REF Buffer& body) {
  respond(status_code, &body);
}

void HTTPRequest::respond(Exception& exception) {
  Request::respond(exception);
}


std::ostream& operator<<(std::ostream& os, const HTTPRequest::Method& method) {
  os << method.get_name();
  return os;
}
}
}
