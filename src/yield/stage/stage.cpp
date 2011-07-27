// yield/stage/stage.cpp

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
#include "yield/exception.hpp"
#include "yield/time.hpp"
#include "yield/stage/stage.hpp"
#include "yield/queue/synchronized_event_queue.hpp"

namespace yield {
namespace stage {
Stage::Stage(YO_NEW_REF EventHandler& event_handler)
  : event_handler(&event_handler),
    event_queue(*new ::yield::queue::SynchronizedEventQueue) {
  init();
}

Stage::Stage(
  YO_NEW_REF EventHandler& event_handler,
  YO_NEW_REF EventQueue& event_queue
)
  : event_handler(&event_handler),
    event_queue(event_queue) {
  init();
}

Stage::Stage(YO_NEW_REF EventQueue& event_queue)
  : event_handler(NULL), event_queue(event_queue) {
  init();
}

Stage::~Stage() {
  EventQueue::dec_ref(event_queue);
  EventHandler::dec_ref(event_handler);
}

void Stage::enqueue(YO_NEW_REF Event& event) {
  event_queue_length++;
  event_queue_arrival_count++;

  if (event_queue.enqueue(event)) {
    return;
  } else {
    //cerr << event_handler.get_type_name() <<
    //  ": event queue full, stopping.";
    debug_break();
  }
}

void Stage::init() {
  event_queue_arrival_count = 0;
  event_queue_length = 0;
}

void Stage::service(YO_NEW_REF Event& event) {
  event_handler->handle(event);
}

void Stage::visit() {
  Event& event = event_queue.dequeue();
  event_queue_length--;

  Time service_time_start(Time::now());

  service(event);

  Time service_time(Time::now() - service_time_start);
}

bool Stage::visit(const Time& timeout) {
  Event* event = event_queue.timeddequeue(timeout);

  if (event != NULL) {
    event_queue_length--;

    Time service_time_start(Time::now());

    service(*event);

    Time service_time(Time::now() - service_time_start);

    return true;
  } else {
    return false;
  }
}
}
}
