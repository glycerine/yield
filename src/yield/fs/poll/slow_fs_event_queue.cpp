// yield/fs/poll/slow_fs_event_queue.cpp

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
#include "file_watch.hpp"
#include "yield/log.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/poll/slow_fs_event_queue.hpp"

namespace yield {
namespace fs {
namespace poll {
SlowFSEventQueue::SlowFSEventQueue(YO_NEW_REF Log* log)
  : log(Object::inc_ref(log)) {
}

SlowFSEventQueue::~SlowFSEventQueue() {
  Log::dec_ref(log);

  for (
    vector<Watch*>::iterator watch_i = watches.begin();
    watch_i != watches.end();
    ++watch_i
  )
    delete *watch_i;
}

bool
SlowFSEventQueue::associate(
  const Path& path,
  FSEvent::Type fs_event_types
) {
  dissociate(path);

  Stat* stbuf = FileSystem().stat(path);
  if (stbuf != NULL) {
    Watch* watch;
    if (stbuf->ISDIR())
      watch = new DirectoryWatch(fs_event_types, path, log);
    else
      watch = new FileWatch(fs_event_types, path, log);
    watch->read(event_queue);
    watches.push_back(watch);
    return true;
  } else
    return false;
}

bool SlowFSEventQueue::dissociate(const Path& path) {
  for (
    vector<Watch*>::iterator watch_i = watches.begin();
    watch_i != watches.end();
    ++watch_i
  ) {
    if ((*watch_i)->get_path() == path) {
      delete *watch_i;
      watches.erase(watch_i);
      return true;
    }
  }

  return false;
}

bool SlowFSEventQueue::enqueue(YO_NEW_REF Event& event) {
  return event_queue.enqueue(event);
}

YO_NEW_REF Event* SlowFSEventQueue::timeddequeue(const Time& timeout) {
  Event* event = event_queue.trydequeue();
  if (event != NULL)
    return event;
  else if (!watches.empty()) {
    Time timeout_remaining(timeout);
    for (;;) {
      for (
        vector<Watch*>::iterator watch_i = watches.begin();
        watch_i != watches.end();
        ++watch_i
      ) {
        Time start_time = Time::now();

        (*watch_i)->read(event_queue);
        event = event_queue.trydequeue();
        if (event != NULL)
          return event;

        Time elapsed_time = Time::now() - start_time;
        if (elapsed_time < timeout_remaining)
          timeout_remaining -= elapsed_time;
        else
          return event_queue.trydequeue();
      }
    }
  } else
    return event_queue.timeddequeue(timeout);
}
}
}
}