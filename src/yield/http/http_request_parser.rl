// yield/http/http_request_parser.rl

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
#include "yield/http/http_request_parser.hpp"
#include "yield/http/http_response.hpp"

#include <stdlib.h> // For atof and atoi

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4702)
#endif

namespace yield {
namespace http {
using yield::uri::URI;

Object& HTTPRequestParser::parse() {
  if (p < eof) {
    ps = p;

    uint8_t http_version;
    HTTPRequest::Method method;
    iovec uri_fragment = { 0 };
    iovec uri_host = { 0 };
    iovec uri_path = { 0 };
    uint16_t uri_port = 0;
    iovec uri_query = { 0 };
    iovec uri_scheme = { 0 };
    iovec uri_userinfo = { 0 };

    if 
    (
      parse_request_line(
        http_version,
        method,
        uri_fragment,
        uri_host,
        uri_path,
        uri_port,
        uri_query,
        uri_scheme,
        uri_userinfo
      )
    ) {
      URI uri(
        buffer,
        uri_fragment,
        uri_host,
        uri_path,
        uri_port,
        uri_query,
        uri_scheme,
        uri_userinfo
      );

      uint16_t fields_offset;
      size_t content_length;
      if (parse_fields(fields_offset, content_length)) {
        Object* body;
        if (parse_body(content_length, body)) {
          return *new HTTPRequest(
                   body,
                   connection_id,
                   fields_offset,
                   buffer,
                   http_version,
                   method,
                   uri
                );
        } else {
          Buffer& next_buffer
            = Buffer::copy(
                Buffer::getpagesize(),
                p - ps + content_length,
                ps,
                eof - ps
              );
          ps = p;
          return next_buffer;
        }
      }
    } else { // cs == request_line_parser_error
      Object* object = parse_body_chunk();
      if (object != NULL)
        return *object;
    }

    if (p == eof) { // EOF parsing
      Buffer& next_buffer
        = Buffer::copy(
            Buffer::getpagesize(),
            eof - ps + Buffer::getpagesize(),
            ps,
            eof - ps
          );
      p = ps;
      return next_buffer;
    } else { // Error parsing
      HTTPResponse* http_response
        = new HTTPResponse(400, NULL, connection_id, http_version);
      http_response->set_field("Content-Length", 14, "0", 1);
      return *http_response;
    }
  } else // p == eof
    return *new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
}

bool HTTPRequestParser::parse_request_line(
  OUT uint8_t& http_version,
  OUT HTTPRequest::Method& method,
  OUT iovec& fragment,
  OUT iovec& host,
  OUT iovec& path,
  OUT uint16_t& port,
  OUT iovec& query,
  OUT iovec& scheme,
  OUT iovec& userinfo
) {
    int cs;

    %%{
      machine request_line_parser;
      alphtype unsigned char;

      include rfc2616 "rfc2616.rl";
      include rfc3986 "../../../include/yield/uri/rfc3986.rl";

      # RFC 2616 5.1.1
      method
        = ("CONNECT" % { method = HTTPRequest::Method::CONNECT; }) |
          ("COPY" % { method = HTTPRequest::Method::COPY; }) |
          ("DELETE" % { method = HTTPRequest::Method::DELETE; }) |
          ("GET" % { method = HTTPRequest::Method::GET; }) |
          ("HEAD" % { method = HTTPRequest::Method::HEAD; }) |
          ("LOCK" % { method = HTTPRequest::Method::LOCK; }) |
          ("MKCOL" % { method = HTTPRequest::Method::MKCOL; }) |
          ("MOVE" % { method = HTTPRequest::Method::MOVE; }) |
          ("OPTIONS" % { method = HTTPRequest::Method::OPTIONS; }) |
          ("PATCH" % { method = HTTPRequest::Method::PATCH; }) |
          ("POST" % { method = HTTPRequest::Method::POST; }) |
          ("PROPFIND" % { method = HTTPRequest::Method::PROPFIND; }) |
          ("PROPPATCH" % { method = HTTPRequest::Method::PROPPATCH; }) |
          ("PUT" % { method = HTTPRequest::Method::PUT; }) |
          ("TRACE" % { method = HTTPRequest::Method::TRACE; }) |
          ("UNLOCK" % { method = HTTPRequest::Method::UNLOCK; });

      # RFC 2616 5.1.2
      # Per the spec, Request-URI = "*" | absoluteURI | abs_path | authority
      # absoluteURI is fully-qualified (http://host...), for use with proxies.
      # authority is (userinfo "@")? host (":" port)?, used with CONNECT.
      # abs_path is the common form, but it does NOT include a ["?" query],
      #   only path segments.
      # -> an oversight in the spec, corrected here.
      request_uri = (
          (
            '*'
            >{ path.iov_base = p; }
            %{ path.iov_len = p - static_cast<char*>(path.iov_base); }
          ) |
          absolute_uri |
          (path_absolute ("?" query)?) |
          authority
      );

      # RFC 2616 5.1
      request_line = method ' '+ request_uri ' '+ http_version crlf;

      main := request_line
              @{ fbreak; };

      write data;
      write init;
      write exec noend;
    }%%

    return cs != request_line_parser_error;
}
}
}

#ifdef _WIN32
#pragma warning(pop)
#endif
//
