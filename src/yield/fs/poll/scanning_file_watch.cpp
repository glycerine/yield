// yield/fs/poll/scanning_file_watch.cpp

// Copyright (c) 2012 Minor Gordon
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

#include "scanning_file_watch.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/event_handler.hpp"
#include "yield/fs/file_system.hpp"

namespace yield {
namespace fs {
namespace poll {
ScanningFileWatch::ScanningFileWatch(
  FSEvent::Type fs_event_types,
  const Path& path,
  Log* log
) : ScanningWatch(fs_event_types, log, path) {
  stbuf = FileSystem().stat(get_path());
  if (stbuf == NULL) {
    throw Exception();
  }
}

ScanningFileWatch::~ScanningFileWatch() {
  delete stbuf;
}

void ScanningFileWatch::scan(EventHandler& fs_event_handler) {
  Stat* new_stbuf = FileSystem().stat(get_path());
  Stat* old_stbuf = stbuf;
  debug_assert_ne(old_stbuf, NULL);
  stbuf = NULL;

  FSEvent::Type fs_event_type;
  if (new_stbuf != NULL) { // File exists
    if (type(*new_stbuf) == type(*old_stbuf)) { // File has not changed type
      if (equals(*new_stbuf, *old_stbuf)) {
        delete old_stbuf;
        stbuf = new_stbuf;
        return;
      } else {
        fs_event_type = FSEvent::TYPE_FILE_MODIFY;
      }
    } else { // File has changed type
      fs_event_type = FSEvent::TYPE_FILE_REMOVE;
    }
  } else { // File does not exist any longer
    fs_event_type = FSEvent::TYPE_FILE_REMOVE;
  }

  if (want_fs_event_type(fs_event_type)) {
    FSEvent* fs_event = new FSEvent(get_path(), fs_event_type);
    log_fs_event(*fs_event);
    fs_event_handler.handle(*fs_event);
  }

  delete old_stbuf;
  stbuf = new_stbuf;
}
}
}
}
