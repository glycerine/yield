// yield/sockets/poll/win32/socket_event_queue.cpp

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
#include "yield/sockets/socket_pair.hpp"
#include "yield/sockets/poll/socket_event.hpp"
#include "yield/sockets/poll/win32/socket_event_queue.hpp"
#include "yield/thread/blocking_concurrent_queue.hpp"
#include "../../win32/winsock.hpp"

namespace yield {
namespace sockets {
namespace poll {
namespace win32 {
using yield::thread::BlockingConcurrentQueue;

class SocketEventQueue::SocketPollerSelector
  : public EventQueue,
    protected BlockingConcurrentQueue<Event> {
public:
  virtual bool associate(socket_t socket_, SocketEvent::Type socket_event_types) = 0;
  virtual bool dissociate(socket_t socket_) = 0;

public:
  // yield::EventQuueue
  virtual YO_NEW_REF Event* dequeue(const Time& timeout) = 0;

  bool enqueue(YO_NEW_REF Event& event) {
    if (BlockingConcurrentQueue<Event>::enqueue(event)) {
      wake_socket_pair.second().write("m", 1);
      return true;
    } else
      return false;      
  }

protected:
  SocketPair wake_socket_pair;
};

#if _WIN32_WINNT >= 0x0600
class SocketEventQueue::SocketPoller : public SocketPollerSelector {
public:
  SocketPoller() {
    if (!associate(wake_socket_pair.first(), SocketEvent::TYPE_READ_READY))
      throw Exception();    
  }

public:
  // SocketPollerSelector
  bool associate(socket_t socket_, uint16_t socket_event_types) {
    if (socket_event_types != 0) {
      for (
        vector<pollfd>::iterator pollfd_i = pollfds.begin();
        pollfd_i != pollfds.end();
        ++pollfd_i
      ) {
        if ((*pollfd_i).fd == socket_) {
          (*pollfd_i).events = socket_event_types;
          return true;
        }
      }

      pollfd pollfd_;
      memset(&pollfd_, 0, sizeof(pollfd_));
      pollfd_.fd = socket_;
      pollfd_.events = socket_event_types;
      pollfds.push_back(pollfd_);
      return true;
    } else
      return dissociate(socket_);
  }

  bool dissociate(socket_t socket_) {
    for (
      vector<pollfd>::iterator pollfd_i = pollfds.begin();
      pollfd_i != pollfds.end();
      ++pollfd_i
    ) {
      if ((*pollfd_i).fd == socket_) {
        pollfds.erase(pollfd_i);
        return true;
      }
    }

    return false;
  }

public:
  // yield::EventQueue
  YO_NEW_REF Event* dequeue(const Time& timeout) {
    // Scan pollfds once for outstanding revents from the last WSAPoll,
    // then call WSAPoll again if necessary.
    int ret = 0;
    do {
      vector<pollfd>::iterator pollfd_i = pollfds.begin();

      do {
        pollfd& pollfd_ = *pollfd_i;

        if (pollfd_.revents != 0) {
          if (pollfd_.fd == wake_socket_pair.first()) {
            pollfd_.revents = 0;
            char m;
            wake_socket_pair.first().read(&m, 1);
            return BlockingConcurrentQueue<Event>::trydequeue();
          } else {
            uint16_t revents = pollfd_.revents;
            pollfd_.revents = 0;
            if (revents == POLLRDNORM)
              revents = POLLIN;
            return new SocketEvent(pollfd_.fd, revents);
          }
        }
      } while (++pollfd_i < pollfds.end());

      ret
      = WSAPoll(
          &pollfds[0],
          pollfds.size(),
          static_cast<int>(timeout.ms())
        );
    } while (ret > 0);

    return NULL;
  }

private:
  vector<pollfd> pollfds;
};
#endif

class SocketEventQueue::SocketSelector
  : public SocketPollerSelector,
    private vector<socket_t> {
public:
  SocketSelector() {
    FD_ZERO(&except_fd_set);
    FD_ZERO(&read_fd_set);
    FD_ZERO(&write_fd_set);

    if (!associate(wake_socket_pair.first(), POLLIN))
      throw Exception();
  }

public:
  bool associate(socket_t socket_, uint16_t socket_event_types) {
    if (socket_event_types != 0) {
      for (iterator socket_i = begin(); socket_i != end(); ++socket_i) {
        if (*socket_i == socket_) {
          if (socket_event_types & POLLERR)
            FD_SET(socket_, &except_fd_set);
          else
            FD_CLR(socket_, &except_fd_set);

          if (socket_event_types & POLLIN)
            FD_SET(socket_, &read_fd_set);
          else
            FD_CLR(socket_, &read_fd_set);

          if (socket_event_types & POLLOUT)
            FD_SET(socket_, &write_fd_set);
          else
            FD_CLR(socket_, &write_fd_set);

          return true;
        }
      }

      push_back(socket_);

      if (socket_event_types & POLLERR)
        FD_SET(socket_, &except_fd_set);
      if (socket_event_types & POLLIN)
        FD_SET(socket_, &read_fd_set);
      if (socket_event_types & POLLOUT)
        FD_SET(socket_, &write_fd_set);

      return true;
    } else
      return dissociate(socket_);
  }

  bool dissociate(socket_t socket_) {
    for (
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

public:
  // yield::EventQueue
  YO_NEW_REF Event* dequeue(const Time& timeout) {
    fd_set except_fd_set_copy, read_fd_set_copy, write_fd_set_copy;

    memcpy_s(
      &except_fd_set_copy,
      sizeof(except_fd_set_copy),
      &except_fd_set,
      sizeof(except_fd_set)
    );

    memcpy_s(
      &read_fd_set_copy,
      sizeof(read_fd_set_copy),
      &read_fd_set,
      sizeof(read_fd_set)
    );

    memcpy_s(
      &write_fd_set_copy,
      sizeof(write_fd_set_copy),
      &write_fd_set,
      sizeof(write_fd_set)
    );

    timeval timeout_tv;
    timeout_tv.tv_sec = static_cast<long>(timeout.ns() / Time::NS_IN_S);
    timeout_tv.tv_usec = (timeout.ns() % Time::NS_IN_S) / Time::NS_IN_US;

    int ret
    = select(
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

        uint16_t socket_event_types = 0;

        if (FD_ISSET(socket_, &except_fd_set_copy)) {
          socket_event_types |= POLLERR;
          ret--; // one for every socket event,
          // not every socket
        }

        if (
          ret > 0
          &&
          FD_ISSET(socket_, &read_fd_set_copy)
        ) {
          socket_event_types |= POLLIN;
          ret--;
        }

        if (
          ret > 0
          &&
          FD_ISSET(socket_, &write_fd_set_copy)
        ) {
          socket_event_types |= POLLOUT;
          ret--;
        }

        if (socket_event_types != 0) {
          if (socket_ == wake_socket_pair.first()) {
            char m;
            wake_socket_pair.second().read(&m, 1);
            return BlockingConcurrentQueue<Event>::trydequeue();
          }
          else
            return new SocketEvent(socket_, socket_event_types);
        }

        ++socket_i;
      }
    }

    return NULL;
  }

private:
  fd_set except_fd_set, read_fd_set, write_fd_set;
};


SocketEventQueue::SocketEventQueue() {
#if _WIN32_WINNT >= 0x0600
  pimpl = new SocketPoller;
#else
  pimpl = new SocketSelector;
#endif
}

SocketEventQueue::~SocketEventQueue() {
  delete pimpl;
}

bool SocketEventQueue::associate(socket_t socket_, uint16_t socket_event_types) {
  return pimpl->associate(socket_, socket_event_types);
}

YO_NEW_REF Event* SocketEventQueue::dequeue(const Time& timeout) {
  return pimpl->dequeue(timeout);
}

bool SocketEventQueue::dissociate(socket_t socket_) {
  return pimpl->dissociate(socket_);
}

bool SocketEventQueue::enqueue(YO_NEW_REF Event& event) {
  return pimpl->enqueue(event);
}
}
}
}
}
