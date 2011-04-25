// yield/poll/fd_event.hpp

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

#ifndef _YIELD_POLL_FD_EVENT_HPP_
#define _YIELD_POLL_FD_EVENT_HPP_

#include "yield/event.hpp"


#ifdef _WIN32
#ifndef POLLIN
#define POLLRDNORM  0x0100
#define POLLRDBAND  0x0200
#define POLLIN      (POLLRDNORM | POLLRDBAND)
#define POLLPRI     0x0400

#define POLLWRNORM  0x0010
#define POLLOUT     (POLLWRNORM)
#define POLLWRBAND  0x0020

#define POLLERR     0x0001
#define POLLHUP     0x0002
#define POLLNVAL    0x0004
#endif
#else
#include <poll.h>
#endif


namespace yield {
namespace poll {
class FDEvent : public Event {
public:
  const static uint32_t TYPE_ID = 117149474;

public:
  FDEvent() {
    events = POLLIN;
    fd = INVALID_FD;
  }

  FDEvent(uint16_t events, fd_t fd)
    : events(events), fd(fd)
  { }

#ifdef _WIN32
  FDEvent(uint16_t events, socket_t socket_)
    : events(events), fd(reinterpret_cast<fd_t>(socket_))
  { }
#endif

public:
  uint16_t get_events() const {
    return events;
  }

  fd_t get_fd() const {
    return fd;
  }

  socket_t get_socket() const {
#ifdef _WIN32
    return reinterpret_cast<socket_t>(get_fd());
#else
    return get_fd();
#endif
  }

public:
  operator uint16_t() const {
    return get_events();
  }

public:
  void set_events(uint16_t events) {
    this->events = events;
  }

  void set_fd(fd_t fd) {
    this->fd = fd;
  }

public:
  // yield::Object
  virtual uint32_t get_type_id() const {
    return TYPE_ID;
  }

  virtual const char* get_type_name() const {
    return "yield::poll::FDEvent";
  }

private:
  uint16_t events;
  fd_t fd;
};
}
}

#endif
