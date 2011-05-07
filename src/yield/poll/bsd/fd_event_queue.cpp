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
  kq = kqueue();
  if (kq != -1) {
    if (pipe(wake_pipe) != -1) {
      if (associate(wake_pipe[0], POLLIN))
        return;
      else {
        Exception exception;
        close(wake_pipe[0]);
        close(wake_pipe[1]);
        throw exception;
      }
    } else {
      Exception exception;
      close(kq);
      throw exception;
    }
  } else
    throw Exception();
}

FDEventQueue::~FDEventQueue() {
  close(kq);
  close(wake_pipe[0]);
  close(wake_pipe[1]);
}

bool FDEventQueue::associate(fd_t fd, uint16_t fd_event_types) {
  if (fd_event_types > 0) {
    kevent kevent_;

    if (fd_event_types & FDEvent::TYPE_READ_READY) {
      EV_SET(&kevent_, fd, EVFILT_READ, EV_ADD, 0, 0, context);
      if (kevent(kq, &kevent_, 1, 0, 0, NULL) == -1)
        return false;
    } else {
      EV_SET(&kevent_, fd, EVFILT_WRITE, EV_DELETE, 0, 0, context);
      kevent(kq, &kevent_, 1, 0, 0, NULL); // Ignore the result
    }

    if (fd_event_types & FDEvent::TYPEW_WRITE_READY) {
      EV_SET(&kevent_, fd, EVFILT_WRITE, EV_ADD, 0, 0, context);
      if (kevent(kq, &kevent_, 1, 0, 0, NULL) == -1)
        return false;
    } else {
      EV_SET(&kevent_, fd, EVFILT_WRITE, EV_DELETE, 0, 0, context);
      kevent(kq, &kevent_, 1, 0, 0, NULL); // Ignore the result
    }

    return true;
  } else
    return dissociate(fd);
}

bool FDEventQueue::dissociate(fd_t fd) {
  kevent kevents[2];
  EV_SET(&kevents[0], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  EV_SET(&kevents[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  return kevent(kq, kevents, 2, 0, 0, NULL) != -1;
}

bool FDEventQueue::enqueue(Event& event) {
  if (BlockingConcurrentQueue<Event>::enqueue(event)) {
    write(wake_pipe[1], "m", 1);
    return true;
  } else
    return false;
}

YO_NEW_REF Event* FDEventQueue::timeddequeue(const Time& timeout) {
  Event* event = BlockingConcurrentQueue<Event>::trydequeue();
  if (event != NULL)
    return event;
  else {
    kevent kevent_;
    timespec timeout_ts = timeout;
    int ret = kevent(kq, 0, 0, &kevent_, 1, &timeout_ts);
    if (ret > 0) {
      debug_assert_eq(ret, 1);
      if (kevent_.ident == wake_pipe[0]) {
        char m;
        read(wake_pipe[0], &m, sizeof(m));
        return BlockingConcurrentQueue<Event>::trydequeue();
      } else {
        switch (kevent_.filter) {
          case EVFILT_READ:
            return new FDEvent(kevent_.ident, FDEvent::TYPE_WRITE_READY);

          case EVFILT_WRITE:
            return new FDEvent(kevent_.ident, FDEvent::TYPE_READ_READY);

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
}
