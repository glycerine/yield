// yield/thread/performance_counter_set.cpp

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


#if defined(__sun)
#include "sunos/cpc_performance_counter_set.hpp"
#elif defined(YIELD_HAVE_UNIX_PAPI)
#include "unix/papi_performance_counter_set.hpp"
#endif
#include "yield/thread/performance_counter_set.hpp"


namespace yield {
namespace thread {
#if defined(__sun) || defined(YIELD_HAVE_UNIX_PAPI)
PerformanceCounterSet::PerformanceCounterSet() {
#if defined(__sun)
  pimpl = new sunos::CPCPerformanceCounterSet;
#elif defined(YIELD_HAVE_UNIX_PAPI)
  pimpl = new unix::PAPIPerformanceCounterSet;
#endif
}

PerformanceCounterSet::~PerformanceCounterSet() {
  delete pimpl;
}

bool PerformanceCounterSet::add(Event event) {
  return pimpl->add(event);
}

bool PerformanceCounterSet::add(const char* event) {
  return pimpl->add(event);
}

void PerformanceCounterSet::start_counting() {
  pimpl->start_counting();
}

void PerformanceCounterSet::stop_counting(uint64_t* counts) {
  pimpl->stop_counting();
}
#endif
}
}
