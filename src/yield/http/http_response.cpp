// yield/http/http_response.cpp

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

#include "yield/buffer.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_response.hpp"

namespace yield {
namespace http {
HTTPResponse::HTTPResponse(
  void* body,
  Buffer& buffer,
  size_t content_length,
  uint16_t fields_offset,
  float http_version,
  uint16_t status_code
)
  : HTTPMessage<HTTPResponse>(
    body,
    buffer,
    content_length,
    fields_offset,
    http_version
  ),
  status_code(status_code)
{ }

HTTPResponse::HTTPResponse(
  uint16_t status_code,
  YO_NEW_REF Buffer* body,
  float http_version
)
  : HTTPMessage<HTTPResponse>(body, http_version),
    status_code(status_code) {
  const char* status_line;
  size_t status_line_len;

  switch (status_code) {
  case 100:
    status_line = "HTTP/1.1 100 Continue\r\n";
    status_line_len = 23;
    break;
  case 200:
    status_line = "HTTP/1.1 200 OK\r\n";
    status_line_len = 17;
    break;
  case 201:
    status_line = "HTTP/1.1 201 Created\r\n";
    status_line_len = 22;
    break;
  case 202:
    status_line = "HTTP/1.1 202 Accepted\r\n";
    status_line_len = 23;
    break;
  case 203:
    status_line = "HTTP/1.1 203 Non-Authoritative Information\r\n";
    status_line_len = 44;
    break;
  case 204:
    status_line = "HTTP/1.1 204 No Content\r\n";
    status_line_len = 25;
    break;
  case 205:
    status_line = "HTTP/1.1 205 Reset Content\r\n";
    status_line_len = 28;
    break;
  case 206:
    status_line = "HTTP/1.1 206 Partial Content\r\n";
    status_line_len = 30;
    break;
  case 207:
    status_line = "HTTP/1.1 207 Multi-Status\r\n";
    status_line_len = 27;
    break;
  case 300:
    status_line = "HTTP/1.1 300 Multiple Choices\r\n";
    status_line_len = 31;
    break;
  case 301:
    status_line = "HTTP/1.1 301 Moved Permanently\r\n";
    status_line_len = 32;
    break;
  case 302:
    status_line = "HTTP/1.1 302 Found\r\n";
    status_line_len = 20;
    break;
  case 303:
    status_line = "HTTP/1.1 303 See Other\r\n";
    status_line_len = 24;
    break;
  case 304:
    status_line = "HTTP/1.1 304 Not Modified\r\n";
    status_line_len = 27;
    break;
  case 305:
    status_line = "HTTP/1.1 305 Use Proxy\r\n";
    status_line_len = 24;
    break;
  case 307:
    status_line = "HTTP/1.1 307 Temporary Redirect\r\n";
    status_line_len = 33;
    break;
  case 400:
    status_line = "HTTP/1.1 400 Bad Request\r\n";
    status_line_len = 26;
    break;
  case 401:
    status_line = "HTTP/1.1 401 Unauthorized\r\n";
    status_line_len = 27;
    break;
  case 403:
    status_line = "HTTP/1.1 403 Forbidden\r\n";
    status_line_len = 24;
    break;
  case 404:
    status_line = "HTTP/1.1 404 Not Found\r\n";
    status_line_len = 24;
    break;
  case 405:
    status_line = "HTTP/1.1 405 Method Not Allowed\r\n";
    status_line_len = 33;
    break;
  case 406:
    status_line = "HTTP/1.1 406 Not Acceptable\r\n";
    status_line_len = 29;
    break;
  case 407:
    status_line = "HTTP/1.1 407 Proxy Authentication Required\r\n";
    status_line_len = 44;
    break;
  case 408:
    status_line = "HTTP/1.1 408 Request Timeout\r\n";
    status_line_len = 30;
    break;
  case 409:
    status_line = "HTTP/1.1 409 Conflict\r\n";
    status_line_len = 23;
    break;
  case 410:
    status_line = "HTTP/1.1 410 Gone\r\n";
    status_line_len = 19;
    break;
  case 411:
    status_line = "HTTP/1.1 411 Length Required\r\n";
    status_line_len = 30;
    break;
  case 412:
    status_line = "HTTP/1.1 412 Precondition Failed\r\n";
    status_line_len = 34;
    break;
  case 413:
    status_line = "HTTP/1.1 413 Request Entity Too Large\r\n";
    status_line_len = 39;
    break;
  case 414:
    status_line = "HTTP/1.1 414 Request-URI Too Long\r\n";
    status_line_len = 35;
    break;
  case 415:
    status_line = "HTTP/1.1 415 Unsupported Media Type\r\n";
    status_line_len = 37;
    break;
  case 416:
    status_line = "HTTP/1.1 416 Request Range Not Satisfiable\r\n";
    status_line_len = 44;
    break;
  case 417:
    status_line = "HTTP/1.1 417 Expectation Failed\r\n";
    status_line_len = 33;
    break;
  case 422:
    status_line = "HTTP/1.1 422 Unprocessable Entitiy\r\n";
    status_line_len = 36;
    break;
  case 423:
    status_line = "HTTP/1.1 423 Locked\r\n";
    status_line_len = 21;
    break;
  case 424:
    status_line = "HTTP/1.1 424 Failed Dependency\r\n";
    status_line_len = 32;
    break;
  case 500:
    status_line = "HTTP/1.1 500 Internal Server Error\r\n";
    status_line_len = 36;
    break;
  case 501:
    status_line = "HTTP/1.1 501 Not Implemented\r\n";
    status_line_len = 30;
    break;
  case 502:
    status_line = "HTTP/1.1 502 Bad Gateway\r\n";
    status_line_len = 26;
    break;
  case 503:
    status_line = "HTTP/1.1 503 Service Unavailable\r\n";
    status_line_len = 34;
    break;
  case 504:
    status_line = "HTTP/1.1 504 Gateway Timeout\r\n";
    status_line_len = 30;
    break;
  case 505:
    status_line = "HTTP/1.1 505 HTTP Version Not Supported\r\n";
    status_line_len = 41;
    break;
  case 507:
    status_line = "HTTP/1.1 507 Insufficient Storage\r\n";
    status_line_len = 35;
    break;
  default:
    status_line = "HTTP/1.1 500 Internal Server Error\r\n";
    status_line_len = 36;
    break;
  }

  get_buffer().put(status_line, status_line_len);

  mark_fields_offset();

  set_field("Date", DateTime::now());
}
}
}
