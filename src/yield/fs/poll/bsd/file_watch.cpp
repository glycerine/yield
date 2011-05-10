// yield/fs/poll/bsd/file_watch.cpp

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

#include "file_watch.hpp"
#include "yield/assert.hpp"

#include <sys/event.h>

namespace yield {
namespace fs {
namespace poll {
namespace bsd {
FileWatch::FileWatch(
  int fd,
  FSEvent::Type fs_event_types,
  const Path& path,
  Log* log
) :
  yield::fs::poll::bsd::Watch(fd),
  yield::fs::poll::Watch(fs_event_types, path, log) {
}

void
DirectoryWatch::read(
  const kevent& kevent_,
  EventHandler& fs_event_handler
) {
  uint32_t fflags = kevent_.fflags;
  while (fflags != 0) {
    FSEvent::Type fs_event_type;
    if ((fflags & NOTE_ATTRIB) == NOTE_ATTRIB) {
      fflags ^= NOTE_ATTRIB;
      fs_event_type = FSEvent::TYPE_FILE_MODIFY;
    } else if ((fflags & NOTE_DELETE) == NOTE_DELETE) {
      fflags ^= NOTE_DELETE;
      fs_event_type = FSEvent::TYPE_FILE_REMOVE;
    } else if ((fflags & NOTE_EXTEND) == NOTE_EXTEND) {
      fflags ^= NOTE_EXTEND;
      fs_event_type = FSEvent::TYPE_FILE_MODIFY;
    } else if ((fflags & NOTE_LINK) == NOTE_LINK) {
      fflags ^= NOTE_LINK;
      fs_event_type = FSEvent::TYPE_FILE_MODIFY;
    } else if ((fflags & NOTE_RENAME) == NOTE_RENAME) {
      fflags ^= NOTE_RENAME;
      fs_event_type = FSEvent::TYPE_FILE_RENAME;
    } else if ((fflags & NOTE_REVOKE) == NOTE_REVOKE) {
      fflags ^= NOTE_REVOKE;
      fs_event_type = FSEvent::TYPE_FILE_REMOVE;
    } else if ((fflags & NOTE_WRITE) == NOTE_WRITE) {
      fflags ^= NOTE_WRITE;
      fs_event_type = FSEvent::TYPE_FILE_MODIFY;
    } else {
      debug_break();
      return;
    }
      
    FSEvent* fs_event = new FSEvent(get_path(), fs_event_type);
    log_fs_event(*fs_event);
    fs_event_handler.handle(*fs_event);
  }
}
}
}
}