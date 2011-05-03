// yield/aio/win32/aio_queue.cpp

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
#include "yield/aio/aiocb.hpp"
#include "yield/aio/win32/aio_queue.hpp"

#include <Windows.h>

namespace yield {
namespace aio {
namespace win32 {
AIOQueue::AIOQueue() {
  hIoCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (hIoCompletionPort == INVALID_HANDLE_VALUE)
    throw Exception();
}

AIOQueue::~AIOQueue() {
  CloseHandle(hIoCompletionPort);
}

bool AIOQueue::associate(fd_t fd) {
  return CreateIoCompletionPort(
           fd,
           hIoCompletionPort,
           0,
           0
         ) != INVALID_HANDLE_VALUE;
}

YO_NEW_REF Event* AIOQueue::dequeue(const Time& timeout) {
  DWORD dwBytesTransferred = 0;
  ULONG_PTR ulCompletionKey = 0;
  LPOVERLAPPED lpOverlapped = NULL;

  BOOL bRet
    = GetQueuedCompletionStatus(
        hIoCompletionPort,
        &dwBytesTransferred,
        &ulCompletionKey,
        &lpOverlapped,
        static_cast<DWORD>(timeout.ms())
      );

  if (lpOverlapped != NULL) {
    AIOCB& aiocb = AIOCB::cast(*lpOverlapped);

    if (bRet)
      aiocb.set_return(dwBytesTransferred);
    else
      aiocb.set_error(GetLastError());

    return &aiocb;
  } else if (ulCompletionKey != 0)
    return reinterpret_cast<Event*>(ulCompletionKey);
  else
    return NULL;
}

bool AIOQueue::enqueue(YO_NEW_REF Event& event) {
  return PostQueuedCompletionStatus(
           hIoCompletionPort,
           0,
           reinterpret_cast<ULONG_PTR>(&event),
           NULL
         )
         == TRUE;
}
}
}
}
