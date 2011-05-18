// yield/date_time.hpp

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

#ifndef _YIELD_DATE_TIME_HPP_
#define _YIELD_DATE_TIME_HPP_

#include "yield/time.hpp"

#include <ostream>

#ifdef _WIN32
struct _FILETIME;
typedef _FILETIME FILETIME;
struct _SYSTEMTIME;
typedef _SYSTEMTIME SYSTEMTIME;
#else
struct tm;
struct timeval;
#endif

namespace yield {
/**
  An absolute date and time.
  Stores the date-time internally as a nanosecond offset from the Unix epoch
    (1/1/1970).
*/
class DateTime {
public:
  const static DateTime INVALID_DATE_TIME;

public:
  /**
    Construct a DateTime from a nanosecond offset from the Unix epoch (1/1/1970).
    @param unix_date_time_time_ns nanosecond offset from the Unix epoch
  */
  DateTime(uint64_t unix_date_time_ns)
    : unix_date_time_ns(unix_date_time_ns)
  { }

  /**
    Construct a DateTime from a second offset from the Unix epoch (1/1/1970).
    @param unix_date_time_s fractions of seconds offset from the Unix epoch
  */
  DateTime(double unix_date_time_s)
    : unix_date_time_ns(
      static_cast<uint64_t>(
        unix_date_time_s* static_cast<double>(Time::NS_IN_S)
      )
    )
  { }

  /**
    Construct a DateTime from a seconds offset from the Unix epoch (1/1/1970).
    @param unix_date_time_s seconds offset from the Unix epoch
  */
  explicit DateTime(time_t unix_date_time_s)
    : unix_date_time_ns(unix_date_time_s* Time::NS_IN_S)
  { }

  /**
    Copy constructor.
    @param other DateTime to copy
  */
  DateTime(const DateTime& other)
    : unix_date_time_ns(other.unix_date_time_ns)
  { }

#ifdef _WIN32
  /**
    Construct a DateTime from a struct FILETIME.
    @param file_time the FILETIME
  */
  DateTime(const FILETIME& file_time);

  /**
    Construct a DateTime from a struct FILETIME.
    @param file_time the FILETIME, checked for NULL.
  */
  DateTime(const FILETIME* file_time);

  /**
    Construct a DateTime from a struct SYSTEMTIME.
    @param system_time the SYSTEMTIME
    @param local true if the SYSTEMTIME is a local datetime, false if UTC
  */
  DateTime(const SYSTEMTIME& system_time, bool local = true);
#else
  /**
    Construct a DateTime from a struct timeval.
    @param timeval_ the timeval
  */
  DateTime(const timeval& timeval_);

  /**
    Construct a DateTime from a struct tm.
    @param tm_ the tm
    @param local true if the tm is a local datetime, false if UTC
  */
  DateTime(const tm& tm_, bool local = true);
#endif

  /**
    Construct a DateTime from struct tm components.
    @param tm_sec seconds after the minute (0-61)
    @param tm_min minutes after the hour (0-59)
    @param tm_hour hours since midnight (0-23)
    @param tm_mday day of the month (1-31)
    @param tm_mon months since January (0-11)
    @param local true if the components are a local datetime, false if UTC
  */
  DateTime(
    int tm_sec, // 
    int tm_min, // 
    int tm_hour, //  
    int tm_mday, //  
    int tm_mon, // 
    int tm_year, //  years since 1900
    bool local = true
  );

#ifdef _WIN32
  /**
    Return the datetime as a local SYSTEMTIME.
    @return a local SYSTEMTIME
  */
  SYSTEMTIME as_local_SYSTEMTIME() const;

  /**
    Return the datetime as a UTC SYSTEMTIME.
    @return a UTC SYSTEMTIME
  */
  SYSTEMTIME as_utc_SYSTEMTIME() const;
#else
  /**
    Retrurn the datetime as a local struct tm.
    @return a local tm
  */
  tm as_local_tm() const;

  /**
    Return the datetime as a UTC struct tm.
    @return a UTC tm
  */
  tm as_utc_tm() const;
#endif

  /**
    Return the datetime as a millisecond offset from the Unix epoch (1/1/1970).
    @return a millisecond offset from the Unix epoch
  */
  double as_unix_date_time_ms() const {
    return static_cast<double>(unix_date_time_ns) /
           static_cast<double>(Time::NS_IN_MS);
  }

  /**
    Return the datetime as a nanosecond offset from the Unix epoch (1/1/1970).
    @return a nanosecond offset from the Unix epoch
  */
  uint64_t as_unix_date_time_ns() const {
    return unix_date_time_ns;
  }

  /**
    Return the datetime as a fractions of seconds offset from the
      Unix epoch (1/1/1970).
    @return a fractions of seconds offset from the Unix epoch
  */
  double as_unix_date_time_s() const {
    return static_cast<double>(unix_date_time_ns) /
           static_cast<double>(Time::NS_IN_S);
  }

  /**
    Return the datetime as a microsecond offset from the Unix epoch (1/1/1970).
    @return a microsecond offset from the Unix epoch
  */
  double as_unix_date_time_us() const {
    return static_cast<double>(unix_date_time_ns) /
           static_cast<double>(Time::NS_IN_US);
  }

  /**
    Retrieve the current datetime.
    @return the current datetime
  */
  static DateTime now();

#ifdef _WIN32
  /**
    Return the datetime as a struct FILETIME.
    @return a FILETIME
  */
  operator FILETIME() const;

  /**
    Return the datetime as a local struct SYSTEMTIME.
    @return a local SYSTEMTIME
  */
  operator SYSTEMTIME() const;
#else
  /**
    Return the datetime as a struct timeval.
    @return a timeval
  */
  operator timeval() const;

  /**
    Return the datetime as a local struct tm.
    @return a local tm
  */
  operator tm() const;
#endif

  /**
    Return the datetime as a seconds offset from the Unix epoch (1/1/1970).
    @return seconds offset from the Unix epoch
  */
  operator time_t() const {
    return static_cast<time_t>(as_unix_date_time_s());
  }

  /**
    Return a string representation of the datetime.
    @return a string representation of the datetime
  */
  operator string() const;

  /**
    Add this DateTime to another, returning a new DateTime.
    @param other the DateTime to add to this one
    @return the sum of the DateTimes
  */
  DateTime operator+(const DateTime& other) const {
    return DateTime(unix_date_time_ns + other.unix_date_time_ns);
  }

  /**
    Subtract another DateTime from this one, returning a new DateTime.
    @param other the DateTime to subtract from this one
    @return the resulting DateTime
  */
  Time operator-(const DateTime& other) const {
    if (unix_date_time_ns >= other.unix_date_time_ns)
      return Time(unix_date_time_ns - other.unix_date_time_ns);
    else
      return Time(static_cast<uint64_t>(0));
  }

  /**
    Compare this DateTime to another for equality.
    @param other the other DateTime to compare this one to
    @return true if the DateTimes are equal
  */
  bool operator==(const DateTime& other) const {
    return unix_date_time_ns == other.unix_date_time_ns;
  }

  /**
    Compare this DateTime to another for inequality.
    @param other the other DateTime to compare this one to
    @return true if the DateTimes are not equal
  */
  bool operator!=(const DateTime& other) const {
    return unix_date_time_ns != other.unix_date_time_ns;
  }

  /**
    Compare this DateTime to another.
    @param other the other DateTime to compare this one to
    @return true if this DateTime is earlier than the other
  */
  bool operator<(const DateTime& other) const {
    return unix_date_time_ns < other.unix_date_time_ns;
  }

  /**
    Compare this DateTime to another.
    @param other the other DateTime to compare this one to
    @return true if this DateTime is earlier or the same as the other
  */
  bool operator<=(const DateTime& other) const {
    return unix_date_time_ns <= other.unix_date_time_ns;
  }

  /**
    Compare this DateTime to another.
    @param other the other DateTime to compare this one to
    @return true if this DateTime is later than the other
  */
  bool operator>(const DateTime& other) const {
    return unix_date_time_ns > other.unix_date_time_ns;
  }

  /**
    Compare this DateTime to another.
    @param other the other DateTime to compare this one to
    @return true if this DateTime is later or the same as the other
  */
  bool operator>=(const DateTime& other) const {
    return unix_date_time_ns >= other.unix_date_time_ns;
  }

private:
#ifdef _WIN32
  void init(const FILETIME&);
  void init(const SYSTEMTIME&, bool local);
#else
  void init(tm&, bool local);
#endif

private:
  uint64_t unix_date_time_ns;
};

std::ostream& operator<<(std::ostream&, const DateTime&);
}

#endif
