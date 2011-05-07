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

#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/log.hpp"
#include "yield/fs/directory.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/poll/linux/fs_event_queue.hpp"
#include "yield/poll/fd_event.hpp"

#include <errno.h>
#include <map>
#include <sys/inotify.h>

namespace yield {
namespace fs {
namespace poll {
namespace linux {
using std::make_pair;
using std::map;
using yield::fs::Directory;
using yield::fs::FileSystem;
using yield::poll::FDEvent;

class FSEventQueue::Watch {
public:
  Watch(
    FSEvent::Type fs_event_types,
    int inotify_fd,
    const Path& path,
    int wd,
    Log* log = NULL
  ) : fs_event_types(fs_event_types),
    log(Object::inc_ref(log)),
    path(path),
    wd(wd) {
  }

  ~Watch() {
    inotify_rm_watch(inotify_fd, wd);
    Log::dec_ref(log);
  }

public:
  FSEvent::Type get_fs_event_types() const {
    return fs_event_types;
  }

  const Path& get_path() const {
    return path;
  }

  int get_wd() const {
    return wd;
  }

public:
  void read(const inotify_event&, EventHandler& fs_event_handler);

public:
  const char* get_type_name() const {
    return "yield::fs::poll::linux::FSEventQueue::Watch";
  }

private:
  FSEvent::Type fs_event_types;
  int inotify_fd;
  Log* log;
  map<uint32_t, Path> old_names;
  Path path;
  int wd;
};


class FSEventQueue::Watches : private map<int, Watch*> {
public:
  ~Watches() {
    for (iterator watch_i = begin(); watch_i != end(); ++watch_i)
      delete watch_i->second;
  }

public:
  Watch* find(const Path& path) {
    for (iterator watch_i = begin(); watch_i != end(); ++watch_i) {
      if (watch_i->second->get_path() == path)
        return watch_i->second;
    }

    return NULL;
  }

  Watch* find(int wd) {
    iterator watch_i = map<int, Watch*>::find(wd);
    if (watch_i != end())
      return watch_i->second;
    else
      return NULL;
  }

  Watch* erase(const Path& path) {
    for (iterator watch_i = begin(); watch_i != end(); ++watch_i) {
      if (watch_i->second->get_path() == path) {
        Watch* watch = watch_i->second;
        map<int, Watch*>::erase(watch_i);
        return watch;
      }
    }

    return NULL;
  }

  void insert(Watch& watch) {
    iterator watch_i = map<int, Watch*>::find(watch.get_wd());
    debug_assert_eq(watch_i, end());
    map<int, Watch*>::insert(make_pair(watch.get_wd(), &watch));
  }
};


void
FSEventQueue::Watch::read(
  const inotify_event& inotify_event_,
  EventHandler& fs_event_handler
) {
  uint32_t cookie = inotify_event_.cookie;
  uint32_t mask = inotify_event_.mask;
  bool isdir = (mask & IN_ISDIR) == IN_ISDIR;
  Path name, path;
  if (inotify_event_.len > 1) {
    // len includes a NULL terminator, but may also include
    // one or more bytes of padding. Do a strlen to find the real length.
    name = Path(inotify_event_.name); //, inotify_event_.len - 1);
    path = this->path / name;
  }

  if (log != NULL) {
    log->get_stream(Log::Level::DEBUG) <<
      get_type_name() << "(path=" << get_path() << ", wd=" << get_wd() << ")"
      << ": read inotify_event(" <<
        "cookie=" << inotify_event_.cookie <<
        ", "
        "mask=" << inotify_event_.mask <<
        ", "
        "name=" << name <<
      ")";
  }

  for (;;) {
    // Mask can have multiple bits set.
    // Keep looping until we find one that matches fs_event_types.
    FSEvent::Type fs_event_type;

    if ((mask & IN_ATTRIB) == IN_ATTRIB) {
      mask ^= IN_ATTRIB;

      if (isdir)
        fs_event_type = FSEvent::TYPE_DIRECTORY_MODIFY;
      else
        fs_event_type = FSEvent::TYPE_FILE_MODIFY;
    } else if ((mask & IN_CREATE) == IN_CREATE) {
      mask ^= IN_CREATE;

      if (isdir)
        fs_event_type = FSEvent::TYPE_DIRECTORY_ADD;
      else
        fs_event_type = FSEvent::TYPE_FILE_ADD;
    } else if ((mask & IN_DELETE) == IN_DELETE) {
      mask ^= IN_DELETE;
      if (isdir)
        fs_event_type = FSEvent::TYPE_DIRECTORY_REMOVE;
      else
        fs_event_type = FSEvent::TYPE_FILE_REMOVE;
    } else if ((mask & IN_MOVED_FROM) == IN_MOVED_FROM) {
      mask ^= IN_MOVED_FROM;
      old_names[cookie] = name;
      fs_event_type = 0;
    } else if ((mask & IN_MOVED_TO) == IN_MOVED_TO) {
      mask ^= IN_MOVED_TO;

      map<uint32_t, Path>::iterator old_name_i = old_names.find(cookie);
      debug_assert_ne(old_name_i, old_names.end());

      if (isdir)
        fs_event_type = FSEvent::TYPE_DIRECTORY_RENAME;
      else
        fs_event_type = FSEvent::TYPE_FILE_RENAME;

      if ((fs_event_types & fs_event_type) == fs_event_type) {
        FSEvent* fs_event
          = new FSEvent(this->path / old_name_i->second, path, fs_event_type);
        old_names.erase(old_name_i);

        if (log != NULL) {
          log->get_stream(Log::Level::DEBUG) <<
            get_type_name() <<
              "(path=" << get_path() << ", wd=" << get_wd() << ")" <<
              ": read " << *fs_event;
        }

        fs_event_handler.handle(*fs_event);
      } else
        old_names.erase(old_name_i);

      continue;
    } else if ((mask & IN_MOVE_SELF) == IN_MOVE_SELF)
      debug_break();
    else
      return;

    if (
      fs_event_type != 0
      &&
      (fs_event_types & fs_event_type) == fs_event_type
    ) {
      FSEvent* fs_event = new FSEvent(path, fs_event_type);

      if (log != NULL) {
        log->get_stream(Log::Level::DEBUG) <<
          get_type_name() <<
            "(path=" << get_path() << ", wd=" << get_wd() << ")" <<
            ": read " << *fs_event;
      }

      fs_event_handler.handle(*fs_event);
    }
  }
}

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

