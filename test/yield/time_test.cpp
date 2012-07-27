// time_test.cpp

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

#include "yield/time.hpp"
#include "gtest/gtest.h"

namespace yield {
#ifndef _WIN32
TEST(Time, as_timespec) {
  Time timeout(10.0);
  timespec as_timespec = timeout;
  ASSERT_EQ(static_cast<long>(timeout.s()), as_timespec.tv_sec);
  ASSERT_LT(as_timespec.tv_nsec, static_cast<long>(Time::NS_IN_S));
}

TEST(Time, as_timeval) {
  Time timeout(10.0);
  timeval as_timeval = timeout;
  ASSERT_EQ(static_cast<long>(timeout.s()), as_timeval.tv_sec);
  ASSERT_LT
  (
    static_cast<uint64_t>(as_timeval.tv_usec),
    Time::NS_IN_MS
  );
}
#endif

TEST(Time, copy) {
  Time timeout(10.0);
  Time timeout_copy(timeout);
  ASSERT_EQ(timeout.ns(), timeout_copy.ns());
}

#ifndef _WIN32
TEST(Time, from_timespec) {
  Time timeout(10.0);
  timespec as_timespec = timeout;
  Time timeout_copy(as_timespec);
  ASSERT_EQ(timeout, timeout_copy);
}

TEST(Time, from_timeval) {
  Time timeout(10.0);
  timeval tv = timeout;
  Time timeout_copy(tv);
  ASSERT_EQ(timeout, timeout_copy);
}
#endif

TEST(Time, now) {
  Time time1(Time::now());
  ASSERT_GT(static_cast<uint64_t>(time1), 0);
  for (uint32_t i = 0; i < UINT32_MAX / 16; i++) {
    ;
  }
  Time time2(Time::now());
  ASSERT_GT(time2, time1);
}
}
