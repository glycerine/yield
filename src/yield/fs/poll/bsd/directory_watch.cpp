// yield/fs/poll/bsd/directory_watch.cpp

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

#include "directory_watch.hpp"
#include "yield/event_handler.hpp"

#include <sys/event.h>

namespace yield {
namespace fs {
namespace poll {
namespace bsd {
DirectoryWatch::DirectoryWatch(
  int fd,
  FSEvent::Type fs_event_types,
  const Path& path,
  Log* log
) :
  yield::fs::poll::bsd::Watch(fd),
  ScanningDirectoryWatch(fs_event_types, path, log) {
}

void
DirectoryWatch::read(
  const struct kevent& kevent_,
  EventHandler& fs_event_handler
) {
  FSEvent::Type fs_event_types = 0;
  if ((kevent_.fflags & NOTE_ATTRIB) == NOTE_ATTRIB) {
    fs_event_types |= FSEvent::TYPE_DIRECTORY_MODIFY;
    fs_event_types |= FSEvent::TYPE_FILE_MODIFY;
  } else if ((kevent_.fflags & NOTE_DELETE) == NOTE_DELETE) {
    fs_event_types |= FSEvent::TYPE_DIRECTORY_REMOVE;
    fs_event_types |= FSEvent::TYPE_FILE_REMOVE;
  } else if ((kevent_.fflags & NOTE_EXTEND) == NOTE_EXTEND) {
    fs_event_types |= FSEvent::TYPE_DIRECTORY_MODIFY;
    fs_event_types |= FSEvent::TYPE_FILE_MODIFY;
  } else if ((kevent_.fflags & NOTE_LINK) == NOTE_LINK) {
    fs_event_types |= FSEvent::TYPE_DIRECTORY_MODIFY;
    fs_event_types |= FSEvent::TYPE_FILE_MODIFY;
  } else if ((kevent_.fflags & NOTE_RENAME) == NOTE_RENAME) {
    fs_event_types |= FSEvent::TYPE_DIRECTORY_RENAME;
    fs_event_types |= FSEvent::TYPE_FILE_RENAME;
  } else if ((kevent_.fflags & NOTE_REVOKE) == NOTE_REVOKE) {
    fs_event_types |= FSEvent::TYPE_DIRECTORY_REMOVE;
    fs_event_types |= FSEvent::TYPE_FILE_REMOVE;
  } else if ((kevent_.fflags & NOTE_WRITE) == NOTE_WRITE) {
    fs_event_types |= FSEvent::TYPE_DIRECTORY_MODIFY;
    fs_event_types |= FSEvent::TYPE_FILE_MODIFY;
  }

  fs_event_types ^= this->get_fs_event_types();

  ScanningDirectoryWatch::scan(fs_event_handler, fs_event_types);
}
}
}
}
}

