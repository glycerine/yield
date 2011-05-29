// socket_event_queue_test.cpp

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

#include "../../event_queue_test.hpp"
#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/exception.hpp"
#include "yield/sockets/stream_socket_pair.hpp"
#include "yield/sockets/poll/socket_event.hpp"
#include "yield/sockets/poll/socket_event_queue.hpp"
#include "yunit.hpp"

TEST_SUITE_EX(
  SocketEventQueue,
  yield::EventQueueTestSuite<yield::sockets::poll::SocketEventQueue>
);

namespace yield {
namespace sockets {
namespace poll {
TEST(SocketEventQueue, associate) {
  StreamSocketPair sockets;
  if (!SocketEventQueue().associate(sockets.first(), SocketEvent::TYPE_READ_READY))
    throw Exception();
}

TEST(SocketEventQueue, associate_change) {
  StreamSocketPair sockets;
  SocketEventQueue socket_event_queue;

  if (!
    socket_event_queue.associate(
      sockets.first(),
      SocketEvent::TYPE_READ_READY | SocketEvent::TYPE_WRITE_READY
    )
  )
    throw Exception();

  if (!socket_event_queue.associate(sockets.first(), SocketEvent::TYPE_READ_READY))
    throw Exception();

  sockets.second().send("m", 1, 0);

  auto_Object<SocketEvent> socket_event
  = Object::cast<SocketEvent>(socket_event_queue.dequeue());
}

TEST(SocketEventQueue, associate_two) {
  StreamSocketPair sockets;
  SocketEventQueue socket_event_queue;

  if (!socket_event_queue.associate(sockets.first(), SocketEvent::TYPE_READ_READY))
    throw Exception();

  if (!socket_event_queue.associate(sockets.second(), SocketEvent::TYPE_WRITE_READY))
    throw Exception();
}

TEST(SocketEventQueue, dissociate) {
  StreamSocketPair sockets;
  SocketEventQueue socket_event_queue;

  if (!socket_event_queue.associate(sockets.first(), SocketEvent::TYPE_READ_READY))
    throw Exception();

  if (!socket_event_queue.dissociate(sockets.first()))
    throw Exception();

  sockets.first().send("m", 1, 0);

  Event* event = socket_event_queue.timeddequeue(0);
  throw_assert_eq(event, NULL);

  if (!socket_event_queue.associate(sockets.first(), SocketEvent::TYPE_READ_READY))
    throw Exception(); // associate after dissociate should succeed
}

TEST(SocketEventQueue, dissociate_two) {
  StreamSocketPair sockets;
  SocketEventQueue socket_event_queue;

  if (!socket_event_queue.associate(sockets.first(), SocketEvent::TYPE_READ_READY))
    throw Exception();
  if (!socket_event_queue.associate(sockets.second(), SocketEvent::TYPE_READ_READY))
    throw Exception();

  if (!socket_event_queue.dissociate(sockets.first()))
    throw Exception();

  sockets.first().send("m", 1, 0);
  sockets.second().send("m", 1, 0);

  auto_Object<SocketEvent> socket_event
    = Object::cast<SocketEvent>(socket_event_queue.dequeue());
  throw_assert_eq(socket_event->get_fd(), sockets.second());
  throw_assert_eq(socket_event->get_type(), SocketEvent::TYPE_READ_READY);
  char m;
  if (reinterpret_cast<fd_t>(socket_event->get_fd()) == sockets.first())
    sockets.first().recv(&m, 1, 0);
  else
    sockets.second().recv(&m, 1, 0);

  Event* event = socket_event_queue.timeddequeue(0);
  throw_assert_eq(event, NULL);
}

TEST(SocketEventQueue, dequeue_SocketEvent) {
  StreamSocketPair sockets;
  SocketEventQueue socket_event_queue;

  if (!socket_event_queue.associate(sockets.first(), SocketEvent::TYPE_READ_READY))
    throw Exception();

  sockets.second().send("m", 1, 0);

  auto_Object<SocketEvent> socket_event
  = Object::cast<SocketEvent>(socket_event_queue.dequeue());
}

TEST(SocketEventQueue, dequeue_two_SocketEvents) {
  StreamSocketPair sockets;
  SocketEventQueue socket_event_queue;

  if (!socket_event_queue.associate(sockets.first(), SocketEvent::TYPE_READ_READY))
    throw Exception();
  if (!socket_event_queue.associate(sockets.second(), SocketEvent::TYPE_READ_READY))
    throw Exception();

  sockets.first().send("m", 1, 0);
  sockets.second().send("m", 1, 0);

  for (uint8_t socket_i = 0; socket_i < 2; socket_i++) {
    auto_Object<SocketEvent> socket_event
    = Object::cast<SocketEvent>(socket_event_queue.dequeue());

    throw_assert(
      socket_event->get_fd() == sockets.first()
      ||
      socket_event->get_fd() == sockets.second()
    );
    throw_assert_eq(socket_event->get_type(), SocketEvent::TYPE_READ_READY);
  }
}

TEST(SocketEventQueue, dequeue_want_send_SocketEvent) {
  StreamSocketPair sockets;
  SocketEventQueue socket_event_queue;

  if (!socket_event_queue.associate(sockets.first(), SocketEvent::TYPE_WRITE_READY))
    throw Exception();

  auto_Object<SocketEvent> socket_event
  = Object::cast<SocketEvent>(socket_event_queue.dequeue());
  throw_assert_eq(socket_event->get_fd(), sockets.first());
  throw_assert_eq(socket_event->get_type(), SocketEvent::TYPE_WRITE_READY);
}
}
}
}