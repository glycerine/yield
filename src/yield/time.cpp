// yield/time.cpp

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

#if defined(_WIN32)
#include <Windows.h> // For FILETIME
#elif defined(__MACH__)
#include <sys/time.h> // For gettimeofday
#else
#include <time.h>
#endif

namespace yield {
#ifndef _WIN32
const uint64_t Time::FOREVER;
const uint64_t Time::NS_IN_MS;
const uint64_t Time::NS_IN_S;
const uint64_t Time::NS_IN_US;
#endif

Time Time::now() {
#if defined(__MACH__)
  timeval tv;
  gettimeofday(&tv, NULL);
  return Time(tv);
#elif defined(_WIN32)
  FILETIME file_time;
  GetSystemTimeAsFileTime(&file_time);
  ULARGE_INTEGER file_time_combined;
  file_time_combined.LowPart = file_time.dwLowDateTime;
  file_time_combined.HighPart = file_time.dwHighDateTime;
  return Time(file_time_combined.QuadPart * 100);
#else
  // POSIX real time
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return Time(ts);
#endif
}

#ifndef _WIN32
Time::Time(const timespec& ts) {
  *this = ts;
}

Time& Time::operator=(const timespec& ts) {
  _ns = ts.tv_sec * NS_IN_S + ts.tv_nsec;
  return *this;
}

Time::operator timespec() const {
  timespec ts;
  ts.tv_sec = _ns / NS_IN_S;
  ts.tv_nsec = _ns % NS_IN_S;
  return ts;
}

Time::Time(const timeval& tv) {
  *this = tv;
}

Time& Time::operator=(const timeval& tv) {
  _ns = tv.tv_sec * NS_IN_S
        +
        static_cast<uint64_t>(tv.tv_usec * NS_IN_US);
  return *this;
}

Time::operator timeval() const {
  timeval tv;
  tv.tv_sec = static_cast<time_t>(_ns / NS_IN_S);
  tv.tv_usec = (_ns % NS_IN_S) / NS_IN_US;
  return tv;
}
#endif
}
