// yield/event_queue.hpp

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

#ifndef _YIELD_EVENT_QUEUE_HPP_
#define _YIELD_EVENT_QUEUE_HPP_

#include "yield/event_handler.hpp"
#include "yield/time.hpp"


namespace yield {
/**
  Abstract base class for event queues in the event-driven concurrency subsystem.
*/
class EventQueue : public EventHandler {
public:
  /**
    Empty virtual destructor.
  */
  virtual ~EventQueue() { }

  /**
    Blocking dequeue. Always returns a new reference to an Event.
    @return a new reference to an Event.
  */
  virtual YO_NEW_REF Event& dequeue() {
    Event* event;
    do {
      event = timeddequeue(Time::FOREVER);
    } while (event == NULL);
    return *event;
  }

  /**
    Enqueue a new reference to an Event.
    Will almost always return true. The exceptions are when a memory queue is full
      and when special enqueues (e.g., AIO) fail at the system call level.
    @param event the new Event reference to enqueue
    @return true if the enqueue succeeded.
  */
  virtual bool enqueue(YO_NEW_REF Event& event) = 0;

  /**
    Timed dequeue.
    Blocks for the specified timeout or until an Event is available.
    If the timeout expires and the queue is still empty, returns NULL.
    @param timeout the time to wait for new Events
    @return a new reference to an Event or NULL
  */
  virtual YO_NEW_REF Event* timeddequeue(const Time& timeout) = 0;

  /**
    Non-blocking dequeue.
    Returns a new reference to an Event or NULL if the queue is empty.
    This method is guaranteed not to block.
    @return a new reference to an Event or NULL.
  */
  virtual YO_NEW_REF Event* trydequeue() {
    return timeddequeue(0);
  }

public:
  // yield::Object
  EventQueue& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::EventHandler
  void handle(YO_NEW_REF Event& event) {
    enqueue(event);
  }
};
}

#endif
