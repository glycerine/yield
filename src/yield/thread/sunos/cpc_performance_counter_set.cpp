// yield/thread/sunos/cpc_performance_counter_set.cpp

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


#include "cpc_performance_counter_set.hpp"
#include "yield/exception.hpp"


namespace yield {
namespace thread {
namespace sunos {
PerformanceCounterSet::PerformanceCounterSet() {
  cpc = cpc_open(CPC_VER_CURRENT);
  if (cpc == NULL)
    throw Exception();

  cpc_set = cpc_set_create(cpc);
  if (cpc_set == NULL) {
    int errno_temp = errno;
    cpc_close(cpc);
    throw Exception(errno_temp);
  }

  cpc_start_buf = NULL;
}

PerformanceCounterSet::~PerformanceCounterSet() {
  cpc_set_destroy(cpc, cpc_set);
  cpc_close(cpc);
}

bool PerformanceCounterSet::add(Event event) {
  switch (event) {
  case EVENT_L1_DCM:
    return add("DC_miss");
  case EVENT_L2_DCM:
    return add("DC_refill_from_system");
  case EVENT_L2_ICM:
    return add("IC_refill_from_system");
  default:
    DebugBreak();
    return false;
  }
}

bool PerformanceCounterSet::add(const char* event) {
  int event_index
  = cpc_set_add_request(cpc, cpc_set, event, 0, CPC_COUNT_USER, 0, NULL);

  if (event_index != -1) {
    event_indices.push_back(event_index);
    return true;
  }

  return false;
}

void PerformanceCounterSet::start_counting() {
  if (start_cpc_buf == NULL)
    start_cpc_buf = cpc_buf_create(cpc, cpc_set);
  cpc_bind_curlwp(cpc, cpc_set, 0);
  cpc_set_sample(cpc, cpc_set, start_cpc_buf);
}

void PerformanceCounterSet::stop_counting(uint64_t* counts) {
  cpc_buf_t* stop_cpc_buf = cpc_buf_create(cpc, cpc_set);
  cpc_set_sample(cpc, cpc_set, stop_cpc_buf);

  cpc_buf_t* diff_cpc_buf = cpc_buf_create(cpc, cpc_set);
  cpc_buf_sub(cpc, diff_cpc_buf, stop_cpc_buf, start_cpc_buf);

  for
  (
    vector<int>::size_type event_index_i = 0;
    event_index_i < event_indices.size();
    event_index_i++
  ) {
    cpc_buf_get
    (
      cpc,
      diff_cpc_buf,
      event_indices[event_index_i],
      &counts[event_index_i]
    );
  }

  cpc_unbind(cpc, cpc_set);
}
}
}
}
