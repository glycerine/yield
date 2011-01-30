// yield/thread/performance_counter_set.hpp

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

#ifndef _YIELD_THREAD_PERFORMANCE_COUNTER_SET_HPP_
#define _YIELD_THREAD_PERFORMANCE_COUNTER_SET_HPP_

#include "yield/types.hpp"


namespace yield {
namespace thread {
#if defined(__sun)
namespace sunos {
class CPCPerformanceCounterSet;
}
#elif defined(YIELD_HAVE_UNIX_PAPI)
namespace linux {
class PAPIPerformanceCounterSet;
}
#endif


#if defined(__sun) || defined(YIELD_HAVE_UNIX_PAPI)
class PerformanceCounterSet {
public:
  enum Event {
    EVENT_L1_DCM, // L1 data cache miss
    EVENT_L2_DCM, // L2 data cache miss
    EVENT_L2_ICM // L2 instruction cache miss
  };

public:
  PerformanceCounterSet();
  ~PerformanceCounterSet();

  bool add(Event event);
  bool add(const char* event);
  void start_counting();
  void stop_counting(uint64_t* counts);

private:
#if defined(__sun)
  sunos::CPCPerformanceCounterSet* pimpl;
#elif defined(YIELD_HAVE_UNIX_PAPI)
  unix::PAPIPerformanceCounterSet* pimpl;
#endif
};
#endif
}
}

#endif
