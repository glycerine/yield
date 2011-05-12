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
class Log;

namespace sockets {
namespace aio {
class acceptAIOCB;
class connectAIOCB;
class recvAIOCB;
class sendAIOCB;
class sendfileAIOCB;

class NBIOQueue : public EventQueue {
public:
  NBIOQueue(YO_NEW_REF Log* log = NULL);
  ~NBIOQueue();

public:
  bool associate(socket_t) {
    return true;
  }

public:
  // yield::Object
  const char* get_type_name() const {
    return "yield::sockets::aio::NBIOQueue";
  }

  NBIOQueue& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::EventQueue
  bool enqueue(YO_NEW_REF Event&);
  YO_NEW_REF Event* timeddequeue(const Time& timeout);

private:
  class AIOCBState;

  enum RetryStatus {
    RETRY_STATUS_COMPLETE,
    RETRY_STATUS_ERROR,
    RETRY_STATUS_WANT_RECV,
    RETRY_STATUS_WANT_SEND
  };

  class SocketState;

private:
  void associate(AIOCB& aiocb, RetryStatus retry_status);

private:
  template <class AIOCBType> void log_completion(AIOCBType&);
  template <class AIOCBType> void log_error(AIOCBType&);
  template <class AIOCBType> void log_retry(AIOCBType&);
  template <class AIOCBType> void log_partial_send(AIOCBType&, size_t);
  template <class AIOCBType> void log_wouldblock(AIOCBType&, RetryStatus);

private:
  static uint8_t get_aiocb_priority(const AIOCB& aiocb);

private:
  RetryStatus retry(AIOCB&, size_t& partial_send_len);
  RetryStatus retry_accept(acceptAIOCB&);
  RetryStatus retry_connect(connectAIOCB&, size_t& partial_send_len);
  RetryStatus retry_recv(recvAIOCB&);
  RetryStatus retry_send(sendAIOCB&, size_t& partial_send_len);
  template <class AIOCBType>
  RetryStatus retry_send(AIOCBType&, const Buffer&, size_t& partial_send_len);
  RetryStatus retry_sendfile(sendfileAIOCB&, size_t& partial_send_len);

private:
  Log* log;
  yield::sockets::poll::SocketEventQueue socket_event_queue;
  std::map<socket_t, SocketState*> socket_state;
};
}
}
}

#endif
