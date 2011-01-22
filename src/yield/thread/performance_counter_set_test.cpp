// yield/thread/performance_counter_set_test.cpp

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


#include "yield/thread/performance_counter_set.hpp"
#include "yunit.hpp"


#if defined(__sun)
#define TEST_EVENT "IC_fetch"
#elif defined(YIELD_HAVE_UNIX_PAPI)
#define TEST_EVENT "PAPI_TOT_INS"
#endif


TEST_SUITE( PerformanceCounterSet );

namespace yield {
namespace thread {
#if defined(__sun) || defined(YIELD_HAVE_UNIX_PAPI)
TEST( PerformanceCounterSet, add ) {
  auto_Object<PerformanceCounterSet> perfctrs( PerformanceCounterSet::create() );
  throw_assert( perfctrs->add( TEST_EVENT ) );
}

TEST( PerformanceCounterSet, start_counting ) {
  auto_Object<PerformanceCounterSet> perfctrs( PerformanceCounterSet::create() );
  perfctrs->add( TEST_EVENT );
  perfctrs->start_counting();
}

TEST( PerformanceCounterSet, stop_counting ) {
  auto_Object<PerformanceCounterSet> perfctrs( PerformanceCounterSet::create() );
  perfctrs->add( TEST_EVENT );
  perfctrs->start_counting();
  for ( int i = 0; i < 100; i++ ) continue;
  uint64_t counts[1];
  perfctrs->stop_counting( counts );
  throw_assert_gt( counts[0], 0 );
}
#endif
}
}
