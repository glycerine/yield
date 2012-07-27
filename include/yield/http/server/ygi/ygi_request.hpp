// yield/http/server/ygi/ygi_request.hpp

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

#ifndef _YIELD_HTTP_SERVER_YGI_YGI_REQUEST_HPP_
#define _YIELD_HTTP_SERVER_YGI_YGI_REQUEST_HPP_

#include "yield/http/server/http_request.hpp"

#include <ygi.h>

namespace yield {
namespace http {
namespace server {
namespace ygi {
class YGIRequest : public ygi_request_t {
public:
  YGIRequest(HTTPRequest& http_request);

public:
  size_t CONTENT_LENGTH();
  iovec DOCUMENT_ROOT();
  iovec HTTP_(const char* field_name);
  iovec HTTP_REFERER();
  iovec HTTP_USER_AGENT();
  iovec PATH_INFO();
  iovec PATH_TRANSLATED();
  iovec QUERY_STRING();
  iovec REMOTE_ADDR();
  iovec REMOTE_HOST();
  iovec REQUEST_METHOD();
  iovec SCRIPT_NAME();
  iovec SERVER_NAME();
  uint16_t SERVER_PORT();

public:
  ygi_response_t* respond(uint16_t status_code);

private:
  static size_t __CONTENT_LENGTH(ygi_request_t* this_) {
    return reinterpret_cast<YGIRequest*>(this_)->CONTENT_LENGTH();
  }

  static iovec __DOCUMENT_ROOT(ygi_request_t* this_) {
    return reinterpret_cast<YGIRequest*>(this_)->DOCUMENT_ROOT();
  }

  static iovec __HTTP_(ygi_request_t* this_, const char* field_name) {
    return reinterpret_cast<YGIRequest*>(this_)->HTTP_(field_name);
  }

  static iovec __HTTP_REFERER(ygi_request_t* this_) {
    return reinterpret_cast<YGIRequest*>(this_)->HTTP_REFERER();
  }

  static iovec __HTTP_USER_AGENT(ygi_request_t* this_) {
    return reinterpret_cast<YGIRequest*>(this_)->HTTP_USER_AGENT();
  }

  static iovec __PATH_INFO(ygi_request_t* this_) {
    return reinterpret_cast<YGIRequest*>(this_)->PATH_INFO();
  }

  static iovec __PATH_TRANSLATED(ygi_request_t* this_) {
    return reinterpret_cast<YGIRequest*>(this_)->PATH_TRANSLATED();
  }

  static iovec __QUERY_STRING(ygi_request_t* this_) {
    return reinterpret_cast<YGIRequest*>(this_)->QUERY_STRING();
  }

  static iovec __REMOTE_ADDR(ygi_request_t* this_) {
    return reinterpret_cast<YGIRequest*>(this_)->REMOTE_ADDR();
  }

  static iovec __REMOTE_HOST(ygi_request_t* this_) {
    return reinterpret_cast<YGIRequest*>(this_)->REMOTE_HOST();
  }

  static iovec __REQUEST_METHOD(ygi_request_t* this_) {
    return reinterpret_cast<YGIRequest*>(this_)->REQUEST_METHOD();
  }

  static iovec __SCRIPT_NAME(ygi_request_t* this_) {
    return reinterpret_cast<YGIRequest*>(this_)->SCRIPT_NAME();
  }

  static iovec __SERVER_NAME(ygi_request_t* this_) {
    return reinterpret_cast<YGIRequest*>(this_)->SERVER_NAME();
  }

  static uint16_t __SERVER_PORT(ygi_request_t* this_) {
    return reinterpret_cast<YGIRequest*>(this_)->SERVER_PORT();
  }

  static ygi_response_t*
  __respond(
    ygi_request_t* this_,
    uint16_t status_code
  ) {
    return reinterpret_cast<YGIRequest*>(this_)->respond(status_code);
  }

  ygi_response_t* __respond(uint16_t status_code);

private:
  HTTPRequest& http_request;
};
}
}
}
}

#endif
