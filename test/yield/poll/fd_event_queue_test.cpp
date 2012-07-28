// fd_event_queue_test.cpp

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

#include "../event_queue_test.hpp"
#include "yield/auto_object.hpp"
#include "yield/exception.hpp"
#include "yield/poll/fd_event.hpp"
#include "yield/poll/fd_event_queue.hpp"
#include "gtest/gtest.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace yield {
namespace poll {
INSTANTIATE_TYPED_TEST_CASE_P(FDEventQueue, EventQueueTest, FDEventQueue);

class FDEventQueueTest : public ::testing::Test {
public:
  FDEventQueueTest() {
#ifdef _WIN32
    fds[0] = fds[1] = INVALID_HANDLE_VALUE;
#else
    fds[0] = fds[1] = -1;
#endif
  }

  // ::testing::Test
  void SetUp() {
#ifdef _WIN32
    if (!CreatePipe(&fds[0], &fds[1], NULL, 0))
#else
    if (pipe(fds) != 0)
#endif
      throw Exception();
  }

  void TearDown() {
#ifdef _WIN32
    if (fds[0] != INVALID_HANDLE_VALUE) {
      CloseHandle(fds[0]);
    }
    if (fds[1] != INVALID_HANDLE_VALUE) {
      CloseHandle(fds[1]);
    }
#else
    close(fds[0]);
    close(fds[1]);
#endif
  }

protected:
  fd_t get_read_fd() const {
    return fds[0];
  }

  fd_t get_write_fd() const {
    return fds[1];
  }

  void signal_pipe() {
    char m = 'm';
#ifdef _WIN32
    DWORD _;
    WriteFile(get_write_fd(), &m, 1, &_, NULL);
#else
    write(get_write_fd(), &m, 1);
#endif
  }

private:
  fd_t fds[2];
};


TEST_F(FDEventQueueTest, associate) {
  if (!FDEventQueue().associate(get_read_fd(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }
}

TEST_F(FDEventQueueTest, associate_change) {
  FDEventQueue fd_event_queue;

  if (!fd_event_queue.associate(get_read_fd(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }

  if (!fd_event_queue.associate(get_read_fd(), FDEvent::TYPE_WRITE_READY)) {
    throw Exception();
  }
}

TEST_F(FDEventQueueTest, associate_duplicate) {
  FDEventQueue fd_event_queue;

  if (!fd_event_queue.associate(get_read_fd(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }

  if (!fd_event_queue.associate(get_read_fd(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }
}

TEST_F(FDEventQueueTest, associate_two) {
  FDEventQueue fd_event_queue;

  if (!fd_event_queue.associate(get_read_fd(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }

  if (!fd_event_queue.associate(get_write_fd(), FDEvent::TYPE_WRITE_READY)) {
    throw Exception();
  }
}

TEST_F(FDEventQueueTest, associate_zero) {
  FDEventQueue fd_event_queue;

  if (!fd_event_queue.associate(get_read_fd(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }

  if (!fd_event_queue.associate(get_read_fd(), 0)) {
    throw Exception();
  }

  signal_pipe();

  Event* event = fd_event_queue.timeddequeue(0);
  ASSERT_EQ(event, static_cast<Event*>(NULL));
}

TEST(FDEventQueue, constructor) {
  FDEventQueue();
}

TEST_F(FDEventQueueTest, dequeue_FDEvent) {
  FDEventQueue fd_event_queue;

  if (!fd_event_queue.associate(get_read_fd(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }

  signal_pipe();

  auto_Object<Event> event = fd_event_queue.dequeue();
}

TEST_F(FDEventQueueTest, dequeue_writable_FDEvent) {
  FDEventQueue fd_event_queue;

  if (!fd_event_queue.associate(get_write_fd(), FDEvent::TYPE_WRITE_READY)) {
    throw Exception();
  }

  auto_Object<FDEvent> fd_event
  = Object::cast<FDEvent>(fd_event_queue.dequeue());
  ASSERT_EQ(fd_event->get_fd(), get_write_fd());
  ASSERT_EQ(fd_event->get_type(), FDEvent::TYPE_WRITE_READY);
}

TEST(FDEventQueue, destructor) {
  FDEventQueue();
}

TEST_F(FDEventQueueTest, dissociate) {
  FDEventQueue fd_event_queue;

  if (!fd_event_queue.associate(get_read_fd(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }

  if (!fd_event_queue.dissociate(get_read_fd())) {
    throw Exception();
  }

  signal_pipe();

  Event* event = fd_event_queue.timeddequeue(0);
  ASSERT_EQ(event, static_cast<Event*>(NULL));

  if (!fd_event_queue.associate(get_read_fd(), FDEvent::TYPE_READ_READY)) {
    throw Exception();  // associate after dissociate should succeed
  }

  if (!fd_event_queue.dissociate(get_read_fd())) {
    throw Exception();
  }

  ASSERT_FALSE(fd_event_queue.dissociate(get_read_fd()));
}
}
}
