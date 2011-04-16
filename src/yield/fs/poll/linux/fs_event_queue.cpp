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


class FSEventQueue::Watch {
public:
  ~Watch();

public:
  static Watch*
  create(
    FSEvent::Type fs_event_types,
    int inotify_fd,
    const Path& path,
    bool recursive,
    Watches& watches
  );

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

  bool is_recursive() const {
    return recursive;
  }

public:
  void
  read(
    const inotify_event& inotify_event_,
    EventHandler& fs_event_handler,
    Watches& watches
  );

private:
  Watch(
    FSEvent::Type fs_event_types,
    int inotify_fd,
    const Path& path,
    bool recursive,
    Watches& watches,
    int wd
  ) : fs_event_types(fs_event_types),
      inotify_fd(inotify_fd),
      path(path),
      recursive(recursive),
      watches(watches),
      wd(wd)
  { }

private:
  vector<Watch*> child_watches;
  FSEvent::Type fs_event_types;
  int inotify_fd;
  map<uint32_t, Path> old_names;
  Path path;
  bool recursive;
  Watches& watches;
  int wd;
};


class FSEventQueue::Watches {
public:
  ~Watches() {
    for (
      map<Path, Watch*>::iterator watch_i = watches_by_path.begin();
      watch_i != watches_by_path.end();
      ++watch_i
    )
      delete watch_i->second;
  }

public:
  Watch* find(const Path& path) {
    map<Path, Watch*>::iterator watch_i = watches_by_path.find(path);
    if (watch_i != watches_by_path.end())
      return watch_i->second;
    else
      return NULL;
  }

  Watch* find(int wd) {
    map<int, Watch*>::iterator watch_i = watches_by_wd.find(wd);
    if (watch_i != watches_by_wd.end())
      return watch_i->second;
    else
      return NULL;
  }

  Watch* erase(const Path& path) {
    map<Path, Watch*>::iterator watch_i = watches_by_path.find(path);
    if (watch_i != watches_by_path.end()) {
      Watch* watch = watch_i->second;
      watches_by_path.erase(watch_i);

      map<int, Watch*>::iterator watch_j = watches_by_wd.find(watch->get_wd());
      debug_assert_ne(watch_j, watches_by_wd.end());
      watches_by_wd.erase(watch_j);

      return watch;
    } else
      return NULL;
  }

  void insert(Watch& watch) {
    debug_assert_eq(
      watches_by_path.find(watch.get_path()),
      watches_by_path.end()
    );

    watches_by_path[watch.get_path()] = &watch;

    debug_assert_eq(
      watches_by_wd.find(watch.get_wd()),
      watches_by_wd.end()
    );

    watches_by_wd[watch.get_wd()] = &watch;
  }

public:
  map<Path, Watch*> watches_by_path;
  map<int, Watch*> watches_by_wd;
};


FSEventQueue::Watch::~Watch() {
  Watch* watch = watches.erase(wd);
  debug_assert_eq(watch, this);
  inotify_rm_watch(inotify_fd, wd);

  for (
    vector<Watch*>::child_watch_i = child_watches.begin();
    child_watch_i != child_watches.end();
    ++child_watch_i
  )
    delete *child_watch_i;
}

FSEventQueue::Watch*
FSEventQueue::Watch::create(
  FSEvent::Type fs_event_types,
  int inotify_fd,
  const Path& path,
  bool recursive,
  Watches& watches
) {
  uint32_t mask = 0;

  if (
    fs_event_types & FSEvent::TYPE_DIRECTORY_MODIFY
    ||
    fs_event_types & FSEvent::TYPE_FILE_MODIFY
  )
    mask |= IN_ATTRIB | IN_MODIFY;

  if (
    fs_event_types & FSEvent::TYPE_DIRECTORY_REMOVE
    ||
    fs_event_types & FSEvent::TYPE_FILE_REMOVE
  )
    mask |= IN_DELETE_SELF;

  if (
    fs_event_types & FSEvent::TYPE_DIRECTORY_RENAME
    ||
    fs_event_types & FSEvent::TYPE_FILE_RENAME
  )
    mask |= IN_MOVE_SELF;

  if (
    fs_event_types & FSEvent::TYPE_DIRECTORY_ADD
    ||
    fs_event_types & FSEvent::TYPE_FILE_ADD
  )
    mask |= IN_CREATE;

  if (
    fs_event_types & FSEvent::TYPE_DIRECTORY_REMOVE
    ||
    fs_event_types & FSEvent::TYPE_FILE_REMOVE
  )
    mask |= IN_DELETE;

  if (
    fs_event_types & FSEvent::TYPE_DIRECTORY_RENAME
    ||
    fs_event_types & FSEvent::TYPE_FILE_RENAME
  )
    mask |= IN_MOVED_FROM | IN_MOVED_TO;

  int wd = inotify_add_watch(inotify_fd, path.c_str(), mask);
  if (wd != -1) {
    Watch* watch = new Watch(fs_event_types, inotify_fd, path, recursive, wd);

    if (recursive) {
      Directory* directory = FileSystem().opendir(path);
      if (directory != NULL) {
        Directory::Entry* dirent = directory->read();
        if (dirent != NULL) {
          do {
            if (dirent->ISDIR() && !dirent->is_hidden() && !dirent->is_special()) {
              Path child_path = path / dirent->get_name();
              Watch* child_watch
                = create(fs_event_types, inotify_fd, child_path, recursive, watches);
              if (child_watch != NULL) {
                child_watches.push_back(child_watch);
                watches.insert(*child_watch);
              }
            }
          } while (directory->read(*dirent));

          Directory::Entry::dec_ref(*dirent);
        }

        Directory::dec_ref(*directory);
      }
    }

    return watch;
  } else
    return NULL;
}

void
FSEventQueue::Watch::read(
  const inotify_event& inotify_event_,
  EventHandler& fs_event_handler,
  Watches& watches
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

      if (isdir) {
        if (recursive) {
          Watch* child_watch =
            create(fs_event_types, inotify_fd, path, recursive, watches);

          if (child_watch != NULL)
            watches.insert(*child_watch);
        }

        fs_event_type = FSEvent::TYPE_DIRECTORY_ADD;
      } else
        fs_event_type = FSEvent::TYPE_FILE_ADD;
    } else if (
        (mask & IN_DELETE) == IN_DELETE
        ||
        (mask & IN_MOVED_FROM) == IN_MOVED_FROM
    ) {
      if (isdir) {
        Watch* child_watch = watches.erase(path);
        debug_assert_ne(child_watch, NULL);
        delete child_watch;

        if ((mask & IN_DELETE) == IN_DELETE) {
          mask ^= IN_DELETE;
          fs_event_type = FSEvent::TYPE_DIRECTORY_REMOVE;
        } else {
          mask ^= IN_MOVED_FROM;
          old_names[cookie] = name;
          fs_event_type = 0;
        }
      } else {
        if ((mask & IN_DELETE) == IN_DELETE) {
          mask ^= IN_DELETE;
          fs_event_type = FSEvent::TYPE_FILE_REMOVE;
        } else {
          mask ^= IN_MOVED_FROM;
          old_names[cookie] = name;
          fs_event_type = 0;
        }
      }
    } else if ((mask & IN_MOVED_TO) == IN_MOVED_TO) {
      mask ^= IN_MOVED_TO;

      map<uint32_t, Path>::iterator old_name_i = old_names.find(cookie);
      debug_assert_ne(old_name_i, old_names.end());

      if (isdir) {
        if (recursive) {
          Watch* child_watch
            = create(fs_event_types, inotify_fd, path, recursive, watches);
          if (child_watch != NULL)
            watches.insert(*child_watch);
        }

        fs_event_type = FSEvent::TYPE_DIRECTORY_RENAME;
      } else
        fs_event_type = FSEvent::TYPE_FILE_RENAME;

      if ((fs_event_types & fs_event_type) == fs_event_type) {
        FSEvent* fs_event
          = new FSEvent(this->path / old_name_i->second, path, fs_event_type);
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

    if (fs_event_type != 0 && (fs_event_types & fs_event_type) == fs_event_type)
      fs_event_handler.handle(*new FSEvent(path, fs_event_type));
  }
}

FSEventQueue::FSEventQueue() {
  inotify_fd = inotify_init();
  if (inotify_fd != -1) {
    if (fd_event_queue.associate(inotify_fd, POLLIN)) {
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
  delete watches;
}

bool
FSEventQueue::associate(
  const Path& path,
  FSEvent::Type fs_event_types,
  bool recursive
) {
  Watch* watch = watches->find(path);
  if (watch != NULL) {
    if (
      watch->get_fs_event_types() == fs_event_types
      &&
      watch->is_recursive() == recursive
    )
      return true;
    else {
      watch = watches->erase(path);
      debug_assert_ne(watch, NULL);
      delete watch;
    }
  }

  watch = Watch::create(fs_event_types, inotify_fd, path, recursive, *watches);
  if (watch != NULL) {
    watches->insert(*watch);
    return true;
  } else
    return false;
}

YO_NEW_REF Event* FSEventQueue::dequeue(const Time& timeout) {
  Event* event = fd_event_queue.dequeue(timeout);
  if (event != NULL) {
    if (event->get_type_id() == FDEvent::TYPE_ID) {
      FDEvent* fd_event = static_cast<FDEvent*>(event);
      if (fd_event->get_fd() == inotify_fd) {
        char inotify_fs_event_types[(sizeof(inotify_event) + PATH_MAX) * 16];
        ssize_t read_ret
          = ::read(inotify_fd, inotify_fs_event_types, sizeof(inotify_fs_event_types));

        if (read_ret > 0) {
          const char* inotify_fs_event_types_p = inotify_fs_event_types;
          const char* inotify_fs_event_types_pe
            = inotify_fs_event_types + static_cast<size_t>(read_ret);

          do {
            const inotify_event* inotify_event_
              = reinterpret_cast<const inotify_event*>(inotify_fs_event_types_p);

            Watch* watch = watches->find(inotify_event_->wd);
            if (watch != NULL)
              watch->read(*inotify_event_, *this, *watches);

            inotify_fs_event_types_p += sizeof(inotify_event) + inotify_event_->len;
          } while (inotify_fs_event_types_p < inotify_fs_event_types_pe);

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
}
}
}
}

