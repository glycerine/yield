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

#include "yield/assert.hpp"
#include "yield/poll/fd_event_queue.hpp"

#include <errno.h>
#include <sys/event.h>

namespace yield {
namespace poll {
FDEventQueue::FDEventQueue() throw (Exception) {
  kq = kqueue();
  if (kq != -1) {
    try {
      if (pipe(wake_pipe) != -1) {
        try {
          if (!associate(wake_pipe[0], FDEvent::TYPE_READ_READY))
            throw Exception();
        } catch (Exception&) {
          close(wake_pipe[0]);
          close(wake_pipe[1]);
          throw;
        }
      } else
        throw Exception();
    } catch (Exception&) {
      close(kq);
      throw;
    }
  } else
    throw Exception();
}

FDEventQueue::~FDEventQueue() {
  close(kq);
  close(wake_pipe[0]);
  close(wake_pipe[1]);
}

bool FDEventQueue::associate(fd_t fd, FDEvent::Type fd_event_types) {
  if (fd_event_types > 0) {
    struct kevent kevent_;

    if (fd_event_types & FDEvent::TYPE_READ_READY) {
      EV_SET(&kevent_, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
      if (kevent(kq, &kevent_, 1, 0, 0, NULL) == -1)
        return false;
    } else {
      EV_SET(&kevent_, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
      kevent(kq, &kevent_, 1, 0, 0, NULL); // Ignore the result
    }

    if (fd_event_types & FDEvent::TYPE_WRITE_READY) {
      EV_SET(&kevent_, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
      if (kevent(kq, &kevent_, 1, 0, 0, NULL) == -1)
        return false;
    } else {
      EV_SET(&kevent_, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
      kevent(kq, &kevent_, 1, 0, 0, NULL); // Ignore the result
    }

    return true;
  } else
    return dissociate(fd);
}

bool FDEventQueue::dissociate(fd_t fd) {
  bool ret = false;
  struct kevent kevent_;
  EV_SET(&kevent_, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  ret |= kevent(kq, &kevent_, 1, 0, 0, NULL) != -1;
  EV_SET(&kevent_, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  ret |= kevent(kq, &kevent_, 1, 0, 0, NULL) != -1;
  return ret;
}

bool FDEventQueue::enqueue(Event& event) {
  if (event_queue.enqueue(event)) {
    ssize_t write_ret = write(wake_pipe[1], "m", 1);
    debug_assert_eq(write_ret, 1);
    return true;
  } else
    return false;
}

YO_NEW_REF Event* FDEventQueue::timeddequeue(const Time& timeout) {
  Event* event = event_queue.trydequeue();
  if (event != NULL)
    return event;
  else {
    struct kevent kevent_;
    timespec timeout_ts = timeout;
    int ret = kevent(kq, 0, 0, &kevent_, 1, &timeout_ts);
    if (ret > 0) {
      debug_assert_eq(ret, 1);
      if (static_cast<int>(kevent_.ident) == wake_pipe[0]) {
        char m;
        ssize_t read_ret = read(wake_pipe[0], &m, sizeof(m));
        debug_assert_eq(read_ret, static_cast<ssize_t>(sizeof(m)));
        return event_queue.trydequeue();
      } else {
        switch (kevent_.filter) {
          case EVFILT_READ:
            return new FDEvent(kevent_.ident, FDEvent::TYPE_READ_READY);

          case EVFILT_WRITE:
            return new FDEvent(kevent_.ident, FDEvent::TYPE_WRITE_READY);

          default: debug_break(); return NULL;
        }
      }
    } else if (ret == 0 || errno == EINTR)
      return NULL;
    else {
      debug_break();
      return NULL;
    }
  }
}
}
}
