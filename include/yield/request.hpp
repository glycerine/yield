// yield/request.hpp

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

#ifndef _YIELD_REQUEST_HPP_
#define _YIELD_REQUEST_HPP_

#include "yield/message.hpp"

namespace yield {
class EventHandler;
class Response;

/**
  Abstract base class for requests in the event-driven concurrency subsystem.
*/
class Request : public Message {
public:
  virtual ~Request();

public:
  /**
    Get the response handler for this request.
    @return the response handler
  */
  EventHandler* get_response_handler() const {
    return response_handler;
  }

public:
  virtual void respond(Response& response);

public:
  /**
    Set the response handler for this request.
    Calls to Request::respond(Response& response) delegate to
      response_handler->handle(response).
    Steals the reference to response_handler.
    @param response_handler a new reference to a response handler
  */
  void set_response_handler(YO_NEW_REF EventHandler* response_handler);

  /**
    Set the response handler for this request.
    Calls to Request::respond(Response& response) delegate to
      response_handler->handle(response).
    Creates a new reference from response_handler.
    @param response_handler a reference to a response handler
  */
  void set_response_handler(EventHandler& response_handler);

public:
  // yield::Object
  Request& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::Message
  bool is_request() const {
    return true;
  }

protected:
  /**
    Protected constructor for subclasses.
  */
  Request();

private:
  EventHandler* response_handler;
};
}

#endif
