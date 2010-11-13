// yield/time.hpp

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


#ifndef _YIELD_TIME_HPP_
#define _YIELD_TIME_HPP_


#include "yield/types.hpp"

#ifdef __MACH__
  struct mach_timespec;
  typedef mach_timespec mach_timespec_t;
#endif


namespace yield
{
  class Time
  {
  public:
    const static uint64_t NS_IN_US = static_cast<uint64_t>( 1000 );
    const static uint64_t NS_IN_MS = static_cast<uint64_t>( 1000000 );
    const static uint64_t NS_IN_S  = static_cast<uint64_t>( 1000000000 );
    const static uint64_t FOREVER  = static_cast<uint64_t>( ~0 );

  public:
    Time( uint64_t ns ) : _ns( ns ) { }
    Time( int s ) : _ns( s * NS_IN_S ) { }
    Time( double s )
      : _ns( static_cast<uint64_t>( s * static_cast<double>( NS_IN_S ) ) )
    { }

    Time( const Time& t ) : _ns( t._ns ) { }

    #ifndef _WIN32
      Time( const timespec& );
      Time( const timeval& );
    #endif

    double ms() const { return static_cast<double>( _ns ) / NS_IN_MS; }
    static Time now();
    uint64_t ns() const { return _ns; }
    double s() const { return static_cast<double>( _ns ) / NS_IN_S; }
    double us() const { return static_cast<double>( _ns ) / NS_IN_US; }
    operator uint64_t() const { return _ns; }

    #ifdef __MACH__
      operator mach_timespec_t() const;
    #endif
    #ifndef _WIN32
      Time& operator=( const timespec& );
      operator timespec() const;
      Time& operator=( const struct timeval& );
      operator timeval() const;
    #endif

    Time& operator=( const Time& t ) { _ns = t._ns; return *this; }
    Time& operator=( uint64_t ns ) { _ns = ns; return *this; }

    Time operator+( const Time& t ) const { return Time( _ns + t._ns ); }
    Time operator+( uint64_t ns ) const { return Time( _ns + ns ); }
    Time& operator+=( const Time& t ) { _ns += t._ns; return *this; }
    Time& operator+=( uint64_t ns ) { _ns += ns; return *this; }

    Time operator-( const Time& t ) const
    {
      if ( _ns >= t._ns )
        return Time( _ns - t._ns );
      else
        return Time( static_cast<uint64_t>( 0 ) );
    }

    Time operator-( uint64_t ns ) const
    {
      if ( _ns >= ns )
        return Time( _ns - ns );
      else
        return Time( static_cast<uint64_t>( 0 ) );
    }

    Time& operator-=( const Time& t )
    {
      _ns = ( _ns >= t._ns ) ? ( _ns - t._ns ) : 0;
      return *this;
    }

    Time& operator-=( uint64_t ns )
    {
      if ( _ns >= ns )
        _ns -= ns;
      else
        _ns = 0;

      return *this;
    }

    Time operator*( const Time& t ) const { return Time( _ns * t._ns ); }
    Time operator*( uint64_t ns ) const { return Time( _ns * ns ); }
    Time& operator*=( const Time& t ) { _ns *= t._ns; return *this; }
    Time& operator*=( const uint64_t ns ) { _ns *= ns; return *this; }

    bool operator==( const Time& t ) const { return _ns == t._ns; }
    bool operator==( uint64_t ns ) const { return _ns == ns; }

    bool operator!=( const Time& t ) const { return _ns != t._ns; }
    bool operator!=( uint64_t ns ) const { return _ns != ns; }

    bool operator<( const Time& t ) const { return _ns < t._ns; }
    bool operator<( uint64_t ns ) const { return _ns < ns; }

    bool operator<=( const Time& t ) const { return _ns <= t._ns; }
    bool operator<=( uint64_t ns ) const { return _ns <= ns; }

    bool operator>( const Time& t ) const { return _ns > t._ns; }
    bool operator>( uint64_t ns ) const { return _ns > ns; }

    bool operator>=( const Time& t ) const { return _ns >= t._ns; }
    bool operator>=( uint64_t ns ) const { return _ns >= ns; }

  private:
    uint64_t _ns; // Not relative to any epoch
  };
}


#endif
