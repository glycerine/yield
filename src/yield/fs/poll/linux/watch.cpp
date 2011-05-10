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
#include "yield/log.hpp"

#include <iostream>
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
) : yield::fs::poll::Watch(fs_event_types, log, path),
    inotify_fd(inotify_fd),
    wd(wd) {
}

Watch::~Watch() {
  inotify_rm_watch(inotify_fd, wd);
}

YO_NEW_REF FSEvent* Watch::parse(const inotify_event& inotify_event_) {
  uint32_t mask = inotify_event_.mask;

  bool isdir;
  if ((mask & IN_ISDIR) == IN_ISDIR) {
    mask ^= IN_ISDIR;
    isdir = true;
  } else
    isdir = false;
  Path name, path;
  if (inotify_event_.len > 1) {
    // len includes a NULL terminator, but may also include
    // one or more bytes of padding. Do a strlen to find the real length.
    name = Path(inotify_event_.name); //, inotify_event_.len - 1);
    path = this->get_path() / name;
  } else
    path = this->get_path();

  if (get_log() != NULL) {
    get_log()->get_stream(Log::Level::DEBUG) <<
      get_type_name() << "(path=" << get_path() << ", wd=" << get_wd() << ")"
      << ": read inotify_event(" <<
        "cookie=" << inotify_event_.cookie <<
        ", " <<
        "isdir=" << (isdir ? "true" : "false") <<
        ", " <<
        "len=" << inotify_event_.len <<
        ", " <<
        "mask=" << inotify_event_.mask <<
        ", "
        "name=" << name <<
      ")";
  }

  FSEvent::Type fs_event_type;

  if ((mask & IN_ATTRIB) == IN_ATTRIB) {
    mask ^= IN_ATTRIB;
    fs_event_type =
      isdir ? FSEvent::TYPE_DIRECTORY_MODIFY : FSEvent::TYPE_FILE_MODIFY;
  } else if ((mask & IN_CREATE) == IN_CREATE) {
    mask ^= IN_CREATE;
    fs_event_type =
      isdir ? FSEvent::TYPE_DIRECTORY_ADD : FSEvent::TYPE_FILE_ADD;
  } else if ((mask & IN_DELETE) == IN_DELETE) {
    mask ^= IN_DELETE;
    fs_event_type =
      isdir ? FSEvent::TYPE_DIRECTORY_REMOVE : FSEvent::TYPE_FILE_REMOVE;
  } else if ((mask & IN_DELETE_SELF) == IN_DELETE_SELF) {
    mask ^= IN_DELETE_SELF;
    fs_event_type =
      isdir ? FSEvent::TYPE_DIRECTORY_REMOVE : FSEvent::TYPE_FILE_REMOVE;
  } else if ((mask & IN_IGNORED) == IN_IGNORED) {
    mask ^= IN_IGNORED;
    debug_assert_false(isdir);
    fs_event_type = FSEvent::TYPE_FILE_REMOVE;
  } else if ((mask & IN_MOVED_FROM) == IN_MOVED_FROM) {
    mask ^= IN_MOVED_FROM;
    debug_assert_false(name.empty());
    old_names[inotify_event_.cookie] = name;
    return NULL;
  } else if ((mask & IN_MOVED_TO) == IN_MOVED_TO) {
    mask ^= IN_MOVED_TO;
    debug_assert_eq(mask, 0);

    fs_event_type =
      isdir ? FSEvent::TYPE_DIRECTORY_RENAME : FSEvent::TYPE_FILE_RENAME;

    map<uint32_t, Path>::iterator old_name_i
      = old_names.find(inotify_event_.cookie);
    debug_assert_ne(old_name_i, old_names.end());

    if (want_fs_event_type(fs_event_type)) {
      FSEvent* fs_event
        = new FSEvent(
                this->get_path() / old_name_i->second,
                path,
                fs_event_type
              );
      old_names.erase(old_name_i);
      log_fs_event(*fs_event);
      return fs_event;
    } else {
      old_names.erase(old_name_i);
      return NULL;
    }
  }

  debug_assert_eq(mask, 0);

  if (want_fs_event_type(fs_event_type)) {
    FSEvent* fs_event = new FSEvent(path, fs_event_type);
    log_fs_event(*fs_event);
    return fs_event;
  } else
    return NULL;
}
}
}
}
}
