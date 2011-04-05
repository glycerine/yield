// yield/fs/poll/linux/file_system_change_event_queue.cpp

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

#include "inotify_volume_change_event_queue.h"
using yield::platform::linux2::InotifyVolumeChangeEventQueue;

#include "../posix/volume.h"
using yield::platform::posix::Volume;

#include <sys/inotify.h>


class InotifyVolumeChangeEventQueue::Watch
  : public VolumeChangeEventQueue::Watch,
    private vector<Watch*> {
public:
  void close() {
    if (wd != -1) {
      inotify_rm_watch(inotify_fd, wd);
      wd = -1;
    }
  }

  static Watch*
  create
  (
    VolumeChangeEvent::Type events,
    Flag flags,
    int inotify_fd,
    const Path& path,
    Watch* parent = NULL
  ) {
    uint32_t mask = 0;

    if (flags & FLAG_DONT_FOLLOW)
      mask |= IN_DONT_FOLLOW;

    if (flags & FLAG_ONESHOT)
      mask |= IN_ONESHOT;

    if
    (
      events & VolumeChangeEvent::TYPE_DIRECTORY_MODIFY
      ||
      events & VolumeChangeEvent::TYPE_FILE_MODIFY
    )
      mask |= IN_ATTRIB | IN_MODIFY;

    if
    (
      events & VolumeChangeEvent::TYPE_DIRECTORY_REMOVE
      ||
      events & VolumeChangeEvent::TYPE_FILE_REMOVE
    )
      mask |= IN_DELETE_SELF;

    if
    (
      events & VolumeChangeEvent::TYPE_DIRECTORY_RENAME
      ||
      events & VolumeChangeEvent::TYPE_FILE_RENAME
    )
      mask |= IN_MOVE_SELF;

    if (flags & FLAG_ONLYDIR) {
      mask |= IN_ONLYDIR;

      if
      (
        events & VolumeChangeEvent::TYPE_DIRECTORY_ADD
        ||
        events & VolumeChangeEvent::TYPE_FILE_ADD
      )
        mask |= IN_CREATE;

      if
      (
        events & VolumeChangeEvent::TYPE_DIRECTORY_REMOVE
        ||
        events & VolumeChangeEvent::TYPE_FILE_REMOVE
      )
        mask |= IN_DELETE;

      if
      (
        events & VolumeChangeEvent::TYPE_DIRECTORY_RENAME
        ||
        events & VolumeChangeEvent::TYPE_FILE_RENAME
      )
        mask |= IN_MOVED_FROM | IN_MOVED_TO;
    }

    int inotify_wd = inotify_add_watch(inotify_fd, path, mask);
    if (inotify_wd != -1) {
      Directory* dir = yield::platform::posix::Volume().opendir(path);
      if (dir != NULL)
        flags |= FLAG_ONLYDIR;

      Watch* watch
      = new Watch
      (
        events,
        flags,
        inotify_fd,
        path.split().second,
        parent,
        path,
        inotify_wd
      );

      if (dir != NULL) {
        if (flags & FLAG_RECURSIVE) {
          Directory::Entry* dirent = dir->read(Directory::Entry::TYPE_DIR);
          if (dirent != NULL) {
            do {
              if (!dirent->is_hidden() && !dirent->is_special()) {
                Path child_path = path / dirent->get_name();
                Watch* child_watch
                = create(events, flags, inotify_fd, child_path, watch);
                if (child_watch != NULL)
                  watch->push_back(child_watch);
              }
            } while (dir->read(*dirent, Directory::Entry::TYPE_DIR));

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

  bool read(VolumeChangeEvent& volume_change_event) {
    uint32_t mask, cookie;
    Path name;
    if (last_read_mask == 0) {
      uint32_t mask_cookie_len[3];
      ssize_t read_ret
      = ::read(inotify_fd, mask_cookie_len, sizeof(mask_cookie_len));
      if (read_ret != sizeof(mask_cookie_len)) DebugBreak();

      mask = mask_cookie_len[0];
      cookie = mask_cookie_len[1];
      uint32_t len = mask_cookie_len[2];

      if (len > 0) {
        if (!this->isdir()) DebugBreak();

        char* read_name = new char[len];
        read_ret = ::read(inotify_fd, read_name, len);
        if (read_ret != len) DebugBreak();
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
    Path path = get_path() / name;

    for (;;) { // Mask can have multiple bits set.
      // Keep looping until we find one that matches events.
      VolumeChangeEvent::Type type;

      if (mask & IN_ATTRIB) {
        if (isdir)
          type = VolumeChangeEvent::TYPE_DIRECTORY_MODIFY;
        else
          type = VolumeChangeEvent::TYPE_FILE_MODIFY;

        mask ^= IN_ATTRIB;
      } else if (mask & IN_CREATE) {
        if (isdir) {
          type = VolumeChangeEvent::TYPE_DIRECTORY_ADD;

          if (get_flags() & FLAG_RECURSIVE) {
            Watch* child_watch
            = create(get_events(), get_flags(), inotify_fd, path, this);

            if (child_watch != NULL)
              push_back(child_watch);
          }
        } else
          type = VolumeChangeEvent::TYPE_FILE_ADD;

        mask ^= IN_CREATE;
      } else if (mask & IN_DELETE) {
        if (isdir) {
          type = VolumeChangeEvent::TYPE_DIRECTORY_REMOVE;

          Watch* child_watch = find_child_watch(name);
          if (child_watch != NULL) {
            child_watch->close();
            delete child_watch;
          }
        } else
          type = VolumeChangeEvent::TYPE_FILE_REMOVE;

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
        if (old_name_i == old_names.end()) DebugBreak();

        if (isdir) {
          type = VolumeChangeEvent::TYPE_DIRECTORY_RENAME;

          if (get_flags() & FLAG_RECURSIVE) {
            Watch* child_watch
            = create(get_events(), get_flags(), inotify_fd, path, this);

            if (child_watch != NULL)
              push_back(child_watch);
          }
        } else
          type = VolumeChangeEvent::TYPE_FILE_RENAME;

        if ((get_events() & type) == type) {
          volume_change_event.set_new_path(path);
          volume_change_event.set_old_path(get_path() / old_name_i->second);
          old_names.erase(old_name_i);
          volume_change_event.set_type(type);
          return true;
        } else {
          old_names.erase(old_name_i);
          continue;
        }

        mask ^= IN_MOVED_TO;
      } else {
        last_read_cookie = 0;
        last_read_mask = 0;
        return false;
      }

      if ((get_events() & type) == type) {
        last_read_cookie = cookie;
        last_read_mask = mask;
        last_read_name = name;
        volume_change_event.set_path(path);
        volume_change_event.set_type(type);
        return true;
      } else
        continue;
    }
  }

private:
  Watch
  (
    VolumeChangeEvent::Type events,
    Flag flags,
    int inotify_fd,
    const Path& name,
    Watch* parent,
    const Path& path,
    int wd
  )
    : VolumeChangeEventQueue::Watch(events, flags, path),
      inotify_fd(inotify_fd),
      name(name),
      parent(parent),
      wd(wd)
  { }

  Watch* find_child_watch(const Path& name) {
    for (iterator watch_i = begin(); watch_i != end(); ++watch_i) {
      Watch* watch = (*watch_i)->find_watch(wd);
      if (watch != NULL)
        return watch;
    }

    return NULL;
  }

private:
  int inotify_fd;
  uint32_t last_read_cookie, last_read_mask;
  Path last_read_name;
  Path name;
  map<uint32_t, Path> old_names;
  Watch* parent;
  int wd;
};


InotifyVolumeChangeEventQueue::InotifyVolumeChangeEventQueue
(
  FDEventQueue& fd_event_queue,
  int inotify_fd
)
  : fd_event_queue(fd_event_queue),
    inotify_fd(inotify_fd)
{ }

InotifyVolumeChangeEventQueue::~InotifyVolumeChangeEventQueue() {
  delete &fd_event_queue;

  close(inotify_fd);

  for
  (
    WatchMap<Watch>::iterator watch_i = watches.begin();
    watch_i != watches.end();
    ++watch_i
  )
    // Don't need to close the Watch here, since the inotify_fd close does that
    delete watch_i->second;
}

bool
InotifyVolumeChangeEventQueue::associate
(
  const Path& path,
  VolumeChangeEvent::Type events,
  Flag flags
) {
  Watch* watch = watches.find(path);
  if (watch != NULL)
    DebugBreak(); // Modify the existing watch
  else {
    watch = Watch::create(events, flags, inotify_fd, path);
    if (watch != NULL) {
      if (fd_event_queue.associate(*watch)) {
        watches.insert(path, *watch);
        return true;
      } else
        delete watch;
    }
  }

  return false;
}

InotifyVolumeChangeEventQueue&
InotifyVolumeChangeEventQueue::create() {
  int inotify_fd = inotify_init();
  if (inotify_fd != -1) {
    FDEventQueue* fd_event_queue = FDEventQueue::create();
    if (fd_event_queue != NULL) {
      if (fd_event_queue->associate(inotify_fd)) {
        return *new InotifyVolumeChangeEventQueue
               (
                 *fd_event_queue,
                 inotify_fd
               );
      } else
        delete fd_event_queue;
    }

    close(inotify_fd);
  }

  throw Exception();
}

void InotifyVolumeChangeEventQueue::dissociate(const Path& path) {
  Watch* watch = watches.erase(path);
  if (watch != NULL) {
    watch->close();
    delete watch;
  }
}

int
InotifyVolumeChangeEventQueue::poll
(
  VolumeChangeEvent* volume_change_events,
  int volume_change_events_len,
  const Time& _timeout
) {
  Time timeout(_timeout);

  int volume_change_event_i
  = VolumeChangeEventQueue::get_leftover_volume_change_events
    (
      volume_change_events,
      volume_change_events_len
    );

  while
  (
    volume_change_event_i < volume_change_events_len
    &&
    fd_event_queue.poll(timeout)
  ) {
    int wd;
    ssize_t read_ret = ::read(inotify_fd, &wd, sizeof(wd));
    if (read_ret != sizeof(wd)) DebugBreak();

    for
    (
      WatchMap<Watch>::iterator watch_i = watches.begin();
      watch_i != watches.end();
      ++watch_i
    ) {
      Watch* watch = watch_i->second->find_watch(wd);
      if (watch != NULL) {
        volume_change_event_i +=
          read
          (
            volume_change_events + volume_change_event_i,
            volume_change_events_len - volume_change_event_i,
            *watch
          );

        break;
      }
    }

    timeout = static_cast<uint64_t>(0);
  }

  return volume_change_event_i;
}
