// socket_event_queue_test.cpp

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
#include "yield/poll/fd_event_queue.hpp"
#include "yield/sockets/stream_socket_pair.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace poll {
using yield::sockets::StreamSocketPair;

INSTANTIATE_TYPED_TEST_CASE_P(SocketEventQueue, EventQueueTest, FDEventQueue);

TEST(SocketEventQueue, associate) {
  StreamSocketPair sockets;
  if (!FDEventQueue(true).associate(sockets.first(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }
}

TEST(SocketEventQueue, associate_change) {
  StreamSocketPair sockets;
  FDEventQueue socket_event_queue(true);

  if (!
      socket_event_queue.associate(
        sockets.first(),
        FDEvent::TYPE_READ_READY | FDEvent::TYPE_WRITE_READY
      )
     ) {
    throw Exception();
  }

  if (!socket_event_queue.associate(sockets.first(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }

  sockets.second().send("m", 1, 0);

  auto_Object<FDEvent> socket_event
  = Object::cast<FDEvent>(socket_event_queue.dequeue());
}

TEST(SocketEventQueue, associate_two) {
  StreamSocketPair sockets;
  FDEventQueue socket_event_queue(true);

  if (!socket_event_queue.associate(sockets.first(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }

  if (!socket_event_queue.associate(sockets.second(), FDEvent::TYPE_WRITE_READY)) {
    throw Exception();
  }
}

TEST(SocketEventQueue, dissociate) {
  StreamSocketPair sockets;
  FDEventQueue socket_event_queue(true);

  if (!socket_event_queue.associate(sockets.first(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }

  if (!socket_event_queue.dissociate(sockets.first())) {
    throw Exception();
  }

  sockets.first().send("m", 1, 0);

  Event* event = socket_event_queue.timeddequeue(0);
  ASSERT_EQ(event, static_cast<Event*>(NULL));

  if (!socket_event_queue.associate(sockets.first(), FDEvent::TYPE_READ_READY)) {
    throw Exception();  // associate after dissociate should succeed
  }
}

TEST(SocketEventQueue, dissociate_two) {
  StreamSocketPair sockets;
  FDEventQueue socket_event_queue(true);

  if (!socket_event_queue.associate(sockets.first(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }
  if (!socket_event_queue.associate(sockets.second(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }

  if (!socket_event_queue.dissociate(sockets.first())) {
    throw Exception();
  }

  sockets.first().send("m", 1, 0);
  sockets.second().send("m", 1, 0);

  auto_Object<FDEvent> socket_event
  = Object::cast<FDEvent>(socket_event_queue.dequeue());
  ASSERT_EQ(socket_event->get_fd(), sockets.second());
  ASSERT_EQ(socket_event->get_type(), FDEvent::TYPE_READ_READY);
  char m;
  if (socket_event->get_fd() == sockets.first()) {
    sockets.first().recv(&m, 1, 0);
  } else {
    sockets.second().recv(&m, 1, 0);
  }

  Event* event = socket_event_queue.timeddequeue(0);
  ASSERT_EQ(event, static_cast<Event*>(NULL));
}

TEST(SocketEventQueue, dequeue_SocketEvent) {
  StreamSocketPair sockets;
  FDEventQueue socket_event_queue(true);

  if (!socket_event_queue.associate(sockets.first(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }

  sockets.second().send("m", 1, 0);

  auto_Object<FDEvent> socket_event
  = Object::cast<FDEvent>(socket_event_queue.dequeue());
}

TEST(SocketEventQueue, dequeue_two_SocketEvents) {
  StreamSocketPair sockets;
  FDEventQueue socket_event_queue(true);

  if (!socket_event_queue.associate(sockets.first(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }
  if (!socket_event_queue.associate(sockets.second(), FDEvent::TYPE_READ_READY)) {
    throw Exception();
  }

  sockets.first().send("m", 1, 0);
  sockets.second().send("m", 1, 0);

  for (uint8_t socket_i = 0; socket_i < 2; socket_i++) {
    auto_Object<FDEvent> socket_event
    = Object::cast<FDEvent>(socket_event_queue.dequeue());

    ASSERT_TRUE(
      socket_event->get_fd() == sockets.first()
      ||
      socket_event->get_fd() == sockets.second()
    );
    ASSERT_EQ(socket_event->get_type(), FDEvent::TYPE_READ_READY);
  }
}

TEST(SocketEventQueue, dequeue_want_send_SocketEvent) {
  StreamSocketPair sockets;
  FDEventQueue socket_event_queue(true);

  if (!socket_event_queue.associate(sockets.first(), FDEvent::TYPE_WRITE_READY)) {
    throw Exception();
  }

  auto_Object<FDEvent> socket_event
  = Object::cast<FDEvent>(socket_event_queue.dequeue());
  ASSERT_EQ(socket_event->get_fd(), sockets.first());
  ASSERT_EQ(socket_event->get_type(), FDEvent::TYPE_WRITE_READY);
}
}
}
