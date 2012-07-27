// yield/poll/fd_event_queue.hpp

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

#ifndef _YIELD_POLL_FD_EVENT_QUEUE_HPP_
#define _YIELD_POLL_FD_EVENT_QUEUE_HPP_

#include "yield/exception.hpp"
#include "yield/event_queue.hpp"
#include "yield/poll/fd_event.hpp"
#include "yield/queue/blocking_concurrent_queue.hpp"

#ifndef _WIN32
#include <sys/poll.h>
#endif

namespace yield {
namespace poll {
/**
  EventQueue for file descriptor events (<code>FDEvent</code>s).
  Implemented in terms of efficient poll() variants on different platforms.
*/
class FDEventQueue : public EventQueue {
public:
  /**
    Construct an FDEventQueue, allocating any associated system resources.
    @param for_sockets_only true if this FDEventQueue is only for sockets
  */
  FDEventQueue(bool for_sockets_only = false) throw(Exception);

  /**
    Destroy an FDEventQueue, deallocating any associated system resources.
  */
  ~FDEventQueue();

public:
  /**
    Associate a file descriptor with this FDEventQueue,
      watching for the specified FDEvent types (read readiness,
      write readiness, et al.).
   @param fd the file descriptor to associate
   @param fd_event_types FDEvent types to monitor
   @return true on success, false+errno on failure
  */
  bool associate(fd_t fd, FDEvent::Type fd_event_types);

  /**
    Dissociate a file descriptor from this FDEventQueue.
    @param fd the file descriptor to dissociate
    @return true on success, false+errno on failure
  */
  bool dissociate(fd_t fd);

public:
  // yield::EventQueue
  bool enqueue(YO_NEW_REF Event& event);
  YO_NEW_REF Event* timeddequeue(const Time& timeout);

private:
  ::yield::queue::BlockingConcurrentQueue<Event> event_queue;
#if defined(__linux__)
  int epfd, wake_fd;
#elif defined(__MACH__) || defined(__FreeBSD__)
  int kq, wake_pipe[2];
#elif defined(__sun)
  int port;
#elif defined(_WIN32)
  class FDImpl;
  class Impl;
  class SocketImpl;
  class SocketPoller;
  class SocketSelector;
  Impl* pimpl;
#else
  vector<pollfd> pollfds;
  int wake_pipe[2];
#endif
};
}
}

#endif
