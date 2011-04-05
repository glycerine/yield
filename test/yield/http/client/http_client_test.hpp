// http_client_test.hpp

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

#ifndef _YIELD_HTTP_CLIENT_HTTP_CLIENT_TEST_HPP_
#define _YIELD_HTTP_CLIENT_HTTP_CLIENT_TEST_HPP_

#include "../test_http_request_handler.hpp"
#include "yield/auto_object.hpp"
#include "yield/log.hpp"
#include "yield/http/http_response.hpp"
#include "yield/http/client/http_client.hpp"
#include "yield/http/server/http_server.hpp"
#include "yield/uri/uri.hpp"
#include "yield/stage/seda_stage_scheduler.hpp"
#include "yield/thread/synchronized_response_queue.hpp"
#include "yunit.hpp"

namespace yield {
namespace http {
namespace client {
using yield::http::server::HTTPServer;
using yield::uri::URI;
using yield::thread::SynchronizedResponseQueue;
using yield::stage::SEDAStageScheduler;

class HTTPClientTest : public yunit::Test {
protected:
  HTTPClientTest(const URI& uri)
    : uri(uri)
  { }

  const URI& get_uri() const {
    return uri;
  }

private:
  URI uri;
};


class HTTPClientGET200Test : public HTTPClientTest {
public:
  HTTPClientGET200Test(const URI& uri)
    : HTTPClientTest(uri)
  { }

  // Test
  void run() {
    auto_Object<HTTPResponse> http_response = HTTPClient::GET(get_uri());

    throw_assert_eq(http_response->get_status_code(), 200);
    throw_assert_ne(http_response->get_body(), NULL);
    throw_assert_gt(http_response->get_content_length(), 0);
  }
};


class HTTPClientGET404Test : public HTTPClientTest {
public:
  HTTPClientGET404Test(const URI& uri)
    : HTTPClientTest(uri)
  { }

  // Test
  void run() {
    auto_Object<HTTPResponse> http_response
    = HTTPClient::GET(get_uri() + "nowhere");

    throw_assert_eq(http_response->get_status_code(), 404);
  }
};


class HTTPClientGETRepeatedTest : public HTTPClientTest {
public:
  HTTPClientGETRepeatedTest(const URI& uri)
    : HTTPClientTest(uri)
  { }

  // Test
  void run() {
    auto_Object<HTTPClient> http_client = new HTTPClient(get_uri());

    auto_Object<HTTPRequest> http_request
    = new HTTPRequest(HTTPRequest::Method::GET, get_uri());

    auto_Object< SynchronizedResponseQueue<HTTPResponse> >
    http_response_queue = new SynchronizedResponseQueue<HTTPResponse>;
    http_request->set_response_handler(*http_response_queue);

    for (uint8_t i = 0; i < UINT8_MAX; i++) {
      http_client->handle(http_request->inc_ref());

      for (;;) {
        http_client->visit();
        HTTPResponse* http_response = http_response_queue->trydequeue();
        if (http_response != NULL) {
          auto_Object<HTTPResponse> http_response_(http_response);
          throw_assert_eq(http_response->get_status_code(), 200);
          break;
        }
      }
    }
  }
};


class HTTPClientGETTimeoutTest : public HTTPClientTest {
public:
  HTTPClientGETTimeoutTest(const URI& uri)
    : HTTPClientTest(uri)
  { }

  // Test
  void run() {
    try {
      HTTPClient::GET(get_uri() + "drop");
    } catch (Exception&) {
      return;
    }

    throw_assert(false);
  }
};


class HTTPClientGETWrongHostTest : public HTTPClientTest {
public:
  HTTPClientGETWrongHostTest(const URI& uri)
    : HTTPClientTest(uri)
  { }

  // Test
  void run() {
    try {
      HTTPClient::GET("http://localhostx:27095");
    } catch (Exception&) {
      return;
    }

    throw_assert(false);
  }
};


class HTTPClientGETWrongPortTest : public HTTPClientTest {
public:
  HTTPClientGETWrongPortTest(const URI& uri)
    : HTTPClientTest(uri)
  { }

  // Test
  void run() {
    try {
      HTTPClient::GET("http://localhost:26095");
    } catch (Exception&) {
      return;
    }

    throw_assert(false);
  }
};


//class HTTPClientPUTTest : public HTTPClientTest
//{
//public:
//  HTTPClientPUTTest( const URI& uri )
//    : HTTPClientTest( uri )
//  { }

//  // Test
//  void run()
//  {
//    auto_Object<HTTPResponse> http_response
//     = HTTPClient::PUT( get_uri(), *new StringBuffer( "test" ) );

//    throw_assert_eq( http_response->get_status_code(), 200 );
//  }
//};


class HTTPClientTestSuite : public yunit::TestSuite {
public:
  // yunit::TestSuite
  virtual int run() {
    auto_Object<Log> trace_log = Log::open(std::cout, Log::DEBUG);

    auto_Object<HTTPServer> http_server
    = new HTTPServer
    (
      *new TestHTTPRequestHandler,
      27095,
      NULL,
      &trace_log.get()
    );

    auto_Object<SEDAStageScheduler> stage_scheduler
    = new SEDAStageScheduler;

    stage_scheduler->schedule(http_server->inc_ref(), 1);

    addTests("http://localhost:27095/");

    int ret = yunit::TestSuite::run();

    return ret;
  }

protected:
  void addTests(const URI& uri) {
    debug_assert_eq(uri.get_port(), 27095);

    add
    (
      "HTTPClient::GET -> 200",
      new HTTPClientGET200Test(uri)
    );

    add
    (
      "HTTPClient::GET -> 404",
      new HTTPClientGET404Test(uri)
    );

    add
    (
      "HTTPClient::GET repeated",
      new HTTPClientGETRepeatedTest(uri)
    );

    //add
    //(
    //  "HTTPClient::GET -> timeout",
    //  new HTTPClientGETTimeoutTest( uri )
    //);

    add
    (
      "HTTPClient::GET( wrong host )",
      new HTTPClientGETWrongHostTest(uri)
    );

    add
    (
      "HTTPClient::GET( wrong port )",
      new HTTPClientGETWrongPortTest(uri)
    );

    //add( "HTTPClient::PUT", new HTTPClientPUTTest( uri ) );
  }
};
}
}
}

#endif
