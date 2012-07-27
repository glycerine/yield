// yield/stage/stage.hpp

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

#ifndef _YIELD_STAGE_STAGE_HPP_
#define _YIELD_STAGE_STAGE_HPP_

#include "yield/event.hpp"
#include "yield/event_handler.hpp"
#include "yield/time.hpp"

namespace yield {
class EventQueue;

namespace stage {
class Stage : public EventHandler {
public:
  class ShutdownEvent : public Event {
  public:
    const static uint32_t TYPE_ID = 1809111020UL;

    // yield::Object
    uint32_t get_type_id() const {
      return TYPE_ID;
    }

    const char* get_type_name() const {
      return "yield::stage::Stage::ShutdownEvent";
    }
  };

public:
  Stage(YO_NEW_REF EventHandler&);
  Stage(YO_NEW_REF EventHandler&, YO_NEW_REF EventQueue&);
  ~Stage();

public:
  double get_arrival_rate_s() const {
    return arrival_rate_s;
  }

  double get_rho() const {
    return rho;
  }

  double get_service_rate_s() const {
    return service_rate_s;
  }

  void visit(); // Blocking
  bool visit(const Time& timeout);

public:
  // yield::Object
  Stage& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::EventHandler
  void handle(YO_NEW_REF Event& event) {
    enqueue(event);
  }

protected:
  Stage(YO_NEW_REF EventQueue&);

  EventQueue& get_event_queue() {
    return event_queue;
  }

private:
  void enqueue(YO_NEW_REF Event& event);
  void init();
  virtual void service(YO_NEW_REF Event& event);

private:
  double arrival_rate_s;
  EventHandler* event_handler;
  EventQueue& event_queue;
  uint32_t event_queue_arrival_count, event_queue_length;
  double rho;
  double service_rate_s;
  // Sampler<uint64_t, 1024, Mutex> service_times;
};
};
};

#endif
