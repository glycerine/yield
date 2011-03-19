// http_request_parser_test.cpp

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
#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_request_parser.hpp"
#include "yunit.hpp"

TEST_SUITE(HTTPRequestParser);

namespace yield {
namespace http {
TEST(HTTPRequestParser, MalformedHTTPVersionMissing) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET /\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, MalformedHTTPVersionMissingHTTP) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / /1.0\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, MalformedHTTPVersionMissingMinorVersion) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / HTTP/1.\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, MalformedHTTPVersionMissingTrailingCRLF) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / HTTP/1.1Host: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, MalformedMethodMissing) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("/ HTTP/1.0\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, MalformedURIEmbeddedLF) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET /\r HTTP/1.1\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, MalformedURIMissing) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET HTTP/1.1\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, WellFormedRequestLineOnly) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / HTTP/1.1\r\n\r\n").parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_eq(http_request->get_method(), HTTPRequest::METHOD_GET);
  throw_assert_eq(http_request->get_http_version(), 1.1F);
  throw_assert_eq(http_request->get_body(), NULL);
  HTTPRequest::dec_ref(*http_request);
}

}
}
