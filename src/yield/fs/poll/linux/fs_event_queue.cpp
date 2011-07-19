// yield/fs/poll/linux/fs_event_queue.cpp

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

#include "watches.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/log.hpp"
#include "yield/fs/poll/fs_event_queue.hpp"

#include <errno.h>
#include <limits.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/inotify.h>

namespace yield {
namespace fs {
namespace poll {
using linux::Watches;

FSEventQueue::FSEventQueue(YO_NEW_REF Log* log) : log(log) {
  epoll_fd = epoll_create(32768);
  if (epoll_fd != -1) {
    try {
      event_fd = eventfd(0, 0);
      if (event_fd != -1) {
        try {
          epoll_event epoll_event_;
          memset(&epoll_event_, 0, sizeof(epoll_event_));
          epoll_event_.data.fd = event_fd;
          epoll_event_.events = EPOLLIN;
          if (
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &epoll_event_) == 0
          ) {
            inotify_fd = inotify_init();
            if (inotify_fd != -1) {
              try {
                memset(&epoll_event_, 0, sizeof(epoll_event_));
                epoll_event_.data.fd = inotify_fd;
                epoll_event_.events = EPOLLIN;
                if (
                  epoll_ctl(
                    epoll_fd,
                    EPOLL_CTL_ADD,
                    inotify_fd,
                    &epoll_event_
                  ) == 0
                ) {
                  watches = new linux::Watches;
                } else {
                  throw Exception();
                }
              } catch (Exception&) {
                close(inotify_fd);
                throw;
              }
            } else {
              throw Exception();
            }
          } else {
            throw Exception();
          }
        } catch (Exception&) {
          close(event_fd);
          throw;
        }
      } else {
        throw Exception();
      }
    } catch (Exception&) {
      close(epoll_fd);
      throw;
    }
  } else {
    throw Exception();
  }
}

FSEventQueue::~FSEventQueue() {
  close(inotify_fd);
  Log::dec_ref(log);
  delete watches;
}

bool
FSEventQueue::associate(
  const Path& path,
  FSEvent::Type fs_event_types
) {
  linux::Watch* watch = watches->find(path);
  if (watch != NULL) {
    if (watch->get_fs_event_types() == fs_event_types) {
      return true;
    } else {
      watch = watches->erase(path);
      debug_assert_ne(watch, NULL);
      delete watch;
    }
  }

  uint32_t mask = 0;
  if (fs_event_types & FSEvent::TYPE_DIRECTORY_ADD) {
    mask |= IN_CREATE;
  }
  if (fs_event_types & FSEvent::TYPE_DIRECTORY_MODIFY) {
    mask |= IN_ATTRIB | IN_MODIFY;
  }
  if (fs_event_types & FSEvent::TYPE_DIRECTORY_REMOVE) {
    mask |= IN_DELETE | IN_DELETE_SELF;
  }
  if (fs_event_types & FSEvent::TYPE_DIRECTORY_RENAME) {
    mask |= IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO;
  }
  if (fs_event_types & FSEvent::TYPE_FILE_ADD) {
    mask |= IN_CREATE;
  }
  if (fs_event_types & FSEvent::TYPE_FILE_MODIFY) {
    mask |= IN_ATTRIB | IN_MODIFY;
  }
  if (fs_event_types & FSEvent::TYPE_FILE_REMOVE) {
    mask |= IN_DELETE | IN_DELETE_SELF;
  }
  if (fs_event_types & FSEvent::TYPE_FILE_RENAME) {
    mask |= IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO;
  }

  int wd = inotify_add_watch(inotify_fd, path.c_str(), mask);
  if (wd != -1) {
    watch = new linux::Watch(fs_event_types, inotify_fd, path, wd, log);
    watches->insert(*watch);
    return true;
  } else {
    return false;
  }
}

bool FSEventQueue::dissociate(const Path& path) {
  linux::Watch* watch = watches->erase(path);
  if (watch != NULL) {
    delete watch;
    return true;
  } else {
    return false;
  }
}

bool FSEventQueue::enqueue(YO_NEW_REF Event& event) {
  if (event_queue.enqueue(event)) {
    uint64_t data = 1;
    ssize_t write_ret = write(event_fd, &data, sizeof(data));
    debug_assert_eq(write_ret, static_cast<ssize_t>(sizeof(data)));
    return true;
  } else {
    return false;
  }
}

YO_NEW_REF Event* FSEventQueue::timeddequeue(const Time& timeout) {
  Event* event = event_queue.trydequeue();
  if (event != NULL) {
    return event;
  } else {
    epoll_event epoll_event_;
    int timeout_ms
    = (timeout == Time::FOREVER) ? -1 : static_cast<int>(timeout.ms());
    int ret = epoll_wait(epoll_fd, &epoll_event_, 1, timeout_ms);

    if (ret > 0) {
      debug_assert_eq(ret, 1);

      if (epoll_event_.data.fd == event_fd) {
        uint64_t data;
        read(event_fd, &data, sizeof(data));
        return event_queue.trydequeue();
      } else {
        debug_assert_eq(epoll_event_.data.fd, inotify_fd);

        char inotify_events[(sizeof(inotify_event) + PATH_MAX) * 16];
        ssize_t read_ret
        = ::read(inotify_fd, inotify_events, sizeof(inotify_events));
        debug_assert_gt(read_ret, 0);

        const char* inotify_events_p = inotify_events;
        const char* inotify_events_pe
        = inotify_events + static_cast<size_t>(read_ret);

        do {
          const inotify_event* inotify_event_
          = reinterpret_cast<const inotify_event*>(inotify_events_p);

          linux::Watch* watch = watches->find(inotify_event_->wd);
          if (watch != NULL) {
            FSEvent* fs_event = watch->parse(*inotify_event_);
            if (fs_event != NULL) {
              event_queue.enqueue(*fs_event);
            }
          }

          inotify_events_p += sizeof(inotify_event) + inotify_event_->len;
        } while (inotify_events_p < inotify_events_pe);

        return event_queue.trydequeue();
      }
    } else {
      debug_assert_true(ret == 0 || errno == EINTR);
      return NULL;
    }
  }
}
}
}
}
