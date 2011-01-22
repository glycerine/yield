// yield/date_time.cpp

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


#include "yield/date_time.hpp"
#include "yield/time.hpp"

#ifdef _WIN32
#include <Windows.h> // For FILETIME and SYSTEMTIME
#else
#include <stdio.h> // For snprintf
#include <sys/time.h> // For gettimeofday
#endif


namespace yield {
const DateTime DateTime::INVALID_DATE_TIME = static_cast<uint64_t>(~0);


#ifdef _WIN32
DateTime::DateTime(const FILETIME& file_time) {
  init(file_time);
}

DateTime::DateTime(const FILETIME* file_time) {
  if (file_time != NULL)
    init(*file_time);
  else
    unix_date_time_ns = 0;
}

DateTime::DateTime(const SYSTEMTIME& system_time, bool local) {
  init(system_time, local);
}
#else
DateTime::DateTime(const timeval& tv) {
  unix_date_time_ns = tv.tv_sec * Time::NS_IN_S
                      +
                      static_cast<uint64_t>(tv.tv_usec * Time::NS_IN_US);
}

DateTime::DateTime(const tm& tm_, bool local) {
  tm tm_copy(tm_);
  init(tm_copy, local);
}
#endif

DateTime::DateTime
(
  int tm_sec, // seconds after the minute  0-61*
  int tm_min, // minutes after the hour  0-59
  int tm_hour, //  hours since midnight 0-23
  int tm_mday, //  day of the month 1-31
  int tm_mon, // months since January  0-11
  int tm_year, //  years since 1900
  bool local
) {
#ifdef _WIN32
  SYSTEMTIME system_time;
  system_time.wMilliseconds = 0;
  system_time.wSecond = static_cast<WORD>(tm_sec);
  system_time.wMinute = static_cast<WORD>(tm_min);
  system_time.wHour = static_cast<WORD>(tm_hour);
  system_time.wDay = static_cast<WORD>(tm_mday);
  system_time.wDayOfWeek = 0;
  system_time.wMonth = static_cast<WORD>(tm_mon + 1);
  system_time.wYear = static_cast<WORD>(1900 + tm_year);
  init(system_time, local);
#else
  tm tm_;
  tm_.tm_sec = tm_sec;
  tm_.tm_min = tm_min;
  tm_.tm_hour = tm_hour;
  tm_.tm_mday = tm_mday;
  tm_.tm_mon = tm_mon;
  tm_.tm_year = tm_year;
  tm_.tm_wday = 0; // Will be filled in by mktime
  tm_.tm_yday = 0; // Will be filled in by mktime
  mktime(&tm_);
  init(tm_, local);
#endif
}

#ifdef _WIN32
SYSTEMTIME DateTime::as_local_SYSTEMTIME() const {
  SYSTEMTIME utc_system_time = this->as_utc_SYSTEMTIME();

  TIME_ZONE_INFORMATION time_zone_information;
  GetTimeZoneInformation(&time_zone_information);
  SYSTEMTIME local_system_time;
  SystemTimeToTzSpecificLocalTime
  (
    &time_zone_information,
    &utc_system_time,
    &local_system_time
  );
  return local_system_time;
}

SYSTEMTIME DateTime::as_utc_SYSTEMTIME() const {
  FILETIME file_time = *this;
  SYSTEMTIME utc_system_time;
  FileTimeToSystemTime(&file_time, &utc_system_time);
  return utc_system_time;
}
#else
tm DateTime::as_local_tm() const {
  time_t unix_date_time_s = *this;
  tm tm_;
  localtime_r(&unix_date_time_s, &tm_);
  return tm_;
}

DateTime::operator tm() const {
  return as_local_tm();
}

tm DateTime::as_utc_tm() const {
  time_t unix_date_time_s = *this;
  tm tm_;
  gmtime_r(&unix_date_time_s, &tm_);
  return tm_;
}
#endif

#ifdef _WIN32
void DateTime::init(const FILETIME& file_time) {
  ULARGE_INTEGER file_time_combined;
  file_time_combined.LowPart = file_time.dwLowDateTime;
  file_time_combined.HighPart = file_time.dwHighDateTime;
  // Subtract the number of 100-ns intervals between
  // January 1, 1601 and January 1, 1970
  file_time_combined.QuadPart -= 116444736000000000;
  file_time_combined.QuadPart *= 100; // Into nanoseconds
  unix_date_time_ns = file_time_combined.QuadPart;
}

void DateTime::init(const SYSTEMTIME& system_time, bool local) {
  FILETIME file_time;

  if (local) {
    TIME_ZONE_INFORMATION time_zone_information;
    GetTimeZoneInformation(&time_zone_information);
    SYSTEMTIME utc_system_time;
    TzSpecificLocalTimeToSystemTime
    (
      &time_zone_information,
      &system_time,
      &utc_system_time
    );
    SystemTimeToFileTime(&utc_system_time, &file_time);
  } else
    SystemTimeToFileTime(&system_time, &file_time);

  init(file_time);
}

DateTime::operator FILETIME() const {
  uint64_t unix_date_time_ns = this->unix_date_time_ns;
  // Add the difference in nanoseconds between
  // January 1, 1601 (start of the Windows epoch) and
  // January 1, 1970 (start of the Unix epoch)
  unix_date_time_ns += 11644473600000000000;
  uint64_t unix_time_100_ns_intervals = unix_date_time_ns / 100;
  FILETIME file_time;
  file_time.dwLowDateTime
  = static_cast<DWORD>(unix_time_100_ns_intervals);
  file_time.dwHighDateTime = unix_time_100_ns_intervals >> 32;
  return file_time;
}

DateTime::operator SYSTEMTIME() const {
  return as_local_SYSTEMTIME();
}
#else
void DateTime::init(tm& tm_, bool local) {
  time_t unix_date_time_s;

  if (local)
    unix_date_time_s = mktime(&tm_);
  else
    unix_date_time_s = timegm(&tm_);

  if (unix_date_time_s != static_cast<time_t>(-1))
    unix_date_time_ns = unix_date_time_s * Time::NS_IN_S;
  else
    *this = INVALID_DATE_TIME;
}

DateTime::operator timeval() const {
  timeval tv;
  tv.tv_sec = static_cast<time_t>(unix_date_time_ns / Time::NS_IN_S);
  tv.tv_usec = (unix_date_time_ns % Time::NS_IN_S) / Time::NS_IN_US;
  return tv;
}
#endif

DateTime DateTime::now() {
#if defined(_WIN32)
  FILETIME file_time;
  GetSystemTimeAsFileTime(&file_time);
  return DateTime(file_time);
#else
  timeval tv;
  gettimeofday(&tv, NULL);
  return DateTime(tv);
#endif
}

DateTime::operator string() const {
  char iso_date_time[64];

#ifdef _WIN32
  SYSTEMTIME local_system_time = this->as_local_SYSTEMTIME();

  _snprintf_s
  (
    iso_date_time,
    64,
    _TRUNCATE,
    "%04d-%02d-%02dT%02d:%02d:%02d.000Z",
    local_system_time.wYear,
    local_system_time.wMonth,
    local_system_time.wDay,
    local_system_time.wHour,
    local_system_time.wMinute,
    local_system_time.wSecond
  );
#else
  tm local_tm = as_local_tm();

  snprintf
  (
    iso_date_time,
    64,
    "%04d-%02d-%02dT%02d:%02d:%02d.000Z",
    local_tm.tm_year + 1900,
    local_tm.tm_mon + 1,
    local_tm.tm_mday,
    local_tm.tm_hour,
    local_tm.tm_min,
    local_tm.tm_sec
  );
#endif

  return iso_date_time;
}

std::ostream& operator<<(std::ostream& os, const DateTime& date_time) {
  os << static_cast<string>(date_time);
  return os;
}
}
