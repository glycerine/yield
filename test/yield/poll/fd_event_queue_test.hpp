// fd_event_queue_test.hpp

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

#ifndef _YIELD_POLL_FD_EVENT_QUEUE_TEST_HPP_
#define _YIELD_POLL_FD_EVENT_QUEUE_TEST_HPP_

#include "yield/auto_object.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/poll/fd_event_queue.hpp"
#include "yunit.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace yield {
namespace poll {
template <class FDEventQueueType>
class FDEventQueueTest : public yunit::Test {
public:
  FDEventQueueTest() {
    fd_event_queue = NULL;
#ifdef _WIN32
    fds[0] = fds[1] = INVALID_HANDLE_VALUE;
#else
    fds[0] = fds[1] = -1;
#endif
  }

  // yunit::Test
  void setup() {
    fd_event_queue = new FDEventQueueType;

#ifdef _WIN32
    if (CreatePipe(&fds[0], &fds[1], NULL, 0))
#else
    if (pipe(fds) != -1)
#endif
      return;
    else {
      delete fd_event_queue;
      throw Exception();
    }
  }

  void teardown() {
    delete fd_event_queue;

#ifdef _WIN32
    if (fds[0] != INVALID_HANDLE_VALUE)
      CloseHandle(fds[0]);
    if (fds[1] != INVALID_HANDLE_VALUE)
      CloseHandle(fds[1]);
#else
    close(fds[0]);
    close(fds[1]);
#endif
  }

protected:
  FDEventQueueType& get_fd_event_queue() const {
    return *fd_event_queue;
  }

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
  FDEventQueueType* fd_event_queue;
  fd_t fds[2];
};


template <class FDEventQueueType>
class FDEventQueueAssociateOneTest
    : public FDEventQueueTest<FDEventQueue> {
public:
  void run() {
    if (!get_fd_event_queue().associate(get_read_fd(), POLLIN))
      throw Exception();
  }
};


template <class FDEventQueueType>
class FDEventQueueAssociateTwoTest
    : public FDEventQueueTest<FDEventQueue> {
public:
  void run() {
    if (!get_fd_event_queue().associate(get_read_fd(), POLLIN))
      throw Exception();

    if (!get_fd_event_queue().associate(get_write_fd(), POLLOUT))
      throw Exception();
  }
};


template <class FDEventQueueType>
class FDEventQueueDissociateOneTest
    : public FDEventQueueTest<FDEventQueue> {
public:
  void run() {
    if (!get_fd_event_queue().associate(get_read_fd(), POLLIN))
      throw Exception();

    if (!get_fd_event_queue().dissociate(get_read_fd()))
      throw Exception();

    this->signal_pipe();

    Event* event = get_fd_event_queue().trydequeue();
    throw_assert_eq(event, NULL);

    if (!get_fd_event_queue().associate(get_read_fd(), POLLIN))
      throw Exception(); // associate after dissociate should succeed
  }
};


template <class FDEventQueueType>
class FDEventQueuePollOneBlockingTest
    : public FDEventQueueTest<FDEventQueue> {
public:
  void run() {
    if (!get_fd_event_queue().associate(get_read_fd(), POLLIN))
      throw Exception();

    this->signal_pipe();

    auto_Object<Event> event = get_fd_event_queue().dequeue();
  }
};


template <class FDEventQueueType>
class FDEventQueuePollOneTimedTest
    : public FDEventQueueTest<FDEventQueue> {
public:
  void run() {
    if (!get_fd_event_queue().associate(get_read_fd(), POLLIN))
      throw Exception();

    this->signal_pipe();

    Time start_time(Time::now());
    auto_Object<FDEvent> fd_event
    = object_cast<FDEvent>(get_fd_event_queue().dequeue(10.0));
    Time elapsed_time(Time::now() - start_time);
    throw_assert_le(elapsed_time, Time(10.0));
  }
};



template <class FDEventQueueType>
class FDEventQueueToggleOneTest : public FDEventQueueTest<FDEventQueue> {
public:
  void run() {
    if (!get_fd_event_queue().associate(get_read_fd(), POLLIN))
      throw Exception();

    if (!get_fd_event_queue().associate(get_read_fd(), 0))
      throw Exception();

    this->signal_pipe();

    Event* event = get_fd_event_queue().trydequeue();
    throw_assert_eq(event, NULL);
  }
};


template <class FDEventQueueType>
class FDEventQueueWantWriteTest : public FDEventQueueTest<FDEventQueue> {
public:
  void run() {
    if (!get_fd_event_queue().associate(get_write_fd(), POLLOUT))
      throw Exception();

    auto_Object<FDEvent> fd_event
    = object_cast<FDEvent>(get_fd_event_queue().dequeue());
    throw_assert_eq(fd_event->get_fd(), get_write_fd());
    throw_assert
    (
      (*fd_event & POLLOUT) == POLLOUT
      ||
      (*fd_event & POLLWRNORM) == POLLWRNORM
    );
  }
};


template <class FDEventQueueType>
class FDEventQueueTestSuite : public yunit::TestSuite {
public:
  FDEventQueueTestSuite() {
    add(
      "FDEventQueue::associate( fd )",
      new FDEventQueueAssociateOneTest<FDEventQueue>
    );

    add(
      "FDEventQueue::associate( fd ) x 2",
      new FDEventQueueAssociateTwoTest<FDEventQueue>
    );

    add(
      "FDEventQueue::dissociate( fd )",
      new FDEventQueueDissociateOneTest<FDEventQueue>
    );

    add(
      "FDEventQueue::poll()",
      new FDEventQueuePollOneBlockingTest<FDEventQueue>
    );

    add(
      "FDEventQueue::poll( timeout )",
      new FDEventQueuePollOneTimedTest<FDEventQueue>
    );

    add(
      "FDEventQueue::toggle",
      new FDEventQueueToggleOneTest<FDEventQueue>
    );

    add(
      "FDEventQueue::want_write",
      new FDEventQueueWantWriteTest<FDEventQueue>
    );
  }
};
}
}

#endif
