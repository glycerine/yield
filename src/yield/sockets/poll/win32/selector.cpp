// yield/poll/win32/selector.cpp

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

#include "selector.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/poll/socket_event.hpp"


namespace yield {
namespace poll {
namespace win32 {
using yield::thread::NonBlockingConcurrentQueue;


Selector::Selector() {
  FD_ZERO(&except_fd_set);
  FD_ZERO(&read_fd_set);
  FD_ZERO(&write_fd_set);

  associate(wake_socket_pair.first(), POLLIN);
}

bool Selector::associate(socket_t socket_, int16_t events) {
  if (events > 0) {
    for (iterator socket_i = begin(); socket_i != end(); ++socket_i) {
      if (*socket_i == socket_) {
        if (events & POLLERR)
          FD_SET(socket_, &except_fd_set);
        else
          FD_CLR(socket_, &except_fd_set);

        if (events & POLLIN)
          FD_SET(socket_, &read_fd_set);
        else
          FD_CLR(socket_, &read_fd_set);

        if (events & POLLOUT)
          FD_SET(socket_, &write_fd_set);
        else
          FD_CLR(socket_, &write_fd_set);

        return true;
      }
    }

    push_back(socket_);

    if (events & POLLERR)
      FD_SET(socket_, &except_fd_set);
    if (events & POLLIN)
      FD_SET(socket_, &read_fd_set);
    if (events & POLLOUT)
      FD_SET(socket_, &write_fd_set);

    return true;
  } else
    return dissociate(socket_);
}

Event* Selector::dequeue(const Time& timeout) {
  fd_set except_fd_set_copy, read_fd_set_copy, write_fd_set_copy;

  memcpy_s
  (
    &except_fd_set_copy,
    sizeof(except_fd_set_copy),
    &except_fd_set,
    sizeof(except_fd_set)
  );

  memcpy_s
  (
    &read_fd_set_copy,
    sizeof(read_fd_set_copy),
    &read_fd_set,
    sizeof(read_fd_set)
  );

  memcpy_s
  (
    &write_fd_set_copy,
    sizeof(write_fd_set_copy),
    &write_fd_set,
    sizeof(write_fd_set)
  );

  timeval timeout_tv;
  timeout_tv.tv_sec = static_cast<long>(timeout.ns() / Time::NS_IN_S);
  timeout_tv.tv_usec = (timeout.ns() % Time::NS_IN_S) / Time::NS_IN_US;

  int ret
  = select
    (
      0,
      &read_fd_set_copy,
      &write_fd_set_copy,
      &except_fd_set_copy,
      &timeout_tv
    );

  if (ret > 0) {
    const_iterator socket_i = begin();

    while (ret > 0 && socket_i != end()) {
      socket_t socket_ = *socket_i;

      int16_t events = 0;

      if (FD_ISSET(socket_, &except_fd_set_copy)) {
        events |= POLLERR;
        ret--; // one for every socket event,
        // not every socket
      }

      if
      (
        ret > 0
        &&
        FD_ISSET(socket_, &read_fd_set_copy)
      ) {
        events |= POLLIN;
        ret--;
      }

      if
      (
        ret > 0
        &&
        FD_ISSET(socket_, &write_fd_set_copy)
      ) {
        events |= POLLOUT;
        ret--;
      }

      if (events != 0) {
        if (socket_ == wake_socket_pair.first())
          return NonBlockingConcurrentQueue<Event, 32>::trydequeue();
        else
          return new SocketEvent(events, socket_);
      }

      ++socket_i;
    }
  }

  return NULL;
}

bool Selector::dissociate(socket_t socket_) {
  for
  (
    iterator socket_i = begin();
    socket_i != end();
    ++socket_i
  ) {
    if (*socket_i == socket_) {
      erase(socket_i);
      FD_CLR(socket_, &except_fd_set);
      FD_CLR(socket_, &read_fd_set);
      FD_CLR(socket_, &write_fd_set);
      return true;
    }
  }

  return false;
}

bool Selector::enqueue(Event& event) {
  bool ret = NonBlockingConcurrentQueue<Event, 32>::enqueue(event);
  debug_assert(ret);
  wake_socket_pair.second().write("m", 1);
  return ret;
}
}
}
}
