// yield/poll/socket_event_queue.cpp

// Copyright (c) 2010 Minor Gordon
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


#ifdef _WIN32
#include "win32/selector.hpp"
#include "win32/wsapoller.hpp"
#else
#include "yield/poll/fd_event_queue.hpp"
#endif
#include "yield/poll/socket_event_queue.hpp"


namespace yield {
namespace poll {
SocketEventQueue::SocketEventQueue() {
#ifdef _WIN32
#if _WIN32_WINNT >= 0x0600
  pimpl = new win32::WSAPoller;
#else
  pimpl = new win32::Selector;
#endif
#else
  pimpl = new yield::poll::FDEventQueue;
#endif
}

SocketEventQueue::~SocketEventQueue() {
  delete pimpl;
}

bool SocketEventQueue::associate(socket_t socket_, int16_t events) {
  return pimpl->associate(socket_, events);
}

bool SocketEventQueue::dissociate(socket_t socket_) {
  return pimpl->dissociate(socket_);
}

Event* SocketEventQueue::dequeue(const Time& timeout) {
  return pimpl->dequeue(timeout);
}

bool SocketEventQueue::enqueue(Event& event) {
  return pimpl->enqueue(event);
}
}
}
