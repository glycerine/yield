// yield/fs/poll/file_system_change_event_queue.cpp

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

#include "yield/fs/file_system_change_event_queue.h"

#if defined(_WIN32)
#include "win32/directory_change_event_queue.h"
#elif defined(__linux__)
#include "linux2/inotify_volume_change_event_queue.h"
#endif

namespace yield {
namespace fs {
namespace poll {
VolumeChangeEventQueue& VolumeChangeEventQueue::create() {
#if defined(_WIN32)
  return yield::fs::poll::win32::DirectoryChangeEventQueue::create();
#elif defined(YIELD_PLATFORM_HAVE_LINUX_INOTIFY)
  return yield::fs::poll::linux2::InotifyVolumeChangeEventQueue::create();
#else
  DebugBreak();
  //return FallbackVolumeChangeEventQueue()::create();
#endif
}

int
VolumeChangeEventQueue::get_leftover_volume_change_events
(
  VolumeChangeEvent* volume_change_events,
  int volume_change_events_len
) {
#ifdef _DEBUG
  if (volume_change_events_len == 0) DebugBreak();
#endif

  int volume_change_event_i = 0;

  while
  (
    !leftover_volume_change_events.empty()
    &&
    volume_change_event_i < volume_change_events_len
  ) {
    volume_change_events[volume_change_event_i++]
    = *leftover_volume_change_events.front();
    leftover_volume_change_events.pop();
  }

  return volume_change_event_i;
}

bool
VolumeChangeEventQueue::poll
(
  VolumeChangeEvent& volume_change_event,
  const Time& timeout
) {
  return poll(&volume_change_event, 1, timeout) == 1;
}

int
VolumeChangeEventQueue::read
(
  VolumeChangeEvent* volume_change_events,
  int volume_change_events_len,
  Watch& watch
) {
#ifdef _DEBUG
  if (volume_change_events_len == 0) DebugBreak();
#endif

  int volume_change_event_i = 0;

  while (watch.read(volume_change_events[volume_change_event_i])) {
    if (++volume_change_event_i == volume_change_events_len) {
      VolumeChangeEvent* leftover_vce = new VolumeChangeEvent;
      while (watch.read(*leftover_vce)) {
        leftover_volume_change_events.push(leftover_vce);
        leftover_vce = new VolumeChangeEvent;
      }
      delete leftover_vce;

      break;
    }
  }

  return volume_change_event_i;
}
}
}
}
