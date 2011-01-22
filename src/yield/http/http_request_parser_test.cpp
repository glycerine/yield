// yield/http/http_request_parser_test.cpp

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


#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_request_parser.hpp"
#include "yunit.hpp"


TEST_SUITE(HTTPRequestParser);

namespace yield {
namespace http {
TEST(HTTPRequestParser, well_formed_pipelined_normal_body) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12");
  vector<HTTPRequest*> http_requests;

  for (uint32_t i = 0; i < 2; i++) {
    HTTPRequest* http_request = object_cast<HTTPRequest>(http_request_parser.parse());

    if (http_request != NULL)
      http_requests.push_back(http_request);
    else {
      while (!http_requests.empty()) {
        HTTPRequest::dec_ref(*http_requests.back());
        http_requests.pop_back();
      }

      throw_assert(false);
    }
  }

  throw_assert_eq(http_requests.size(), 2);
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq(http_requests[i]->get_method(), HTTPRequest::METHOD_GET);
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq(http_requests[i]->get_http_version(), 1.1F);
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq((*http_requests[i])["Host"], "localhost");
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_ne(http_requests[i]->get_body(), NULL);
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq(http_requests[i]->get_content_length(), 2);
  }
  for (size_t i = 0; i < 2; i++) HTTPRequest::dec_ref(*http_requests[i]);
}

TEST(HTTPRequestParser, well_formed_pipelined_no_body) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\n\r\nGET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
  vector<HTTPRequest*> http_requests;

  for (uint32_t i = 0; i < 2; i++) {
    HTTPRequest* http_request = object_cast<HTTPRequest>(http_request_parser.parse());

    if (http_request != NULL)
      http_requests.push_back(http_request);
    else {
      while (!http_requests.empty()) {
        HTTPRequest::dec_ref(*http_requests.back());
        http_requests.pop_back();
      }

      throw_assert(false);
    }
  }

  throw_assert_eq(http_requests.size(), 2);
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq(http_requests[i]->get_method(), HTTPRequest::METHOD_GET);
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq(http_requests[i]->get_http_version(), 1.1F);
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq((*http_requests[i])["Host"], "localhost");
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq(http_requests[i]->get_body(), NULL);
  }
  for (size_t i = 0; i < 2; i++) HTTPRequest::dec_ref(*http_requests[i]);
}

TEST(HTTPRequestParser, well_formed_normal_body) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12").parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_ne(http_request->get_body(), NULL);
  throw_assert_eq(http_request->get_content_length(), 2);
  HTTPRequest::dec_ref(*http_request);
}

TEST(HTTPRequestParser, well_formed_no_fields) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / HTTP/1.1\r\n\r\n").parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_eq(http_request->get_http_version(), 1.1F);
  throw_assert_eq(http_request->get_body(), NULL);
  HTTPRequest::dec_ref(*http_request);
}

TEST(HTTPRequestParser, well_formed_no_body) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_eq(http_request->get_method(), HTTPRequest::METHOD_GET);
  throw_assert_eq(http_request->get_http_version(), 1.1F);
  throw_assert_eq((*http_request)["Host"], "localhost");
  throw_assert_eq(http_request->get_body(), NULL);
  HTTPRequest::dec_ref(*http_request);
}

TEST(HTTPRequestParser, well_formed_chunk_2_body) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n1\r\nx\r\n1\r\ny\r\n0\r\n\r\n");
  vector<HTTPRequest*> http_requests;

  for (uint32_t i = 0; i < 3; i++) {
    HTTPRequest* http_request = object_cast<HTTPRequest>(http_request_parser.parse());

    if (http_request != NULL)
      http_requests.push_back(http_request);
    else {
      while (!http_requests.empty()) {
        HTTPRequest::dec_ref(*http_requests.back());
        http_requests.pop_back();
      }

      throw_assert(false);
    }
  }

  throw_assert_eq(http_requests.size(), 3);
  for (size_t i = 0; i < 3; i++) {
    throw_assert_eq(http_requests[i]->get_method(), HTTPRequest::METHOD_GET);
  }
  for (size_t i = 0; i < 3; i++) {
    throw_assert_eq(http_requests[i]->get_http_version(), 1.1F);
  }
  for (size_t i = 0; i < 3; i++) {
    throw_assert_eq((*http_requests[i])["Host"], "localhost");
  }
  for (size_t i = 0; i < 3; i++) {
    throw_assert_ne(http_requests[i]->get_body(), NULL);
  }
  throw_assert_eq(http_requests[0]->get_content_length(), 1);
  throw_assert_eq(http_requests[1]->get_content_length(), 1);
  throw_assert_eq(http_requests[2]->get_content_length(), 0);
  for (size_t i = 0; i < 3; i++) HTTPRequest::dec_ref(*http_requests[i]);
}

TEST(HTTPRequestParser, well_formed_chunk_1_body) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n1\r\nx\r\n0\r\n\r\n");
  vector<HTTPRequest*> http_requests;

  for (uint32_t i = 0; i < 2; i++) {
    HTTPRequest* http_request = object_cast<HTTPRequest>(http_request_parser.parse());

    if (http_request != NULL)
      http_requests.push_back(http_request);
    else {
      while (!http_requests.empty()) {
        HTTPRequest::dec_ref(*http_requests.back());
        http_requests.pop_back();
      }

      throw_assert(false);
    }
  }

  throw_assert_eq(http_requests.size(), 2);
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq(http_requests[i]->get_method(), HTTPRequest::METHOD_GET);
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_eq(http_requests[i]->get_http_version(), 1.1F);
  }
  for (size_t i = 0; i < 2; i++) {
    throw_assert_ne(http_requests[i]->get_body(), NULL);
  }
  throw_assert_eq(http_requests[0]->get_content_length(), 1);
  throw_assert_eq(http_requests[1]->get_content_length(), 0);
  for (size_t i = 0; i < 2; i++) HTTPRequest::dec_ref(*http_requests[i]);
}

TEST(HTTPRequestParser, malformed_version_missing_trailing_crlf) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / HTTP/1.1Host: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, malformed_version_missing_minor_version) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / HTTP/1.\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, malformed_version_missing_http) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET / /1.0\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, malformed_version_missing) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET /\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, malformed_uri_missing) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET HTTP/1.1\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, malformed_uri_embedded_lf) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("GET /\r HTTP/1.1\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPRequestParser, malformed_method_missing) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser("/ HTTP/1.0\r\nHost: localhost\r\n\r\n").parse());
  throw_assert_eq(http_request, NULL);
}
}
}
