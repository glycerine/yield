// fd_event_test.cpp

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
#include "yield/poll/fd_event.hpp"
#include "yunit.hpp"

TEST_SUITE(FDEvent);

namespace yield {
namespace poll {
TEST(FDEvent, constructors) {
  FDEvent();
  FDEvent(POLLIN, static_cast<fd_t>(0));
#ifdef _WIN32
  FDEvent(POLLIN, static_cast<socket_t>(0));
#endif
}

TEST(FDEvent, get_events) {
  throw_assert_eq(FDEvent(POLLIN, static_cast<fd_t>(0)).get_events(), POLLIN);
}

TEST(FDEvent, get_fd) {
  throw_assert_eq(FDEvent(POLLIN, static_cast<fd_t>(0)).get_fd(), 0);
}

TEST(FDEvent, get_socket) {
  throw_assert_eq(FDEvent(POLLIN, static_cast<socket_t>(0)).get_socket(), 0);
}

TEST(FDEvent, get_type_id) {
  throw_assert_eq(FDEvent().get_type_id(), FDEvent::TYPE_ID);
}

TEST(FDEvent, get_type_name) {
  throw_assert_eq(
    strcmp(FDEvent().get_type_name(), "yield::poll::FDEvent"),
    0
  );
}

TEST(FDEvent, operator_uint16_t) {
  throw_assert_eq(
    static_cast<uint16_t>(FDEvent(POLLIN, static_cast<fd_t>(0))),
    POLLIN
  );
}

TEST(FDEvent, set_events) {
  FDEvent fd_event;
  throw_assert_ne(fd_event.get_events(), POLLOUT);
  fd_event.set_events(POLLOUT);
  throw_assert_eq(fd_event.get_events(), POLLOUT);
}

TEST(FDEvent, set_fd) {
  FDEvent fd_event;
  throw_assert_ne(fd_event.get_fd(), 0);
  fd_event.set_fd(0);
  throw_assert_eq(fd_event.get_fd(), 0);
}
}
}