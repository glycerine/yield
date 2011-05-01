// yield/poll/sunos/fd_event_queue.cpp

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

#include "event_port.hpp"


namespace yield {
namespace poll {
namespace sunos {
EventPort::EventPort(int port)
  : port(port)
{ }

EventPort::~EventPort() {
  close(port);
}

bool EventPort::associate(fd_t fd, uint16_t fd_event_types) {
  if (fd_event_types > 0) {
    return port_associate(
             port,
             PORT_SOURCE_FD,
             fd,
             fd_event_types,
             NULL
           ) != -1;
  } else {
    dissociate(fd);
    return true;
  }
}

EventPort* EventPort::create() {
  int port = port_create();
  if (port != -1)
    return new EventPort(port);
  else
    return NULL;
}

bool EventPort::dissociate(fd_t fd) {
  return port_dissociate(port, PORT_SOURCE_FD, fd) != -1;
}

int16_t
EventPort::poll
(
  FDEvent* fd_events,
  int16_t fd_events_len,
  const Time& timeout
) {
  if (fd_events_len > port_events.size())
    port_events.resize(fd_events_len);

  uint_t max = fd_events_len, nget;
  timespec timeout_ts = timeout;

  int ret = port_getn(port, port_events, max, &nget, &timeout_ts);

  if (ret == 0) {
    int16_t event_i = 0;

    for (uint_t port_event_i = 0; port_event_i < nget; port_event_i++) {
      const port_event_t& port_event = port_fd_events[port_event_i];

      if (port_event.portev_source != PORT_SOURCE_USER) {
        fd_events[event_i].set_events(port_event.portev_events);
        fd_events[event_i].set_fd(port_event.portev_object);
        if (++event_i == fd_events_len) break;
      }
    }

    return event_i;
  } else
    return static_cast<int16_t>(ret);
}
}

void EventPort::wake() {
  port_send(port, 0, NULL);
}
}
}
