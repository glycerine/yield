// stage_scheduler_test.hpp

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

#ifndef _YIELD_STAGE_STAGE_SCHEDULER_TEST_HPP_
#define _YIELD_STAGE_STAGE_SCHEDULER_TEST_HPP_

#include "test_event.hpp"
#include "test_event_handler.hpp"
#include "yield/auto_object.hpp"
#include "yield/stage/stage.hpp"
#include "yield/stage/stage_scheduler.hpp"
#include "yield/thread/thread.hpp"
#include "yunit.hpp"


namespace yield {
namespace stage {
template <class StageSchedulerType>
class StageSchedulerScheduleTest : public yunit::Test {
public:
  // yunit::Test
  void run() {
    auto_Object<TestEventHandler> event_handler = new TestEventHandler;
    auto_Object<Stage> stage = new Stage(event_handler->inc_ref());
    auto_Object<StageScheduler> stage_scheduler = new StageSchedulerType;
    stage_scheduler->schedule(*stage);
    stage->handle(*new TestEvent);
    while (event_handler->get_seen_events_count() < 1) {
      yield::thread::Thread::sleep(0.1);
    }
  }
};


template <class StageSchedulerType>
class StageSchedulerTestSuite : public yunit::TestSuite {
public:
  StageSchedulerTestSuite() {
    add(
      "StageScheduler::schedule",
      new StageSchedulerScheduleTest<StageSchedulerType>
    );
  }
};
}
}

#endif
