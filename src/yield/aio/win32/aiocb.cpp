// yield/aio/win32/aiocb.cpp

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

#include "yield/assert.hpp"
#include "yield/channel.hpp"
#include "yield/event_handler.hpp"
#include "yield/aio/win32/aiocb.hpp"

#include <Windows.h>


namespace yield {
namespace aio {
namespace win32 {
AIOCB::AIOCB(Channel& channel, void*, size_t nbytes, uint64_t offset)
  : channel(channel.inc_ref()), nbytes(nbytes) {
  static_assert(sizeof(OVERLAPPED) == sizeof(::OVERLAPPED), "");
  debug_assert_ne(static_cast<fd_t>(channel), INVALID_FD);

  completion_handler = NULL;
  error = 0;
  memset(&overlapped, 0, sizeof(overlapped));
  overlapped.Offset = static_cast<uint32_t>(offset);
  overlapped.OffsetHigh = static_cast<uint32_t>(offset >> 32);
  return_ = -1;
  this_aiocb = this;
}

AIOCB::~AIOCB() {
  Channel::dec_ref(channel);
  EventHandler::dec_ref(completion_handler);
}

bool AIOCB::cancel() {
  return CancelIoEx(get_channel(), *this) == TRUE;
}

AIOCB& AIOCB::cast(::OVERLAPPED& lpOverlapped) {
  AIOCB* aiocb;

  memcpy_s
  (
    &aiocb,
    sizeof(aiocb),
    reinterpret_cast<char*>(&lpOverlapped) + sizeof(::OVERLAPPED),
    sizeof(aiocb)
  );

  return *aiocb;
}

uint64_t AIOCB::get_offset() const {
  return static_cast<uint64_t>(overlapped.OffsetHigh << 32)
         |
         overlapped.Offset;
}

bool AIOCB::issue(EventHandler& completion_handler) {
  retry();
  completion_handler.handle(*this);
  return true;
}

bool AIOCB::issue(win32::AIOQueue&) {
  DebugBreak();
  SetLastError(ERROR_NOT_SUPPORTED);
  return false;
}

AIOCB::operator ::OVERLAPPED* () {
  return reinterpret_cast<::OVERLAPPED*>(&overlapped);
}

void AIOCB::set_completion_handler(EventHandler& completion_handler) {
  EventHandler::dec_ref(this->completion_handler);
  this->completion_handler = &completion_handler.inc_ref();
}
}
}
}
