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
#include "yield/fs/poll/linux/fs_event_queue.hpp"
#include "yield/poll/fd_event.hpp"

#include <limits.h>
#include <sys/inotify.h>

namespace yield {
namespace fs {
namespace poll {
namespace linux {
using yield::poll::FDEvent;

FSEventQueue::FSEventQueue(YO_NEW_REF Log* log) : log(log) {
  inotify_fd = inotify_init();
  if (inotify_fd != -1) {
    if (fd_event_queue.associate(inotify_fd, FDEvent::TYPE_READ_READY)) {
      watches = new Watches;
    } else {
      close(inotify_fd);
      throw Exception();
    }
  } else
    throw Exception();
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
  Watch* watch = watches->find(path);
  if (watch != NULL) {
    if (watch->get_fs_event_types() == fs_event_types)
      return true;
    else {
      watch = watches->erase(path);
      debug_assert_ne(watch, NULL);
      delete watch;
    }
  }

  uint32_t mask = 0;
  if (fs_event_types & FSEvent::TYPE_DIRECTORY_ADD)
    mask |= IN_CREATE;
  if (fs_event_types & FSEvent::TYPE_DIRECTORY_MODIFY)
    mask |= IN_ATTRIB | IN_MODIFY;
  if (fs_event_types & FSEvent::TYPE_DIRECTORY_REMOVE)
    mask |= IN_DELETE | IN_DELETE_SELF;
  if (fs_event_types & FSEvent::TYPE_DIRECTORY_RENAME)
    mask |= IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO;
  if (fs_event_types & FSEvent::TYPE_FILE_ADD)
    mask |= IN_CREATE;
  if (fs_event_types & FSEvent::TYPE_FILE_MODIFY)
    mask |= IN_ATTRIB | IN_MODIFY;
  if (fs_event_types & FSEvent::TYPE_FILE_REMOVE)
    mask |= IN_DELETE | IN_DELETE_SELF;
  if (fs_event_types & FSEvent::TYPE_FILE_RENAME)
    mask |= IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO;

  int wd = inotify_add_watch(inotify_fd, path.c_str(), mask);
  if (wd != -1) {
    watch = new Watch(fs_event_types, inotify_fd, path, wd, log);
    watches->insert(*watch);
    return true;
  } else
    return false;
}

bool FSEventQueue::dissociate(const Path& path) {
  Watch* watch = watches->erase(path);
  if (watch != NULL) {
    delete watch;
    return true;
  } else
    return false;
}

bool FSEventQueue::enqueue(YO_NEW_REF Event& event) {
  return fd_event_queue.enqueue(event);
}

YO_NEW_REF Event* FSEventQueue::timeddequeue(const Time& timeout) {
  Event* event = fd_event_queue.timeddequeue(timeout);
  if (event != NULL) {
    if (event->get_type_id() == FDEvent::TYPE_ID) {
      FDEvent* fd_event = static_cast<FDEvent*>(event);
      if (fd_event->get_fd() == inotify_fd) {
        char inotify_events[(sizeof(inotify_event) + PATH_MAX) * 16];
        ssize_t read_ret
          = ::read(inotify_fd, inotify_events, sizeof(inotify_events));

        if (read_ret > 0) {
          const char* inotify_events_p = inotify_events;
          const char* inotify_events_pe
            = inotify_events + static_cast<size_t>(read_ret);

          do {
            const inotify_event* inotify_event_
              = reinterpret_cast<const inotify_event*>(inotify_events_p);

            Watch* watch = watches->find(inotify_event_->wd);
            if (watch != NULL)
              watch->read(*inotify_event_, *this);

            inotify_events_p += sizeof(inotify_event) + inotify_event_->len;
          } while (inotify_events_p < inotify_events_pe);

          return fd_event_queue.timeddequeue(0);
        }
      } else
        return fd_event;
    } else
      return event;
  }

  return NULL;
}
}
}
}
}

