// http_message_parser_test.cpp

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
#include "yield/http/http_message_body_chunk.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_request_parser.hpp"
#include "yunit.hpp"

TEST_SUITE(HTTPMessageParser);

namespace yield {
namespace http {
TEST(HTTPMessageParser, MalformedFieldMissingColon) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost\r\n\r\n");
  HTTPRequest* http_request = Object::cast<HTTPRequest>(http_request_parser.parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPMessageParser, MalformedFieldMissingName) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\n: localhost\r\n\r\n");
  HTTPRequest* http_request = Object::cast<HTTPRequest>(http_request_parser.parse());
  throw_assert_eq(http_request, NULL);
}

TEST(HTTPMessageParser, WellFormedChunkedBodyWithChunkExtension) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n1;chunk_ext1;chunk_ext2=\"ChunkExtension\"\r\nx\r\n0\r\n\r\n");
  HTTPRequest* http_request = Object::cast<HTTPRequest>(http_request_parser.parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_eq(http_request->get_http_version(), 1);
  {
    HTTPMessageBodyChunk* http_message_body_chunk = Object::cast<HTTPMessageBodyChunk>(http_request_parser.parse());
    throw_assert_ne(http_message_body_chunk, NULL);
    throw_assert_eq(http_message_body_chunk->size(), 1);
    HTTPMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  {
    HTTPMessageBodyChunk* http_message_body_chunk = Object::cast<HTTPMessageBodyChunk>(http_request_parser.parse());
    throw_assert_ne(http_message_body_chunk, NULL);
    throw_assert_eq(http_message_body_chunk->size(), 0);
    HTTPMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  HTTPRequest::dec_ref(http_request);
}

TEST(HTTPMessageParser, WellFormedChunkedBodyWithTrailer) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n1\r\nx\r\n1\r\ny\r\n0\r\nHost: localhost\r\nX-Host: x-localhost\r\n\r\n");
  HTTPRequest* http_request = Object::cast<HTTPRequest>(http_request_parser.parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_eq(http_request->get_http_version(), 1);
  {
    HTTPMessageBodyChunk* http_message_body_chunk = Object::cast<HTTPMessageBodyChunk>(http_request_parser.parse());
    throw_assert_ne(http_message_body_chunk, NULL);
    throw_assert_eq(http_message_body_chunk->size(), 1);
    HTTPMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  {
    HTTPMessageBodyChunk* http_message_body_chunk = Object::cast<HTTPMessageBodyChunk>(http_request_parser.parse());
    throw_assert_ne(http_message_body_chunk, NULL);
    throw_assert_eq(http_message_body_chunk->size(), 1);
    HTTPMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  {
    HTTPMessageBodyChunk* http_message_body_chunk = Object::cast<HTTPMessageBodyChunk>(http_request_parser.parse());
    throw_assert_ne(http_message_body_chunk, NULL);
    throw_assert_eq(http_message_body_chunk->size(), 0);
    HTTPMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  HTTPRequest::dec_ref(http_request);
}

TEST(HTTPMessageParser, WellFormed1ChunkBody) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n1\r\nx\r\n0\r\n\r\n");
  HTTPRequest* http_request = Object::cast<HTTPRequest>(http_request_parser.parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_eq(http_request->get_http_version(), 1);
  throw_assert_eq((*http_request)["Host"], "localhost");
  {
    HTTPMessageBodyChunk* http_message_body_chunk = Object::cast<HTTPMessageBodyChunk>(http_request_parser.parse());
    throw_assert_ne(http_message_body_chunk, NULL);
    throw_assert_eq(http_message_body_chunk->size(), 1);
    HTTPMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  {
    HTTPMessageBodyChunk* http_message_body_chunk = Object::cast<HTTPMessageBodyChunk>(http_request_parser.parse());
    throw_assert_ne(http_message_body_chunk, NULL);
    throw_assert_eq(http_message_body_chunk->size(), 0);
    HTTPMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  HTTPRequest::dec_ref(http_request);
}

TEST(HTTPMessageParser, WellFormed2ChunkBody) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n1\r\nx\r\n1\r\ny\r\n0\r\n\r\n");
  HTTPRequest* http_request = Object::cast<HTTPRequest>(http_request_parser.parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_eq(http_request->get_http_version(), 1);
  throw_assert_eq((*http_request)["Host"], "localhost");
  {
    HTTPMessageBodyChunk* http_message_body_chunk = Object::cast<HTTPMessageBodyChunk>(http_request_parser.parse());
    throw_assert_ne(http_message_body_chunk, NULL);
    throw_assert_eq(http_message_body_chunk->size(), 1);
    HTTPMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  {
    HTTPMessageBodyChunk* http_message_body_chunk = Object::cast<HTTPMessageBodyChunk>(http_request_parser.parse());
    throw_assert_ne(http_message_body_chunk, NULL);
    throw_assert_eq(http_message_body_chunk->size(), 1);
    HTTPMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  {
    HTTPMessageBodyChunk* http_message_body_chunk = Object::cast<HTTPMessageBodyChunk>(http_request_parser.parse());
    throw_assert_ne(http_message_body_chunk, NULL);
    throw_assert_eq(http_message_body_chunk->size(), 0);
    HTTPMessageBodyChunk::dec_ref(http_message_body_chunk);
  }
  HTTPRequest::dec_ref(http_request);
}

TEST(HTTPMessageParser, WellFormedFieldMissingValue) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost:\r\n\r\n");
  HTTPRequest* http_request = Object::cast<HTTPRequest>(http_request_parser.parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_eq(http_request->get_http_version(), 1);
  throw_assert_eq((*http_request)["Host"], "");
  HTTPRequest::dec_ref(http_request);
}

TEST(HTTPMessageParser, WellFormedNoBody) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
  HTTPRequest* http_request = Object::cast<HTTPRequest>(http_request_parser.parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_eq(http_request->get_body(), NULL);
  HTTPRequest::dec_ref(http_request);
}

TEST(HTTPMessageParser, WellFormedNormalBody) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12");
  HTTPRequest* http_request = Object::cast<HTTPRequest>(http_request_parser.parse());
  throw_assert_ne(http_request, NULL);
  throw_assert_ne(http_request->get_body(), NULL);
  throw_assert_eq(static_cast<Buffer*>(http_request->get_body())->size(), 2);
  HTTPRequest::dec_ref(http_request);
}

TEST(HTTPMessageParser, WellFormedPipelinedNoBody) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\n\r\nGET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
  {
    HTTPRequest* http_request = Object::cast<HTTPRequest>(http_request_parser.parse());
    throw_assert_ne(http_request, NULL);
    throw_assert_eq(http_request->get_http_version(), 1);
    throw_assert_eq((*http_request)["Host"], "localhost");
    throw_assert_eq(http_request->get_body(), NULL);
    HTTPRequest::dec_ref(http_request);
  }
  {
    HTTPRequest* http_request = Object::cast<HTTPRequest>(http_request_parser.parse());
    throw_assert_ne(http_request, NULL);
    throw_assert_eq(http_request->get_http_version(), 1);
    throw_assert_eq((*http_request)["Host"], "localhost");
    throw_assert_eq(http_request->get_body(), NULL);
    HTTPRequest::dec_ref(http_request);
  }
}

TEST(HTTPMessageParser, WellFormedPipelinedNormalBody) {
  HTTPRequestParser http_request_parser("GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12GET / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n12");
  {
    HTTPRequest* http_request = Object::cast<HTTPRequest>(http_request_parser.parse());
    throw_assert_ne(http_request, NULL);
    throw_assert_eq(http_request->get_http_version(), 1);
    throw_assert_eq((*http_request)["Host"], "localhost");
    throw_assert_ne(http_request->get_body(), NULL);
    throw_assert_eq(static_cast<Buffer*>(http_request->get_body())->size(), 2);
    HTTPRequest::dec_ref(http_request);
  }
  {
    HTTPRequest* http_request = Object::cast<HTTPRequest>(http_request_parser.parse());
    throw_assert_ne(http_request, NULL);
    throw_assert_eq(http_request->get_http_version(), 1);
    throw_assert_eq((*http_request)["Host"], "localhost");
    throw_assert_ne(http_request->get_body(), NULL);
    throw_assert_eq(static_cast<Buffer*>(http_request->get_body())->size(), 2);
    HTTPRequest::dec_ref(http_request);
  }
}
}
}
