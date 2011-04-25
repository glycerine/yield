// test_http_request_handler.hpp

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

#ifndef _YIELD_HTTP_SERVER_TEST_HTTP_REQUEST_HANDLER_HPP_
#define _YIELD_HTTP_SERVER_TEST_HTTP_REQUEST_HANDLER_HPP_

#include "yield/assert.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/path.hpp"
#include "yield/http/server/http_request_handler.hpp"

namespace yield {
namespace http {
namespace server {
class TestHTTPRequestHandler : public HTTPRequestHandler {
  void handle(YO_NEW_REF HTTPRequest& http_request) {
    if (http_request.get_uri().get_path() == "/")
      http_request.respond(200, "Hello world");
    else if (http_request.get_uri().get_path() == "/drop")
      ;
    else if (http_request.get_uri().get_path() == "/sendfile") {
      yield::fs::File* file
#ifdef _WIN32
        = yield::fs::FileSystem().open("yield.http.server.Makefile");
#else
        = yield::fs::FileSystem().open("Makefile");
#endif
      if (file != NULL)
        http_request.respond(200, *file);
      else
        http_request.respond(404);
    }
    else
      http_request.respond(404);

    HTTPRequest::dec_ref(http_request);
  }
};
}
}
}

#endif
