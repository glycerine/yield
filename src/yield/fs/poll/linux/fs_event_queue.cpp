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
      Directory* directory = FileSystem().opendir(path);

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

      if (directory != NULL) {
        if (recursive) {
          Directory::Entry* dirent = directory->read();
          if (dirent != NULL) {
            do {
              if (dirent->ISDIR() && !dirent->is_hidden() && !dirent->is_special()) {
                Path child_path = path / dirent->get_name();
                Watch* child_watch
                  = create(events, inotify_fd, watch, child_path, recursive);
                if (child_watch != NULL)
                  watch->push_back(child_watch);
              }
            } while (directory->read(*dirent));

            Directory::Entry::dec_ref(*dirent);
          }
        }

        Directory::dec_ref(*directory);
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
        debug_assert_ne(*watch_i, NULL);
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

  void
  read(
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

    for (;;) { // Mask can have multiple bits set.
      // Keep looping until we find one that matches events.
      FSEvent::Type type;

      if ((mask & IN_ATTRIB) == IN_ATTRIB) {
        mask ^= IN_ATTRIB;

        if (isdir)
          type = FSEvent::TYPE_DIRECTORY_MODIFY;
        else
          type = FSEvent::TYPE_FILE_MODIFY;
      } else if ((mask & IN_CREATE) == IN_CREATE) {
        mask ^= IN_CREATE;

        if (isdir) {
          if (recursive) {
            Watch* child_watch
              = create(events, inotify_fd, this, path, recursive);
            if (child_watch != NULL)
              push_back(child_watch);
          }

          type = FSEvent::TYPE_DIRECTORY_ADD;
        } else
          type = FSEvent::TYPE_FILE_ADD;
      } else if (
          (mask & IN_DELETE) == IN_DELETE
          ||
          (mask & IN_MOVED_FROM) == IN_MOVED_FROM
      ) {
        if (isdir) {
          bool found_child_watch = false;
          for (iterator watch_i = begin(); watch_i != end(); ++watch_i) {
            if ((*watch_i)->name == name) {
              found_child_watch = true;
              (*watch_i)->close();
              delete *watch_i;
              erase(watch_i);

              if ((mask & IN_DELETE) == IN_DELETE) {
                mask ^= IN_DELETE;
                type = FSEvent::TYPE_DIRECTORY_REMOVE;
              } else {
                mask ^= IN_MOVED_FROM;
                old_names[cookie] = name;
                type = 0;
              }

              break;
            }
          }

          debug_assert(found_child_watch);
        } else {
          if ((mask & IN_DELETE) == IN_DELETE) {
            mask ^= IN_DELETE;
            type = FSEvent::TYPE_FILE_REMOVE;
          } else {
            mask ^= IN_MOVED_FROM;
            old_names[cookie] = name;
            type = 0;
          }
        }
      } else if ((mask & IN_MOVED_TO) == IN_MOVED_TO) {
        mask ^= IN_MOVED_TO;

        map<uint32_t, Path>::iterator old_name_i = old_names.find(cookie);
        debug_assert_ne(old_name_i, old_names.end());

        if (isdir) {
          if (recursive) {
            Watch* child_watch
              = create(events, inotify_fd, this, path, recursive);
            if (child_watch != NULL)
              push_back(child_watch);
          }

          type = FSEvent::TYPE_DIRECTORY_RENAME;
        } else
          type = FSEvent::TYPE_FILE_RENAME;

        if ((events & type) == type) {
          FSEvent* fs_event
            = new FSEvent(this->path / old_name_i->second, path, type);
          old_names.erase(old_name_i);
          fs_event_handler.handle(*fs_event);
        } else {
          old_names.erase(old_name_i);
          continue;
        }
      } else if ((mask & IN_MOVE_SELF) == IN_MOVE_SELF)
        DebugBreak();
      else
        return;

      if (type != 0 && (events & type) == type)
        fs_event_handler.handle(*new FSEvent(path, type));
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

private:
  FSEvent::Type events;
  int inotify_fd;
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
    if (!fd_event_queue.associate(inotify_fd, POLLIN)) {
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
      watches[path] = watch;
      return true;
    }
  } else
    DebugBreak(); // Modify the existing watch

  return false;
}

YO_NEW_REF Event* FSEventQueue::dequeue(const Time& timeout) {
  Event* event = fd_event_queue.dequeue(timeout);
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

            for (
              map<Path, Watch*>::iterator watch_i = watches.begin();
              watch_i != watches.end();
              ++watch_i
            ) {
              Watch* watch = watch_i->second->find_watch(inotify_event_->wd);
              if (watch != NULL)
                watch->read(*inotify_event_, *this);
            }

            inotify_events_p += sizeof(inotify_event) + inotify_event_->len;
          } while (inotify_events_p < inotify_events_pe);

          return fd_event_queue.trydequeue();
        }
      } else
        return fd_event;
    } else
      return event;
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

bool FSEventQueue::enqueue(YO_NEW_REF Event& event) {
  return fd_event_queue.enqueue(event);
}
}
}
}
}

