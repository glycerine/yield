// http_message_test.cpp

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

#include "yield/auto_object.hpp"
#include "yield/assert.hpp"
#include "yield/string_buffer.hpp"
#include "yield/http/http_request.hpp"
#include "yunit.hpp"

TEST_SUITE(HTTPMessage);

namespace yield {
namespace http {
//TEST(HTTPMessage, get_body) {
//  throw_assert_eq(HTTPRequest(HTTPRequest::METHOD_GET, "/").get_body(), NULL);
//
//  auto_Object<Buffer> body = new StringBuffer("body");
//  throw_assert_eq(
//    memcmp(
//      HTTPRequest(HTTPRequest::METHOD_GET, "/", &body->inc_ref()).get_body(),
//      "body",
//      4
//    ),
//    0
//  );
//}
//
//TEST(HTTPMessage, get_content_length) {
//  throw_assert_eq(
//    HTTPRequest(HTTPRequest::METHOD_GET, "/").get_content_length(),
//    0
//  );
//
//  auto_Object<Buffer> body = new StringBuffer("body");
//  throw_assert_eq(
//    HTTPRequest(HTTPRequest::METHOD_GET, "/", &body->inc_ref()).get_content_length(),
//    4
//  );
//}
//
//TEST(HTTPMessage, get_date_field) {
//  auto_Object<HTTPRequest> http_request
//    = new HTTPRequest(HTTPRequest::METHOD_GET, "/");
//  http_request->set_field("Date", "Wed, 15 Nov 1995 06:25:24 GMT");
//  DateTime date = http_request->get_date_field("Date");
//}
//
TEST(HTTPMessage, get_field) {
  auto_Object<HTTPRequest> http_request
    = new HTTPRequest(HTTPRequest::METHOD_GET, "/");
  http_request->set_field("Date", "Wed, 15 Nov 1995 06:25:24 GMT");

  {
    iovec date;
    throw_assert(http_request->get_field("Date", date));
    throw_assert_eq(date.iov_len, 29);
    throw_assert_eq(memcmp(date.iov_base, "Wed, 15 Nov 1995 06:25:24 GMT", 29), 0);
  }

  {
    iovec date;
    throw_assert(http_request->get_field("Date", 4, date));
    throw_assert_eq(date.iov_len, 29);
    throw_assert_eq(memcmp(date.iov_base, "Wed, 15 Nov 1995 06:25:24 GMT", 29), 0);
  }

  {
    string date = http_request->get_field("Date");
    throw_assert_eq(date, "Wed, 15 Nov 1995 06:25:24 GMT");
  }

  throw_assert(http_request->get_field("DateX").empty());
}

TEST(HTTPMessage, get_fields) {
  auto_Object<HTTPRequest> http_request
    = new HTTPRequest(HTTPRequest::METHOD_GET, "/");
  http_request->set_field("Host", "localhost");
  http_request->set_field("XHost", "localhost");

  vector< pair<iovec, iovec> > fields;
  http_request->get_fields(fields);
  throw_assert_eq(fields.size(), 2);
  throw_assert_eq(fields[0].first.iov_len, 4);
  throw_assert_eq(fields[0].second.iov_len, 9);
  throw_assert_eq(fields[1].first.iov_len, 5);
  throw_assert_eq(fields[1].second.iov_len, 9);
}
}
}
