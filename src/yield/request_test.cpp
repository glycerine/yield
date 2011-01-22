// yield/request_test.cpp

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


#include "yield/auto_object.hpp"
#include "yield/assert.hpp"
#include "yield/event_handler.hpp"
#include "yield/request.hpp"
#include "yunit.hpp"


TEST_SUITE(Request);

namespace yield {
class TestEventHandler : public EventHandler {
public:
  // EventHandler
  void handle(Event&) { }
};


TEST(Request, createDefaultResponse) {
  throw_assert_eq(Request().createDefaultResponse(), NULL);
}

TEST(Request, get_response_handler) {
  throw_assert_eq(Request().get_response_handler(), NULL);
}

TEST(Request, is_request) {
  throw_assert(Request().is_request());
}

TEST(Request, respond) {
  Response response;

  {
    Request().respond(response.inc_ref());
  }

  {
    auto_Object<EventHandler> event_handler = new TestEventHandler;
    Request request;
    request.set_response_handler(*event_handler);
    request.respond(response);
  }
}

TEST(Request, rtti) {
  throw_assert_eq(Request().get_type_id(), Request::TYPE_ID);
  throw_assert_eq(strcmp(Request().get_type_name(), "yield::Request"), 0);
}

TEST(Request, set_response_handler) {
  {
    Request request;
    auto_Object<EventHandler> event_handler = new TestEventHandler;
    request.set_response_handler(*event_handler);
    throw_assert_eq(request.get_response_handler(), &event_handler.get());
  }

  {
    Request request;
    auto_Object<EventHandler> event_handler = new TestEventHandler;
    request.set_response_handler(&event_handler->inc_ref());
    throw_assert_eq(request.get_response_handler(), &event_handler.get());
  }
}
}
