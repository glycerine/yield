// yield/time.hpp

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

#ifndef _YIELD_TIME_HPP_
#define _YIELD_TIME_HPP_

#include "yield/types.hpp"

namespace yield {
/**
  An interval time with no reference, similar to struct timespec.
  Often used for specifying timeouts.
  Stores the time in nanoseconds internally.
*/
class Time {
public:
  /**
    Constant: the largest value this class can store,
      often used to indicate infinite timeouts.
  */
  const static uint64_t FOREVER = static_cast<uint64_t>(~0);

  /**
    Constant: number of nanoseconds in a millisecond.
  */
  const static uint64_t NS_IN_MS = static_cast<uint64_t>(1000000);

  /**
    Constant: number of nanoseconds in a second.
  */
  const static uint64_t NS_IN_S = static_cast<uint64_t>(1000000000);

  /**
    Constant: number of nanoseconds in a microsecond.
  */
  const static uint64_t NS_IN_US = static_cast<uint64_t>(1000);

public:
  /**
    Construct a Time from a nanosecond value.
    @param ns nanoseconds
  */
  Time(uint64_t ns)
    : _ns(ns) {
  }

  /**
    Construct a Time from a second value, usually an integer literal.
    @param s seconds
  */
  Time(int s)
    : _ns(static_cast<uint32_t>(s) * NS_IN_S) {
  }

  /**
    Construct a Time instance from a second value.
    @param s fractions of seconds
  */
  Time(double s)
    : _ns(static_cast<uint64_t>(s* static_cast<double>(NS_IN_S))) {
  }

  /**
    Copy constructor.
  */
  Time(const Time& time) : _ns(time._ns) { }

#ifndef _WIN32
  /**
    Construct a Time from a struct timespec.
    @param timespec_ the struct timespec
  */
  Time(const timespec& timespec_);

  /**
    Construct a Time from a struct timeval
    @param timeval_ the struct timeval
  */
  Time(const timeval& timeval_);
#endif

public:
  /**
    Return the stored time in fractions of milliseconds.
    @return the stored time in fractions of milliseconds
  */
  double ms() const {
    return static_cast<double>(_ns) / NS_IN_MS;
  }

  /**
    Retrieve the current time. This is not guaranteed to have a specific
      reference time (e.g., the Unix epoch), but may be based on cycle counts
      or or similar.
    @return the current time
  */
  static Time now();

  /**
    Return the stored time in nanoseconds.
    @return the stored time in nanoseconds
  */
  uint64_t ns() const {
    return _ns;
  }

  /**
    Return the stored time in fractions of seconds.
    @return the stored time in fractions of seconds
  */
  double s() const {
    return static_cast<double>(_ns) / NS_IN_S;
  }

  /**
    Return the stored time in fractions of microseconds.
    @return the stored time in fractions of microseconds
  */
  double us() const {
    return static_cast<double>(_ns) / NS_IN_US;
  }

public:
  /**
    Return the stored time in nanoseconds.
    @return the stored time in nanoseconds
  */
  operator uint64_t() const {
    return _ns;
  }

#ifndef _WIN32
  /**
    Set the stored time from a struct timespec.
    @param timespec_ the struct timespec
  */
  Time& operator=(const timespec& timespec_);

  /**
    Cast this Time to a struct timespec.
    @return the struct timespec
  */
  operator timespec() const;

  /**
    Set the stored time from a struct timeval.
    @param timeval_ the struct timeval
  */
  Time& operator=(const struct timeval& timeval_);

  /**
    Cast this Time to a struct timeval.
    @return the struct timeval
  */
  operator timeval() const;
#endif

  /**
    Set the stored time.
    @param time another other Time instance
  */
  Time& operator=(const Time& time) {
    _ns = time._ns;
    return *this;
  }

  /**
    Set the stored time.
    @param ns nanoseconds
  */
  Time& operator=(uint64_t ns) {
    _ns = ns;
    return *this;
  }

  /**
    Add a Time to this Time, returning a new Time.
    @param time Time to add
    @return a new Time instance
  */
  Time operator+(const Time& time) const {
    return Time(_ns + time._ns);
  }

  /**
    Add a nanosecond value to this Time, returning a new Time.
    @param ns nanoseconds to add
    @return a new Time instance
  */
  Time operator+(uint64_t ns) const {
    return Time(_ns + ns);
  }

  /**
    Add a Time to this Time, storing the result in this Time.
    @param time Time to add
    @return *this
  */
  Time& operator+=(const Time& time) {
    _ns += time._ns;
    return *this;
  }

  /**
    Add a nanosecond value to this Time, storing the result in this Time.
    @param ns nanoseconds to add
    @return *this
  */
  Time& operator+=(uint64_t ns) {
    _ns += ns;
    return *this;
  }

  /**
    Subtract a Time from this Time, returning a new Time.
    @param time Time to subtract
    @return a new Time instance
  */
  Time operator-(const Time& time) const {
    if (_ns >= time._ns) {
      return Time(_ns - time._ns);
    } else {
      return Time(static_cast<uint64_t>(0));
    }
  }

  /**
    Subtract a nanosecond value from this Time, returning a new Time.
    @param ns nanoseconds to subtract
    @return a new Time instance
  */
  Time operator-(uint64_t ns) const {
    if (_ns >= ns) {
      return Time(_ns - ns);
    } else {
      return Time(static_cast<uint64_t>(0));
    }
  }

  /**
    Subtract a Time from this Time, storing the result in this Time.
    @param time Time to subtract
    @return *this
  */
  Time& operator-=(const Time& time) {
    _ns = (_ns >= time._ns) ? (_ns - time._ns) : 0;
    return *this;
  }

  /**
    Subtract a nanosecond value from this Time, storing the result in this Time.
    @param ns nanoseconds to subtract
    @return *this
  */
  Time& operator-=(uint64_t ns) {
    if (_ns >= ns) {
      _ns -= ns;
    } else {
      _ns = 0;
    }

    return *this;
  }

  /**
    Multiply this Time by another Time, returning a new Time.
    @param time to add
    @return a new Time instance
  */
  Time operator*(const Time& time) const {
    return Time(_ns * time._ns);
  }

  /**
    Multiply this Time by a nanosecond value, returning a new Time.
    @param ns nanoseconds to multiply
    @return a new Time instance
  */
  Time operator*(uint64_t ns) const {
    return Time(_ns * ns);
  }

  /**
    Multiply this Time by another Time, storing the result in this Time.
    @param time Time to multiply
    @return *this
  */
  Time& operator*=(const Time& time) {
    _ns *= time._ns;
    return *this;
  }

  /**
    Multiply this Time by a nanosecond value, storing the result in this Time.
    @param ns nanoseconds to multiply
    @return *this
  */
  Time& operator*=(const uint64_t ns) {
    _ns *= ns;
    return *this;
  }

  /**
    Compare this Time to another for equality.
    @param time Time to compare this one to
    @return true if the two Times are equal
  */
  bool operator==(const Time& time) const {
    return _ns == time._ns;
  }

  /**
    Compare this Time and a nanosecond value for equality.
    @param ns the nanosecond value to compare this one to
    @return true if the two times are equal
  */
  bool operator==(uint64_t ns) const {
    return _ns == ns;
  }

  /**
    Compare this Time to another for inequality.
    @param time Time to compare this one to
    @return true if the two Times are not equal
  */
  bool operator!=(const Time& time) const {
    return _ns != time._ns;
  }

  /**
    Compare this Time and a nanosecond value for inequality.
    @param ns the nanosecond value to compare this Time to
    @return true if the two times are not equal
  */
  bool operator!=(uint64_t ns) const {
    return _ns != ns;
  }

  /**
    Compare this Time to another.
    @param time Time to compare this one to
    @return true if this time is less than the other
  */
  bool operator<(const Time& time) const {
    return _ns < time._ns;
  }

  /**
    Compare this Time and a nanosecond value.
    @param ns the nanosecond value to compare this Time to
    @return true if this Time is less than the nanosecond value
  */
  bool operator<(uint64_t ns) const {
    return _ns < ns;
  }

  /**
    Compare this Time to another.
    @param time Time to compare this one to
    @return true if this time is less than or equal to the other
  */
  bool operator<=(const Time& time) const {
    return _ns <= time._ns;
  }

  /**
    Compare this Time and a nanosecond value.
    @param ns the nanosecond value to compare this Time to
    @return true if this Time is less than or equal to the nanosecond value
  */
  bool operator<=(uint64_t ns) const {
    return _ns <= ns;
  }

  /**
    Compare this Time to another.
    @param time Time to compare this one to
    @return true if this time is greater than the other
  */
  bool operator>(const Time& time) const {
    return _ns > time._ns;
  }

  /**
    Compare this Time and a nanosecond value.
    @param ns the nanosecond value to compare this Time to
    @return true if this Time is greater than the nanosecond value
  */
  bool operator>(uint64_t ns) const {
    return _ns > ns;
  }

  /**
    Compare this Times and another.
    @param time Time to compare this one to
    @return true if this time is greater than or equal to the other
  */
  bool operator>=(const Time& time) const {
    return _ns >= time._ns;
  }

  /**
    Compare this Time and a nanosecond value.
    @param ns the nanosecond value to compare this Time to
    @return true if this Time is greater than or equal to the nanosecond value
  */
  bool operator>=(uint64_t ns) const {
    return _ns >= ns;
  }

private:
  uint64_t _ns;
};
}

#endif
