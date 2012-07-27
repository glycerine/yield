// yield/http/server/ygi/ygi_request.cpp

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

#include "yield/http/server/http_connection.hpp"
#include "yield/http/server/ygi/ygi_request.hpp"

namespace yield {
namespace http {
namespace server {
namespace ygi {
YGIRequest::YGIRequest(HTTPRequest& http_request)
  : http_request(http_request) {
  ygi_request_t::CONTENT_LENGTH = __CONTENT_LENGTH;
  ygi_request_t::DOCUMENT_ROOT = __DOCUMENT_ROOT;
  ygi_request_t::HTTP_ = __HTTP_;
  ygi_request_t::HTTP_REFERER = __HTTP_REFERER;
  ygi_request_t::HTTP_USER_AGENT = __HTTP_USER_AGENT;
  ygi_request_t::PATH_INFO = __PATH_INFO;
  ygi_request_t::PATH_TRANSLATED = __PATH_TRANSLATED;
  ygi_request_t::QUERY_STRING = __QUERY_STRING;
  ygi_request_t::REMOTE_ADDR = __REMOTE_ADDR;
  ygi_request_t::REMOTE_HOST = __REMOTE_HOST;
  ygi_request_t::REQUEST_METHOD = __REQUEST_METHOD;
  ygi_request_t::SCRIPT_NAME = __SCRIPT_NAME;
  ygi_request_t::SERVER_NAME = __SERVER_NAME;
  ygi_request_t::SERVER_PORT = __SERVER_PORT;
  ygi_request_t::respond = __respond;
}

size_t YGIRequest::CONTENT_LENGTH() {
  return http_request.get_content_length();
}

iovec YGIRequest::DOCUMENT_ROOT() {
  return iovec();
}

iovec YGIRequest::HTTP_(const char* field_name) {
  iovec field_value = { 0, 0 };
  http_request.get_field(field_name, field_value);
  return field_value;
}

iovec YGIRequest::HTTP_REFERER() {
  iovec field_value = { 0, 0 };
  http_request.get_field("Referer", field_value);
  return field_value;
}

iovec YGIRequest::HTTP_USER_AGENT() {
  iovec field_value = { 0, 0 };
  http_request.get_field("User-Agent", field_value);
  return field_value;
}

iovec YGIRequest::PATH_INFO() {
  return iovec();
}

iovec YGIRequest::PATH_TRANSLATED() {
  return iovec();
}

iovec YGIRequest::QUERY_STRING() {
  iovec query_string;
  http_request.get_uri().get_query(query_string);
  return query_string;
}

iovec YGIRequest::REMOTE_ADDR() {
  return iovec();
}

iovec YGIRequest::REMOTE_HOST() {
  return iovec();
}

iovec YGIRequest::REQUEST_METHOD() {
  iovec request_method;
  request_method.iov_base
  = const_cast<char*>(http_request.get_method().get_name());
  request_method.iov_len = http_request.get_method().get_name_len();
  return request_method;
}

ygi_response_t* YGIRequest::respond(uint16_t status_code) {
  http_request.respond(status_code);
  return NULL;
}

iovec YGIRequest::SCRIPT_NAME() {
  return iovec();
}

iovec YGIRequest::SERVER_NAME() {
  return iovec();
}

uint16_t YGIRequest::SERVER_PORT() {
  return 0;
}
}
}
}
}
