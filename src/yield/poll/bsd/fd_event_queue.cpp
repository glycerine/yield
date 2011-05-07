// yield/poll/bsd/fd_event_queue.cpp

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

#include "yield/poll/bsd/fd_event_queue.hpp"

namespace yield {
namespace poll {
namespace bsd {
FDEventQueue::FDEventQueue()
  : kq(kq) {
  memcpy(this->wake_pipe, wake_pipe, sizeof(this->wake_pipe));
}

FDEventQueue::~FDEventQueue() {
  close(kq);
  close(wake_pipe[0]);
  close(wake_pipe[1]);
}

bool FDEventQueue::associate(fd_t fd, uint16_t fd_event_types) {
  if (fd_event_types > 0) {
    kevent kevent_;

    if (fd_event_types & POLLIN) {
      EV_SET(&kevent_, fd, EVFILT_READ, EV_ADD, 0, 0, context);
      if (kevent(kq, &kevent_, 1, 0, 0, NULL) == -1)
        return false;
    } else {
      EV_SET(&kevent_, fd, EVFILT_WRITE, EV_DELETE, 0, 0, context);
      kevent(kq, &kevent_, 1, 0, 0, NULL);   // Ignore the result
    }

    if (fd_event_types & POLLOUT) {
      EV_SET(&kevent_, fd, EVFILT_WRITE, EV_ADD, 0, 0, context);
      if (kevent(kq, &kevent_, 1, 0, 0, NULL) == -1)
        return false;
    } else {
      EV_SET(&kevent_, fd, EVFILT_WRITE, EV_DELETE, 0, 0, context);
      kevent(kq, &kevent_, 1, 0, 0, NULL);   // Ignore the result
    }
  } else
    dissociate(fd);

  return true;
}

FDEventQueue* FDEventQueue::create() {
  int kq = kqueue();
  if (kq != -1) {
    int wake_pipe[2];
    if (pipe(wake_pipe) != -1) {
      kevent kevent_;
      EV_SET(&kevent_, wake_pipe[0], EVFILT_READ, EV_ADD, 0, 0, 0);
      if (kevent(kq, &kevent_, 1, 0, 0, NULL) != -1)
        return new FDEventQueue(kq, wake_pipe);

      close(wake_pipe[0]);
      close(wake_pipe[1]);
    }

    close(kq);
  }

  return NULL;
}

bool FDEventQueue::dissociate(fd_t fd) {
  kevent kfd_events[2];
  EV_SET(&kfd_events[0], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  EV_SET(&kfd_events[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  return kevent(kq, kevents, 2, 0, 0, NULL) != -1;
}

int16_t
FDEventQueue::poll
(
  FDEvent* fd_events,
  int16_t fd_events_len,
  const Time& timeout
) {
  if (fd_events_len > kevents.size())
    kevents.resize(fd_events_len);

  timespec timeout_ts = timeout;

  int ret = kevent(epfd, 0, 0, &kfd_events[0], fd_events_len, &timeout_ts);

  if (ret > 0) {
    int16_t event_i = 0;

    for (int16_t kevent_i = 0; kevent_i < ret; kevent_i++) {
      if (kfd_events[kevent_i].ident == wake_pipe[0]) {
        uint8_t data;
        read(wake_pipe[0], data, sizeof(data));
      } else {
        if (kfd_events[kevent_i].filter == EVFILT_WRITE)
          fd_events[event_i].set_events(POLLOUT);
        else
          fd_events[event_i].set_events(POLLIN);
        fd_events[event_i].set_fd(kfd_events[kevent_i].ident);
        if (++event_i == fd_events_len) break;
      }
    }

    return event_i;
  } else
    return static_cast<int16_t>(ret);
}

void FDEventQueue::wake() {
  write(wake_pipe[1], "m", 1);
}
}
}
}
