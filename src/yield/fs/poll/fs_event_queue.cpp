// yield/fs/poll/fs_event_queue.cpp

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

#if defined(__linux__)
#include "linux/fs_event_queue.hpp"
#elif defined(_WIN32)
#include "win32/fs_event_queue.hpp"
#endif
#include "yield/fs/poll/fs_event_queue.hpp"

namespace yield {
namespace fs {
namespace poll {
FSEventQueue::FSEventQueue() {
#if defined(__linux__)
  pimpl = new linux::FSEventQueue;
#elif defined(_WIN32)
  pimpl = new win32::FSEventQueue;
#endif
}

FSEventQueue::~FSEventQueue() {
#if defined(__linux__)
  linux::FSEventQueue::dec_ref(*pimpl);
#elif defined(_WIN32)
  win32::FSEventQueue::dec_ref(*pimpl);
#endif
}

bool
FSEventQueue::associate(
  const Path& path,
  FSEvent::Type events,
  bool recursive
) {
  return pimpl->associate(path, events, recursive);
}

bool FSEventQueue::dissociate(const Path& path) {
  return pimpl->dissociate(path);
}

YO_NEW_REF Event* FSEventQueue::dequeue(const Time& timeout) {
  return pimpl->dequeue(timeout);
}

bool FSEventQueue::enqueue(YO_NEW_REF Event& event) {
  return pimpl->enqueue(event);
}
}
}
}
