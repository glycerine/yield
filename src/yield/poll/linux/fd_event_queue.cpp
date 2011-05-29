// yield/poll/linux/fd_event_queue.cpp

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

#include <iostream>
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

namespace yield {
namespace poll {
FDEventQueue::FDEventQueue(bool) throw (Exception) {
  epfd = epoll_create(32768);
  if (epfd != -1) {
    try {
      wake_fd = eventfd(0, 0);
      if (wake_fd != -1) {
        try {
          if (!associate(wake_fd, POLLIN))
            throw Exception();
        } catch (Exception&) {
          close(wake_fd);
          throw;
        }
      } else
        throw Exception();
    } catch (Exception&) {
      close(epfd);
      throw;
    }
  } else
    throw Exception();
}

FDEventQueue::~FDEventQueue() {
  close(epfd);
  close(wake_fd);
}

bool FDEventQueue::associate(fd_t fd, FDEvent::Type fd_event_types) {
  if (fd_event_types > 0) {
    epoll_event epoll_event_;
    memset(&epoll_event_, 0, sizeof(epoll_event_));
    epoll_event_.data.fd = fd;
    epoll_event_.events = fd_event_types;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epoll_event_) == 0)
      return true;
    else if (errno == EEXIST)
      return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &epoll_event_) == 0;
    else
      return false;
  } else
    return dissociate(fd);
}

bool FDEventQueue::dissociate(fd_t fd) {
  // From the man page: In kernel versions before 2.6.9,
  // the EPOLL_CTL_DEL operation required a non-NULL pointer in event,
  // even though this argument is ignored. Since kernel 2.6.9,
  // event can be specified as NULL when using EPOLL_CTL_DEL.
  epoll_event epoll_event_;
  memset(&epoll_event_, 0, sizeof(epoll_event_));
  return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &epoll_event_) == 0;
}

bool FDEventQueue::enqueue(Event& event) {
  if (event_queue.enqueue(event)) {
    uint64_t data = 1;
    ssize_t write_ret = write(wake_fd, &data, sizeof(data));
    debug_assert_eq(write_ret, static_cast<ssize_t>(sizeof(data)));
    return true;
  } else
    return false;
}

YO_NEW_REF Event* FDEventQueue::timeddequeue(const Time& timeout) {
  Event* event = event_queue.trydequeue();
  if (event != NULL)
    return event;
  else {
    epoll_event epoll_event_;
    int timeout_ms
      = (timeout == Time::FOREVER) ? -1 : static_cast<int>(timeout.ms());
    int ret = epoll_wait(epfd, &epoll_event_, 1, timeout_ms);
    if (ret > 0) {
      debug_assert_eq(ret, 1);

      if (epoll_event_.data.fd == wake_fd) {
        uint64_t data;
        ssize_t read_ret = read(wake_fd, &data, sizeof(data));
        debug_assert_eq(read_ret, static_cast<ssize_t>(sizeof(data)));
        return event_queue.trydequeue();
      } else
        return new FDEvent(epoll_event_.data.fd, epoll_event_.events);
    } else {
      debug_assert_true(ret == 0 || errno == EINTR);
      return NULL;
    }
  }
}
}
}
