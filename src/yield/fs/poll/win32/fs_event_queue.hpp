// yield/fs/poll/win32/fs_event_queue.hpp

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

#ifndef _YIELD_FS_POLL_WIN32_FS_EVENT_QUEUE_HPP_
#define _YIELD_FS_POLL_WIN32_FS_EVENT_QUEUE_HPP_

#include <map>

#include "yield/aio/win32/aio_queue.hpp"
#include "yield/fs/poll/fs_event.hpp"

namespace yield {
namespace fs {
namespace poll {
namespace win32 {
class FSEventQueue : public EventQueue {
public:
  ~FSEventQueue();

public:
  bool associate(
    const Path& path,
    FSEvent::Type events = FSEvent::TYPE_ALL,
    bool recursive = true
  );

  bool dissociate(const Path& path);

public:
  // yield::EventQueue
  YO_NEW_REF Event* dequeue(const Time& timeout);
  bool enqueue(YO_NEW_REF Event& event);

private:
  class Watch;

private:
  yield::aio::win32::AIOQueue aio_queue;
  std::map<Path, Watch*> watches;
};
}
}
}
}

#endif
