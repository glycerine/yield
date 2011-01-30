// yield/poll/fd_event_queue.cpp

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

#if defined(__FreeBSD__) || defined(__MACH__) || defined(__OpenBSD__)
#include "bsd/kqueue.hpp"
#elif defined(__linux__)
#include "linux/epoller.hpp"
#elif defined(_WIN32)
#include "win32/handle_event_queue.hpp"
#elif defined(YIELD_HAVE_SOLARIS_EVENT_PORTS)
#include "sunos/event_port.hpp"
#else
#include "posix/poller.hpp"
#endif
#include "yield/poll/fd_event_queue.hpp"


namespace yield {
namespace poll {
FDEventQueue::FDEventQueue() {
#if defined(__FreeBSD__) || defined(__MACH__) || defined(__OpenBSD__)
  pimpl = new bsd::Kqueue;
#elif defined(__linux__)
  pimpl = new linux::EPoller;
#elif defined(_WIN32)
  pimpl = new win32::HandleEventQueue;
#elif defined(YIELD_HAVE_SOLARIS_EVENT_PORTS)
  pimpl = new sunos::EventPort;
#else
  pimpl = new posix::Poller;
#endif
}

FDEventQueue::~FDEventQueue() {
  delete pimpl;
}

bool FDEventQueue::associate(fd_t fd, uint16_t events) {
  return pimpl->associate(fd, events);
}

bool FDEventQueue::dissociate(fd_t fd) {
  return pimpl->dissociate(fd);
}

YO_NEW_REF Event* FDEventQueue::dequeue(const Time& timeout) {
  return pimpl->dequeue(timeout);
}

bool FDEventQueue::enqueue(YO_NEW_REF Event& event) {
  return pimpl->enqueue(event);
}
}
}
