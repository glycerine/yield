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
#include "yield/exception.hpp"
#include "yield/poll/fd_event.hpp"
#include "yield/poll/linux/fd_event_queue.hpp"

#include <iostream>
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

namespace yield {
namespace poll {
namespace linux {
FDEventQueue::FDEventQueue() {
  uint32_t error_code;
  epfd = epoll_create(32768);
  if (epfd != -1) {
    wake_fd = eventfd(0, 0);
    if (wake_fd != -1) {
      if (associate(wake_fd, POLLIN))
        return;
      else
        error_code = static_cast<uint32_t>(errno);
      close(wake_fd);
    } else
      error_code = static_cast<uint32_t>(errno);
    close(epfd);
  } else
    error_code = static_cast<uint32_t>(errno);

  throw Exception(error_code);
}

FDEventQueue::~FDEventQueue() {
  close(epfd);
  close(wake_fd);
}

bool FDEventQueue::associate(fd_t fd, uint16_t fd_event_types) {
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
  if (BlockingConcurrentQueue<Event>::enqueue(event)) {
    uint64_t data = 1;
    write(wake_fd, &data, sizeof(data));
    return true;
  } else
    return false;
}

YO_NEW_REF Event* FDEventQueue::timeddequeue(const Time& timeout) {
  Event* event = BlockingConcurrentQueue<Event>::trydequeue();
  if (event != NULL)
    return event;
  else {
    epoll_event epoll_event_;
    int ret = epoll_wait(epfd, &epoll_event_, 1, static_cast<int>(timeout.ms()));

    if (ret > 0) {
      debug_assert_eq(ret, 1);

      if (epoll_event_.data.fd == wake_fd) {
        uint64_t data;
        read(wake_fd, &data, sizeof(data));
        return BlockingConcurrentQueue<Event>::trydequeue();
      } else
        return new FDEvent(epoll_event_.data.fd, epoll_event_.events);
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
