// yield/thread/unix/papi_performance_counter_set.cpp

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


#include "papi_performance_counter_set.hpp"
#include "yield/exception.hpp"

#ifdef YIELD_HAVE_UNIX_PAPI
#include <papi.h>
#endif


namespace yield {
namespace thread {
namespace unix {
#ifdef YIELD_HAVE_UNIX_PAPI
PAPIPerformanceCounterSet::PAPIPerformanceCounterSet() {
  if (PAPI_library_init(PAPI_VER_CURRENT) == PAPI_VER_CURRENT) {
    if (PAPI_thread_init(pthread_self) == PAPI_OK) {
      int eventset = PAPI_NULL;
      if (PAPI_create_eventset(&eventset) == PAPI_OK)
        return;
    }
  }

  throw Exception();
}

PAPIPerformanceCounterSet::~PAPIPerformanceCounterSet() {
  PAPI_cleanup_eventset(eventset);
  PAPI_destroy_eventset(&eventset);
}

bool PAPIPerformanceCounterSet::add(Event event) {
  switch (event) {
  case EVENT_L1_DCM:
    return add("PAPI_l1_dcm");
  case EVENT_L2_DCM:
    return add("PAPI_l2_dcm");
  case EVENT_L2_ICM:
    return add("PAPI_l2_icm");
  default:
    DebugBreak();
    return false;
  }
}

bool PAPIPerformanceCounterSet::add(const char* event) {
  int event_code;
  if
  (
    PAPI_event_name_to_code
    (
      const_cast<char*>(event),
      &event_code
    ) == PAPI_OK
  )
    return PAPI_add_event(eventset, event_code) == PAPI_OK;
  else
    return false;
}

void PAPIPerformanceCounterSet::start_counting() {
  PAPI_start(eventset);
}

void PAPIPerformanceCounterSet::stop_counting(uint64_t* counts) {
  PAPI_stop(eventset, reinterpret_cast<long long int*>(counts));
}
#endif
}
}
}
