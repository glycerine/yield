// yield/poll/fd_event_queue.hpp

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

#ifndef _YIELD_POLL_FD_EVENT_QUEUE_HPP_
#define _YIELD_POLL_FD_EVENT_QUEUE_HPP_

#include "yield/event_queue.hpp"
#include "yield/poll/fd_event.hpp"


namespace yield {
namespace poll {
#if defined(__FreeBSD__) || defined(__MACH__) || defined(__OpenBSD__)
namespace bsd {
class Kqueue;
}
#elif defined(__linux__)
namespace linux {
class EPoller;
}
#elif defined(_WIN32)
namespace win32 {
class HandleEventQueue;
}
#elif defined(YIELD_HAVE_SOLARIS_EVENT_PORTS)
namespace sunos {
class EventPort;
}
#else
namespace posix {
class Poller;
}
#endif


class FDEventQueue : public EventQueue {
public:
  FDEventQueue();
  ~FDEventQueue();

  bool associate(fd_t fd, uint16_t events);
  bool dissociate(fd_t fd);

  // EventQueue
  YO_NEW_REF Event& dequeue() {
    return EventQueue::dequeue();
  }
  YO_NEW_REF Event* dequeue(const Time& timeout);
  bool enqueue(YO_NEW_REF Event& event);

private:
#if defined(__FreeBSD__) || defined(__MACH__) || defined(__OpenBSD__)
  bsd::Kqueue* pimpl;
#elif defined(__linux__)
  linux::EPoller* pimpl;
#elif defined(_WIN32)
  win32::HandleEventQueue* pimpl;
#elif defined(YIELD_HAVE_SOLARIS_EVENT_PORTS)
  sunos::EventPort* pimpl;
#else
  posix::Poller* pimpl;
#endif
};
}
}

#endif
