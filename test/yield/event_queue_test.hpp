// event_queue_test.hpp

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

#ifndef _YIELD_EVENT_QUEUE_TEST_HPP_
#define _YIELD_EVENT_QUEUE_TEST_HPP_

#include "yield/auto_object.hpp"
#include "yield/event.hpp"
#include "yield/event_queue.hpp"
#include "gtest/gtest.h"

namespace yield {
template <class TypeParam>
class EventQueueTest : public ::testing::Test {
protected:
  class MockEvent : public Event {
  public:
    uint32_t get_type_id() const {
      return 0;
    }
  };
};

TYPED_TEST_CASE_P(EventQueueTest);

TYPED_TEST_P(EventQueueTest, dequeue) {
  auto_Object<Event> event = new MockEvent;
  TypeParam event_queue;

  bool enqueue_ret = event_queue.enqueue(event->inc_ref());
  ASSERT_TRUE(enqueue_ret);

  auto_Object<Event> dequeued_event = event_queue.dequeue();
  ASSERT_EQ(event, dequeued_event);

  Event* null_event = static_cast<EventQueue&>(event_queue).trydequeue();
  ASSERT_EQ(null_event, static_cast<Event*>(NULL));
}

TYPED_TEST_P(EventQueueTest, timeddequeue) {
  auto_Object<Event> event = new MockEvent;
  TypeParam event_queue;

  bool enqueue_ret = event_queue.enqueue(event->inc_ref());
  ASSERT_TRUE(enqueue_ret);

  auto_Object<Event> dequeued_event = event_queue.timeddequeue(1.0);
  ASSERT_EQ(event, dequeued_event);

  Event* null_event = event_queue.timeddequeue(1.0);
  ASSERT_EQ(null_event, static_cast<Event*>(NULL));
}

TYPED_TEST_P(EventQueueTest, trydequeue) {
  auto_Object<Event> event = new MockEvent;
  TypeParam event_queue;

  bool enqueue_ret = event_queue.enqueue(event->inc_ref());
  ASSERT_TRUE(enqueue_ret);

  auto_Object<Event> dequeued_event
  = static_cast<EventQueue&>(event_queue).trydequeue();
  ASSERT_EQ(event, dequeued_event);

  Event* null_event
  = static_cast<EventQueue&>(event_queue).trydequeue();
  ASSERT_EQ(null_event, static_cast<Event*>(NULL));
}

REGISTER_TYPED_TEST_CASE_P(EventQueueTest, dequeue, timeddequeue, trydequeue);
}

#endif
