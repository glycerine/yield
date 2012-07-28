// http_request_test.cpp

// Copyright (c) 2012 Minor Gordon
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
#include "yield/buffer.hpp"
#include "yield/date_time.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_response.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace http {
TEST(HTTPRequest, constructor) {
  HTTPRequest(HTTPRequest::Method::GET, "/");
  HTTPRequest(HTTPRequest::Method::GET, "/", &Buffer::copy("test"));
  HTTPRequest(HTTPRequest::Method::GET, "/", &Buffer::copy("test"));
  HTTPRequest(HTTPRequest::Method::GET, "/", &Buffer::copy("test"), 1);
}

//TEST(HTTPRequest, get_creation_date_time) {
//  DateTime now = DateTime::now();
//  ASSERT_GE(
//    HTTPRequest(HTTPRequest::Method::GET, "/").get_creation_date_time(),
//    DateTime.now()
//  );
//}

TEST(HTTPRequest, get_method) {
  ASSERT_EQ(
    HTTPRequest(HTTPRequest::Method::GET, "/").get_method(),
    HTTPRequest::Method::GET
  );

  ASSERT_EQ(
    HTTPRequest(HTTPRequest::Method::PUT, "/").get_method(),
    HTTPRequest::Method::PUT
  );
}

TEST(HTTPRequest, get_uri) {
  yield::uri::URI uri("/test");
  ASSERT_EQ(HTTPRequest(HTTPRequest::Method::GET, uri).get_uri(), uri);
}

//TEST(HTTPRequest, respond) {
//  auto_Object<HTTPRequest> http_request
//    = new HTTPRequest(HTTPRequest::Method::GET, "/");
//
//  http_request->respond(*new HTTPResponse(200));
//  http_request->respond(404);
//  http_request->respond(404, "test");
//  http_request->respond(404, Buffer::copy("test"));
//  http_request->respond(404, &Buffer::copy("test"));
//  http_request->respond(*new Exception("exception"));
//}
}
}
