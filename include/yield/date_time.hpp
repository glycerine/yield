// yield/date_time.hpp

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
class DateTime {
public:
  const static DateTime INVALID_DATE_TIME;

public:
  DateTime( uint64_t unix_date_time_ns )
    : unix_date_time_ns( unix_date_time_ns )
  { }

  DateTime( double unix_date_time_s )
    : unix_date_time_ns
    (
      static_cast<uint64_t>
      (
        unix_date_time_s * static_cast<double>( Time::NS_IN_S )
      )
    )
  { }

  explicit DateTime( time_t unix_date_time_s )
    : unix_date_time_ns( unix_date_time_s * Time::NS_IN_S )
  { }

  DateTime( const DateTime& other )
    : unix_date_time_ns( other.unix_date_time_ns )
  { }

#ifdef _WIN32
  DateTime( const FILETIME& );
  DateTime( const FILETIME* );
  DateTime( const SYSTEMTIME&, bool local = true );
#else
  DateTime( const timeval& );
  DateTime( const tm&, bool local = true );
#endif

  DateTime
  (
    int tm_sec, // seconds after the minute  0-61*
    int tm_min, // minutes after the hour  0-59
    int tm_hour, //  hours since midnight 0-23
    int tm_mday, //  day of the month 1-31
    int tm_mon, // months since January  0-11
    int tm_year, //  years since 1900
    bool local = true
  );

#ifdef _WIN32
  SYSTEMTIME as_local_SYSTEMTIME() const;
  SYSTEMTIME as_utc_SYSTEMTIME() const;
#else
  tm as_local_tm() const;
  tm as_utc_tm() const;
#endif

  double as_unix_date_time_ms() const {
    return static_cast<double>( unix_date_time_ns ) /
           static_cast<double>( Time::NS_IN_MS );
  }

  uint64_t as_unix_date_time_ns() const {
    return unix_date_time_ns;
  }

  double as_unix_date_time_s() const {
    return static_cast<double>( unix_date_time_ns ) /
           static_cast<double>( Time::NS_IN_S );
  }

  double as_unix_date_time_us() const {
    return static_cast<double>( unix_date_time_ns ) /
           static_cast<double>( Time::NS_IN_US );
  }

  static DateTime now();

#ifdef _WIN32
  operator FILETIME() const;
  operator SYSTEMTIME() const; // as_local_SYSTEMTIME
#else
  operator timeval() const;
  operator tm() const; // as_local_tm
#endif

  operator time_t() const {
    return static_cast<time_t>( as_unix_date_time_s() );
  }

  operator string() const;

  DateTime operator+( const DateTime& other ) const {
    return DateTime( unix_date_time_ns + other.unix_date_time_ns );
  }

  Time operator-( const DateTime& other ) const {
    if ( unix_date_time_ns >= other.unix_date_time_ns )
      return Time( unix_date_time_ns - other.unix_date_time_ns );
    else
      return Time( static_cast<uint64_t>( 0 ) );
  }

  bool operator==( const DateTime& other ) const {
    return unix_date_time_ns == other.unix_date_time_ns;
  }

  bool operator!=( const DateTime& other ) const {
    return unix_date_time_ns != other.unix_date_time_ns;
  }

  bool operator<( const DateTime& other ) const {
    return unix_date_time_ns < other.unix_date_time_ns;
  }

  bool operator<=( const DateTime& other ) const {
    return unix_date_time_ns <= other.unix_date_time_ns;
  }

  bool operator>( const DateTime& other ) const {
    return unix_date_time_ns > other.unix_date_time_ns;
  }

  bool operator>=( const DateTime& other ) const {
    return unix_date_time_ns >= other.unix_date_time_ns;
  }

private:
#ifdef _WIN32
  void init( const FILETIME& );
  void init( const SYSTEMTIME&, bool local );
#else
  void init( tm&, bool local );
#endif

private:
  // The time is stored internally as a Unix epoch time, i.e.
  // nanoseconds since January 1, 1970
  uint64_t unix_date_time_ns;
};

std::ostream& operator<<( std::ostream&, const DateTime& );
}


#endif
