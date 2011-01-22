// yield/stage/polling_stage_scheduler.cpp

// Copyright (c) 2010 Minor Gordon
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


#include "yield/stage/polling_stage_scheduler.hpp"
#include "yield/stage/stage.hpp"


namespace yield {
namespace stage {
using yield::thread::Thread;


PollingStageScheduler::~PollingStageScheduler() {
  for
  (
    vector<Thread*>::iterator thread_i = threads.begin();
    thread_i != threads.end();
    ++thread_i
  ) {
    static_cast<StagePoller*>( ( *thread_i )->get_runnable() )
    ->stop();
    ( *thread_i )->join();
    delete *thread_i;
  }
}

void
PollingStageScheduler::schedule
(
  Stage& stage,
  ConcurrencyLevel concurrency_level
) {
  for ( uint16_t thread_i = 0; thread_i < concurrency_level; thread_i++ ) {
    if ( thread_i < threads.size() ) {
      Thread* thread = threads[thread_i % threads.size()];
      StagePoller* stage_poller
      = static_cast<StagePoller*>( thread->get_runnable() );
      stage_poller->schedule( stage );
    } else
      threads.push_back( new Thread( createStagePoller( stage ) ) );
  }
}


PollingStageScheduler::StagePoller::StagePoller( Stage& first_stage ) {
  stages.push_back( &first_stage.inc_ref() );
}

PollingStageScheduler::StagePoller::~StagePoller() {
  for
  (
    vector<Stage*>::iterator stage_i = stages.begin();
    stage_i != stages.end();
    ++stage_i
  )
    Stage::dec_ref( **stage_i );
}

vector<Stage*>& PollingStageScheduler::StagePoller::get_stages() {
  Stage* new_stage = this->new_stage.trydequeue();
  while ( new_stage != NULL ) {
    stages.push_back( reinterpret_cast<Stage*>( new_stage ) );
    new_stage = this->new_stage.trydequeue();
  }

  return stages;
}

void PollingStageScheduler::StagePoller::schedule( Stage& stage ) {
  stage.inc_ref();
  while ( !new_stage.enqueue( stage ) )
    ;
}
}
}
