// yield/http/server/http_request_queue.hpp

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

#ifndef _YIELD_HTTP_SERVER_HTTP_REQUEST_QUEUE_HPP_
#define _YIELD_HTTP_SERVER_HTTP_REQUEST_QUEUE_HPP_

#include "yield/sockets/socket_address.hpp"
#include "yield/sockets/aio/aio_queue.hpp"

namespace yield {
class Log;

namespace sockets {
namespace aio {
class acceptAIOCB;
}
class SocketAddress;
class TCPSocket;
}

namespace http {
namespace server {
class HTTPConnection;

template <class AIOQueueType = yield::sockets::aio::AIOQueue>
class HTTPRequestQueue : public EventQueue {
public:
  HTTPRequestQueue(
    const yield::sockets::SocketAddress& sockname,
    YO_NEW_REF Log* log = NULL
  );

  ~HTTPRequestQueue();

public:
  // yield::EventQueue
  YO_NEW_REF Event& dequeue() {
    return EventQueue::dequeue();
  }

  bool enqueue(YO_NEW_REF Event&);
  YO_NEW_REF Event* timeddequeue(const Time& timeout);

private:
  void handle(YO_NEW_REF yield::sockets::aio::acceptAIOCB& accept_aiocb);
  template <class AIOCBType> void handle(YO_NEW_REF AIOCBType& aiocb);

private:
  AIOQueueType& aio_queue;
  vector<HTTPConnection*> connections;
  Log* log;
  yield::sockets::TCPSocket& socket_;
};
}
}
}

#endif
