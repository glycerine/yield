// yield/http/http_response_parser_test.cpp

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
TEST(HTTPResponseParser, well_formed_reason_phrase_split) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 415 Unsupported Media Type\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_ne(http_response, NULL);
  throw_assert_eq(http_response->get_status_code(), 415);
  throw_assert_eq((*http_response)["Date"], "Fri, 31 Dec 1999 23:59:59 GMT");
  throw_assert_eq(http_response->get_body(), NULL);
  HTTPResponse::dec_ref(*http_response);
}

TEST(HTTPResponseParser, well_formed_pipelined_normal_body) {
  HTTPResponseParser http_response_parser("HTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\nContent-Length: 2\r\n\r\n12HTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\nContent-Length: 2\r\n\r\n12");
  vector<HTTPResponse*> http_responses;

  for (uint32_t i = 0; i < 2; i++) {
    HTTPResponse* http_response = object_cast<HTTPResponse>(http_response_parser.parse());

    if (http_response != NULL)
      http_responses.push_back(http_response);
    else {
      while (!http_responses.empty()) {
        HTTPResponse::dec_ref(*http_responses.back());
        http_responses.pop_back();
      }

      throw_assert(false);
    }
  }

  throw_assert_eq(http_responses.size(), 2);
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq(http_responses[i]->get_status_code(), 200);
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq((*http_responses[i])["Date"], "Fri, 31 Dec 1999 23:59:59 GMT");
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_ne(http_responses[i]->get_body(), NULL);
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq(http_responses[i]->get_content_length(), 2);
  }
  for (size_t i = 0; i < 2; i++) HTTPResponse::dec_ref(*http_responses[i]);
}

TEST(HTTPResponseParser, well_formed_pipelined_no_body) {
  HTTPResponseParser http_response_parser("HTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\nHTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n");
  vector<HTTPResponse*> http_responses;

  for (uint32_t i = 0; i < 2; i++) {
    HTTPResponse* http_response = object_cast<HTTPResponse>(http_response_parser.parse());

    if (http_response != NULL)
      http_responses.push_back(http_response);
    else {
      while (!http_responses.empty()) {
        HTTPResponse::dec_ref(*http_responses.back());
        http_responses.pop_back();
      }

      throw_assert(false);
    }
  }

  throw_assert_eq(http_responses.size(), 2);
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq(http_responses[i]->get_status_code(), 200);
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq((*http_responses[i])["Date"], "Fri, 31 Dec 1999 23:59:59 GMT");
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq(http_responses[i]->get_body(), NULL);
  }
  for (size_t i = 0; i < 2; i++) HTTPResponse::dec_ref(*http_responses[i]);
}

TEST(HTTPResponseParser, well_formed_normal_body) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\nContent-Length: 2\r\n\r\n12").parse());
  throw_assert_ne(http_response, NULL);
  throw_assert_eq(http_response->get_status_code(), 200);
  throw_assert_eq((*http_response)["Date"], "Fri, 31 Dec 1999 23:59:59 GMT");
  throw_assert_ne(http_response->get_body(), NULL);
  throw_assert_eq(http_response->get_content_length(), 2);
  HTTPResponse::dec_ref(*http_response);
}

TEST(HTTPResponseParser, well_formed_no_body) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 200 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_ne(http_response, NULL);
  throw_assert_eq(http_response->get_status_code(), 200);
  throw_assert_eq((*http_response)["Date"], "Fri, 31 Dec 1999 23:59:59 GMT");
  throw_assert_eq(http_response->get_body(), NULL);
  HTTPResponse::dec_ref(*http_response);
}

TEST(HTTPResponseParser, malformed_status_line_missing) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_eq(http_response, NULL);
}

TEST(HTTPResponseParser, malformed_status_code_missing) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_eq(http_response, NULL);
}

TEST(HTTPResponseParser, malformed_status_code_alpha) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 XX OK\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_eq(http_response, NULL);
}

TEST(HTTPResponseParser, malformed_reason_phrase_missing) {
  HTTPResponse* http_response = object_cast<HTTPResponse>(HTTPResponseParser("HTTP/1.1 200\r\nDate: Fri, 31 Dec 1999 23:59:59 GMT\r\n\r\n").parse());
  throw_assert_eq(http_response, NULL);
}
}
}
