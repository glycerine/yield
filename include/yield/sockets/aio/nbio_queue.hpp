// yield/sockets/aio/nbio_queue.hpp

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

#ifndef _YIELD_SOCKETS_AIO_NBIO_QUEUE_HPP_
#define _YIELD_SOCKETS_AIO_NBIO_QUEUE_HPP_

#include "yield/event_queue.hpp"
#include "yield/sockets/aio/aiocb.hpp"
#include "yield/sockets/poll/socket_event_queue.hpp"

#include <map>


namespace yield {
namespace sockets {
namespace aio {
class AIOCB;

class NBIOQueue : public EventQueue {
public:
  bool associate(socket_t) {
    return true;
  }

public:
  // yield::Object
  NBIOQueue& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::EventQueue
  YO_NEW_REF Event& dequeue() {
    return EventQueue::dequeue();
  }

  YO_NEW_REF Event* dequeue(const Time& timeout);

  bool enqueue(YO_NEW_REF Event&);

  YO_NEW_REF Event* trydequeue() {
    return EventQueue::trydequeue();
  }

private:
  class SocketState {
  public:
    SocketState() {
      memset(aiocb_queues, 0, sizeof(aiocb_queues));
    }

    AIOCB* aiocb_queues[4]; // accept, connect, send, recv
  };

  yield::sockets::poll::SocketEventQueue socket_event_queue;
  std::map<socket_t, SocketState*> state;
};
}
}
}

#endif
