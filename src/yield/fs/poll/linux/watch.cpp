// yield/fs/poll/linux/watch.cpp

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

#include "watch.hpp"
#include "yield/assert.hpp"
#include "yield/event_handler.hpp"
#include "yield/log.hpp"

#include <sys/inotify.h>

namespace yield {
namespace fs {
namespace poll {
namespace linux {
using std::map;

Watch::Watch(
  FSEvent::Type fs_event_types,
  int inotify_fd,
  const Path& path,
  int wd,
  Log* log
) : yield::fs::poll::Watch(fs_event_types, path, log),
    inotify_fd(inotify_fd),
    wd(wd) {
}

Watch::~Watch() {
  inotify_rm_watch(inotify_fd, wd);
}

void
Watch::read(
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
    path = this->get_path() / name;
  }

  if (get_log() != NULL) {
    get_log()->get_stream(Log::Level::DEBUG) <<
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

      if ((get_fs_event_types() & fs_event_type) == fs_event_type) {
        FSEvent* fs_event
          = new FSEvent(
                  this->get_path() / old_name_i->second,
                  path,
                  fs_event_type
                );
        old_names.erase(old_name_i);

        if (get_log() != NULL) {
          get_log()->get_stream(Log::Level::DEBUG) <<
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
      (get_fs_event_types() & fs_event_type) == fs_event_type
    ) {
      FSEvent* fs_event = new FSEvent(path, fs_event_type);

      if (get_log() != NULL) {
        get_log()->get_stream(Log::Level::DEBUG) <<
          get_type_name() <<
            "(path=" << get_path() << ", wd=" << get_wd() << ")" <<
            ": read " << *fs_event;
      }

      fs_event_handler.handle(*fs_event);
    }
  }
}
}
}
}
}
