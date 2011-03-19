// http_response_parser_test.cpp

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
#include "yield/http/http_response.hpp"
#include "yield/http/http_response_parser.hpp"
#include "yunit.hpp"

TEST_SUITE(HTTPResponseParser);

namespace yield {
namespace http {
TEST(HTTPResponseParser, MalformedReasonPhraseMissing) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 200\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_eq(http_response, NULL);
}

TEST(HTTPResponseParser, MalformedStatusCodeAlpha) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 XX OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_eq(http_response, NULL);
}

TEST(HTTPResponseParser, MalformedStatusCodeMissing) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_eq(http_response, NULL);
}

TEST(HTTPResponseParser, MalformedStatusLineMissing) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_eq(http_response, NULL);
}

TEST(HTTPResponseParser, WellFormedNoBody) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_ne(http_response, NULL);
  throw_assert_eq(http_response->get_status_code(), 200);
  throw_assert_eq((*http_response)["Date"], "Fri, 31 Dec 1999 23:59:59 GMT");
  throw_assert_eq(http_response->get_body(), NULL);
  HTTPResponse::dec_ref(*http_response);
}

TEST(HTTPResponseParser, WellFormedNoFields) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 200 OK\r\n\r\n").parse());
  throw_assert_ne(http_response, NULL);
  throw_assert_eq(http_response->get_http_version(), 1.1F);
  throw_assert_eq(http_response->get_body(), NULL);
  HTTPResponse::dec_ref(*http_response);
}

}
}
