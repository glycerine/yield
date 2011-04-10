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

#include "fs_event_queue.hpp"
#include "yield/exception.hpp"
#include "yield/fs/posix/directory.hpp"
#include "yield/fs/posix/file_system.hpp"
#include "yield/poll/fd_event.hpp"

#include <errno.h>
#include <sys/inotify.h>

namespace yield {
namespace fs {
namespace poll {
namespace linux {
using std::map;
using yield::fs::posix::Directory;
using yield::fs::posix::FileSystem;
using yield::poll::FDEvent;
using yield::thread::NonBlockingConcurrentQueue;

class FSEventQueue::Watch : private vector<Watch*> {
public:
  void close() {
    if (wd != -1) {
      inotify_rm_watch(inotify_fd, wd);
      wd = -1;
    }
  }

  static Watch*
  create(
    FSEvent::Type events,
    int inotify_fd,
    Watch* parent,
    const Path& path,
    bool recursive
  ) {
    uint32_t mask = 0;

    if (
      events & FSEvent::TYPE_DIRECTORY_MODIFY
      ||
      events & FSEvent::TYPE_FILE_MODIFY
    )
      mask |= IN_ATTRIB | IN_MODIFY;

    if (
      events & FSEvent::TYPE_DIRECTORY_REMOVE
      ||
      events & FSEvent::TYPE_FILE_REMOVE
    )
      mask |= IN_DELETE_SELF;

    if (
      events & FSEvent::TYPE_DIRECTORY_RENAME
      ||
      events & FSEvent::TYPE_FILE_RENAME
    )
      mask |= IN_MOVE_SELF;

    if (
      events & FSEvent::TYPE_DIRECTORY_ADD
      ||
      events & FSEvent::TYPE_FILE_ADD
    )
      mask |= IN_CREATE;

    if (
      events & FSEvent::TYPE_DIRECTORY_REMOVE
      ||
      events & FSEvent::TYPE_FILE_REMOVE
    )
      mask |= IN_DELETE;

    if (
      events & FSEvent::TYPE_DIRECTORY_RENAME
      ||
      events & FSEvent::TYPE_FILE_RENAME
    )
      mask |= IN_MOVED_FROM | IN_MOVED_TO;

    int inotify_wd = inotify_add_watch(inotify_fd, path.c_str(), mask);
    if (inotify_wd != -1) {
      Directory* dir = FileSystem().opendir(path);

      Watch* watch
      = new Watch(
        events,
        inotify_fd,
        path.split().second,
        parent,
        path,
        recursive,
        inotify_wd
      );

      if (dir != NULL) {
        if (recursive) {
          Directory::Entry* dirent = dir->read();
          if (dirent != NULL) {
            do {
              if (dirent->ISDIR() && !dirent->is_hidden() && !dirent->is_special()) {
                Path child_path = path / dirent->get_name();
                Watch* child_watch
                = create(events, inotify_fd, watch, child_path, recursive);
                if (child_watch != NULL)
                  watch->push_back(child_watch);
              }
            } while (dir->read(*dirent));

            Directory::Entry::dec_ref(*dirent);
          }
        }

        Directory::dec_ref(*dir);
      }

      return watch;
    } else
      return NULL;
  }

  Watch* find_watch(int wd) {
    if (this->wd == wd)
      return this;
    else {
      for (iterator watch_i = begin(); watch_i != end(); ++watch_i) {
        Watch* watch = (*watch_i)->find_watch(wd);
        if (watch != NULL)
          return watch;
      }

      return NULL;
    }
  }

  operator int() const {
    return wd;
  }

  void read(NonBlockingConcurrentQueue<FSEvent, 1024>& fs_events) {
    uint32_t mask, cookie;
    Path name;
    if (last_read_mask == 0) {
      uint32_t mask_cookie_len[3];
      ssize_t read_ret
        = ::read(inotify_fd, mask_cookie_len, sizeof(mask_cookie_len));
      debug_assert_eq(read_ret, static_cast<ssize_t>(sizeof(mask_cookie_len)));

      mask = mask_cookie_len[0];
      cookie = mask_cookie_len[1];
      uint32_t len = mask_cookie_len[2];

      if (len > 0) {
        char* read_name = new char[len];
        read_ret = ::read(inotify_fd, read_name, len);
        debug_assert_eq(read_ret, static_cast<ssize_t>(len));
        name = Path(read_name, len);
        delete [] read_name;
      } else
        name = this->name;
    } else {
      mask = last_read_mask;
      cookie = last_read_cookie;
      name = last_read_name;
    }

    bool isdir = (mask & IN_ISDIR) == IN_ISDIR;
    Path path = path / name;

    for (;;) { // Mask can have multiple bits set.
      // Keep looping until we find one that matches events.
      FSEvent::Type type;

      if (mask & IN_ATTRIB) {
        if (isdir)
          type = FSEvent::TYPE_DIRECTORY_MODIFY;
        else
          type = FSEvent::TYPE_FILE_MODIFY;

        mask ^= IN_ATTRIB;
      } else if (mask & IN_CREATE) {
        if (isdir) {
          type = FSEvent::TYPE_DIRECTORY_ADD;

          if (recursive) {
            Watch* child_watch
            = create(events, inotify_fd, this, path, recursive);

            if (child_watch != NULL)
              push_back(child_watch);
          }
        } else
          type = FSEvent::TYPE_FILE_ADD;

        mask ^= IN_CREATE;
      } else if (mask & IN_DELETE) {
        if (isdir) {
          type = FSEvent::TYPE_DIRECTORY_REMOVE;

          Watch* child_watch = find_child_watch(name);
          if (child_watch != NULL) {
            child_watch->close();
            delete child_watch;
          }
        } else
          type = FSEvent::TYPE_FILE_REMOVE;

        mask ^= IN_DELETE;
      } else if (mask & IN_MOVE_SELF)
        DebugBreak();
      else if ((mask & IN_MOVED_FROM) == IN_MOVED_FROM) {
        if (isdir) {
          Watch* child_watch = find_child_watch(name);
          if (child_watch != NULL) {
            child_watch->close();
            delete child_watch;
          } else
            DebugBreak();
        }

        old_names[cookie] = name;

        mask ^= IN_MOVED_FROM;
      } else if (mask & IN_MOVED_TO) {
        map<uint32_t, Path>::iterator old_name_i = old_names.find(cookie);
        debug_assert_ne(old_name_i, old_names.end());

        if (isdir) {
          type = FSEvent::TYPE_DIRECTORY_RENAME;

          if (recursive) {
            Watch* child_watch
            = create(events, inotify_fd, this, path, recursive);

            if (child_watch != NULL)
              push_back(child_watch);
          }
        } else
          type = FSEvent::TYPE_FILE_RENAME;

        if ((events & type) == type) {
          FSEvent* fs_event
            = new FSEvent(path / old_name_i->second, path, type);
          fs_events.enqueue(*fs_event);
          old_names.erase(old_name_i);
          //return true;
          return;
        } else {
          old_names.erase(old_name_i);
          continue;
        }

        mask ^= IN_MOVED_TO;
      } else {
        last_read_cookie = 0;
        last_read_mask = 0;
        //return false;
        return;
      }

      if ((events & type) == type) {
        FSEvent* fs_event = new FSEvent(path, type);
        fs_events.enqueue(*fs_event);
        last_read_cookie = cookie;
        last_read_mask = mask;
        last_read_name = name;
        //return true;
        return;
      } else
        continue;
    }
  }

private:
  Watch(
    FSEvent::Type events,
    int inotify_fd,
    const Path& name,
    Watch* parent,
    const Path& path,
    bool recursive,
    int wd
  )
    : events(events),
      inotify_fd(inotify_fd),
      name(name),
      path(path),
      parent(parent),
      recursive(recursive),
      wd(wd)
  { }

  Watch* find_child_watch(const Path& name) {
    for (iterator watch_i = begin(); watch_i != end(); ++watch_i) {
      if ((*watch_i)->name == name)
        return *watch_i;
    }

    return NULL;
  }

private:
  FSEvent::Type events;
  int inotify_fd;
  uint32_t last_read_cookie, last_read_mask;
  Path last_read_name;
  Path name;
  map<uint32_t, Path> old_names;
  Path path;
  Watch* parent;
  bool recursive;
  int wd;
};


FSEventQueue::FSEventQueue() {
  inotify_fd = inotify_init();
  if (inotify_fd != -1) {
    if (!FDEventQueue::associate(inotify_fd, POLLIN)) {
      close(inotify_fd);
      throw Exception();
    }
  } else
    throw Exception();
}

FSEventQueue::~FSEventQueue() {
  close(inotify_fd);

  for (
    map<Path, Watch*>::iterator watch_i = watches.begin();
    watch_i != watches.end();
    ++watch_i
  )
    // Don't need to close the Watch here, since the inotify_fd close does that
    delete watch_i->second;
}

bool
FSEventQueue::associate(
  const Path& path,
  FSEvent::Type events,
  bool recursive
) {
  map<Path, Watch*>::iterator watch_i = watches.find(path);
  if (watch_i == watches.end()) {
    Watch* watch = Watch::create(events, inotify_fd, NULL, path, recursive);
    if (watch != NULL) {
      if (FDEventQueue::associate(*watch, POLLIN)) {
        watches[path] = watch;
        return true;
      } else
        delete watch;
    }
  } else
    DebugBreak(); // Modify the existing watch

  return false;
}

YO_NEW_REF Event* FSEventQueue::dequeue(const Time& timeout) {
  FSEvent* fs_event = fs_events.trydequeue();
  if (fs_event != NULL)
    return fs_event;
  else {
    Event* event = FDEventQueue::dequeue(timeout);
    if (event != NULL) {
      if (event->get_type_id() == FDEvent::TYPE_ID) {
        FDEvent* fd_event = static_cast<FDEvent*>(event);
        if (fd_event->get_fd() == inotify_fd) {
          int wd;
          ssize_t read_ret = ::read(inotify_fd, &wd, sizeof(wd));
          debug_assert_eq(read_ret, static_cast<ssize_t>(sizeof(wd)));

          for (
            map<Path, Watch*>::iterator watch_i = watches.begin();
            watch_i != watches.end();
            ++watch_i
          ) {
            Watch* watch = watch_i->second->find_watch(wd);
            if (watch != NULL) {
              watch->read(fs_events);
              return fs_events.trydequeue();
            }
          }
        } else
          return fd_event;
      } else
        return event;
    }
  }

  return NULL;
}

bool FSEventQueue::dissociate(const Path& path) {
  map<Path, Watch*>::iterator watch_i = watches.find(path);
  if (watch_i != watches.end()) {
    Watch* watch = watch_i->second;
    watch->close();
    delete watch;
    watches.erase(watch_i);
    return true;
  } else
    return false;
}
}
}
}
}

