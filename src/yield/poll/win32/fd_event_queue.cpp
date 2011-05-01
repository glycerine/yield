// yield/poll/win32/fd_event_queue.cpp

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

#include "yield/exception.hpp"
#include "yield/poll/fd_event.hpp"
#include "yield/poll/win32/fd_event_queue.hpp"

#include <Windows.h>

namespace yield {
namespace poll {
namespace win32 {
using yield::thread::BlockingConcurrentQueue;

FDEventQueue::FDEventQueue() {
  HANDLE hWakeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (hWakeEvent != NULL)
    fds.push_back(hWakeEvent);
  else
    throw Exception();
}

FDEventQueue::~FDEventQueue() {
  CloseHandle(fds[0]);
}

bool FDEventQueue::associate(fd_t fd, uint16_t events) {
  if (events != 0) {
    if (fds.size() < MAXIMUM_WAIT_OBJECTS) {
      for (
        vector<fd_t>::const_iterator fd_i = fds.begin();
        fd_i != fds.end();
        ++fd_i
      ) {
        if (*fd_i == fd)
          return true;
      }

      fds.push_back(fd);
      return true;
    } else
      return false;
  } else
    return dissociate(fd);
}

bool FDEventQueue::dissociate(fd_t fd) {
  for (
    vector<fd_t>::iterator fd_i = fds.begin();
    fd_i != fds.end();
    ++fd_i
  ) {
    if (*fd_i == fd) {
      fds.erase(fd_i);
      return true;
    }
  }

  SetLastError(ERROR_INVALID_HANDLE);

  return false;
}

Event* FDEventQueue::dequeue(const Time& timeout) {
  DWORD dwRet
  = WaitForMultipleObjectsEx(
      fds.size(),
      &fds[0],
      FALSE,
      static_cast<DWORD>(timeout.ms()),
      TRUE
    );

  if (dwRet == WAIT_OBJECT_0)
    return BlockingConcurrentQueue<Event>::trydequeue();
  else if (dwRet > WAIT_OBJECT_0 && dwRet < WAIT_OBJECT_0 + fds.size())
    return new FDEvent(POLLIN | POLLOUT, fds[dwRet - WAIT_OBJECT_0]);
  else
    return NULL;
}

bool FDEventQueue::enqueue(Event& event) {
  if (BlockingConcurrentQueue<Event>::enqueue(event)) {
    SetEvent(fds[0]);
    return true;
  } else
    return false;
}
}
}
}
