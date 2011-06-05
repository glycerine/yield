// yield/stage/seda_stage_scheduler.cpp

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
#include "yield/time.hpp"
#include "yield/stage/seda_stage_scheduler.hpp"
#include "yield/stage/stage.hpp"
#include "yield/thread/runnable.hpp"
#include "yield/thread/thread.hpp"

namespace yield {
namespace stage {
using yield::thread::Thread;

class SEDAStageScheduler::SEDAStage : public ::yield::thread::Runnable {
public:
  SEDAStage(Stage& stage)
    : stage(stage.inc_ref()) {
    should_run = true;
  }

  void stop() {
    should_run = false;
    stage.handle(*new Stage::ShutdownEvent);
  }

  // yield::thread::Runnable
  void run() {
    while (should_run)
      stage.visit();
  }

private:
  bool should_run;
  Stage& stage;
};


SEDAStageScheduler::~SEDAStageScheduler() {
  for (
    vector<Thread*>::iterator thread_i = threads.begin();
    thread_i != threads.end();
    ++thread_i
  ) {
    static_cast<SEDAStage*>((*thread_i)->get_runnable())->stop();
    (*thread_i)->join();
    Thread::dec_ref(**thread_i);
  }
}

void
SEDAStageScheduler::schedule(
  Stage& stage,
  ConcurrencyLevel concurrency_level
) {
  auto_Object<SEDAStage> seda_stage = new SEDAStage(stage);
  for (int16_t thread_i = 0; thread_i < concurrency_level; thread_i++)
    threads.push_back(new Thread(seda_stage->inc_ref()));
}
}
}
