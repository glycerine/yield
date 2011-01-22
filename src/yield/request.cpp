// yield/request.cpp

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


#include "yield/event_handler.hpp"
#include "yield/request.hpp"
#include "yield/response.hpp"


namespace yield {
Request::Request() {
  // credentials = NULL;
  response_handler = NULL;
}

Request::~Request() {
  // MarshallableObject::dec_ref( credentials );
  EventHandler::dec_ref(response_handler);
}

void Request::respond(Response& response) {
  if (response_handler != NULL)
    response_handler->handle(response);
  else
    Response::dec_ref(response);
}

//void Request::set_credentials( MarshallableObject* credentials )
//{
//  MarshallableObject::dec_ref( this->credentials );
//  this->credentials = credentials;
//}

void Request::set_response_handler(EventHandler* response_handler) {
  EventHandler::dec_ref(this->response_handler);
  this->response_handler = response_handler;
}

void Request::set_response_handler(EventHandler& response_handler) {
  EventHandler::dec_ref(this->response_handler);
  this->response_handler = &response_handler.inc_ref();
}
}
