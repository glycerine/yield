// yield/fs/poll/scanning_fs_event_queue.cpp

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

#include "scanning_directory_watch.hpp"
#include "scanning_file_watch.hpp"
#include "watches.hpp"
#include "yield/log.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/poll/scanning_fs_event_queue.hpp"

namespace yield {
namespace fs {
namespace poll {
ScanningFSEventQueue::ScanningFSEventQueue(YO_NEW_REF Log* log)
  : log(Object::inc_ref(log)) {
  watches = new Watches<ScanningWatch>;
}

ScanningFSEventQueue::~ScanningFSEventQueue() {
  Log::dec_ref(log);
  delete watches;
}

bool
ScanningFSEventQueue::associate(
  const Path& path,
  FSEvent::Type fs_event_types
) {
  dissociate(path);

  Stat* stbuf = FileSystem().stat(path);
  if (stbuf != NULL) {
    ScanningWatch* watch;
    if (stbuf->ISDIR()) {
      watch = new ScanningDirectoryWatch(fs_event_types, path, log);
    } else {
      watch = new ScanningFileWatch(fs_event_types, path, log);
    }
    watches->insert(path, *watch);
    return true;
  } else {
    return false;
  }
}

bool ScanningFSEventQueue::dissociate(const Path& path) {
  ScanningWatch* watch = watches->erase(path);
  if (watch != NULL) {
    delete watch;
    return true;
  } else {
    return false;
  }
}

bool ScanningFSEventQueue::enqueue(YO_NEW_REF Event& event) {
  return event_queue.enqueue(event);
}

YO_NEW_REF Event* ScanningFSEventQueue::timeddequeue(const Time& timeout) {
  Event* event = event_queue.trydequeue();
  if (event != NULL) {
    return event;
  } else if (!watches->empty()) {
    Time timeout_remaining(timeout);
    for (;;) {
      for (
        Watches<ScanningWatch>::const_iterator watch_i = watches->begin();
        watch_i != watches->end();
        ++watch_i
      ) {
        Time start_time = Time::now();

        watch_i->second->scan(event_queue);
        event = event_queue.trydequeue();
        if (event != NULL) {
          return event;
        }

        Time elapsed_time = Time::now() - start_time;
        if (elapsed_time < timeout_remaining) {
          timeout_remaining -= elapsed_time;
        } else {
          return event_queue.trydequeue();
        }
      }
    }
  } else {
    return event_queue.timeddequeue(timeout);
  }
}
}
}
}
