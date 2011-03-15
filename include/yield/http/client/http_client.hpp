// yield/http/client/http_client.hpp

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

#ifndef _YIELD_HTTP_CLIENT_HTTP_CLIENT_HPP_
#define _YIELD_HTTP_CLIENT_HTTP_CLIENT_HPP_

#include "yield/sockets/client/stream_socket_client.hpp"

namespace yield {
class Log;
namespace stage {
class Stage;
}

namespace http {
class HTTPRequest;
class HTTPResponse;

namespace client {
class HTTPClient : public yield::sockets::client::StreamSocketClient {
public:
  HTTPClient(
    const yield::uri::URI& uri,
    Log* error_log = NULL,
    Log* trace_log = NULL
  );

  HTTPClient(
    const Configuration& configuration,
    const yield::uri::URI& uri,
    Log* reror_log = NULL,
    Log* trace_log = NULL
  );

  ~HTTPClient();

public:
  static YO_NEW_REF HTTPResponse& GET(const yield::uri::URI& uri);

  //static
  //YO_NEW_REF HTTPResponse&
  //PUT
  //(
  //  const yield::uri::URI& uri,
  //  YO_NEW_REF Buffer& body
  //);

public:
  // Object
  HTTPClient& inc_ref() {
    return Object::inc_ref(*this);
  }

private:
  class Connection;

private:
  void init();

private:
  // Stage
  void service(YO_NEW_REF Event& event);

private:
  vector<Connection*> connections;
};
}
}
}

#endif
