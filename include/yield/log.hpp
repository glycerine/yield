// yield/log.hpp

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

#ifndef _YIELD_LOG_HPP_
#define _YIELD_LOG_HPP_

#include "yield/object.hpp"

#include <sstream> // For std::ostringstream

namespace yield {
class Buffer;

class Log : public Object {
public:
  class Level {
  public:
    Level(const char* level);
    Level(const string& level);
    Level(uint8_t level);
    Level(const char* level_string, uint8_t level_uint8);
    Level(const Level& other);

    operator const string& () const {
      return level_string;
    }
    operator const char* () const {
      return level_string.c_str();
    }
    inline operator uint8_t() const {
      return level_uint8;
    }

    bool operator<(const Level& other) const;
    bool operator<=(const Level& other) const;
    bool operator==(const Level& other) const;
    bool operator>(const Level& other) const;
    bool operator>=(const Level& other) const;

  private:
    void init(const char* level, size_t level_len);

  private:
    string level_string;
    uint8_t level_uint8;
  };


  class Stream {
  public:
    Stream(Log& log, Level);
    Stream(const Stream& other);
    ~Stream();

    template <typename T>
    Stream& operator<<(T t) {
      if (level <= log.get_level())
        oss << t;
      return *this;
    }

  private:
    Log& log;
    Level level;
    std::ostringstream oss;
  };

public:
  // Adapted from syslog levels
  static Level EMERG;
  static Level ALERT;
  static Level CRIT;
  static Level ERR;
  static Level WARNING;
  static Level NOTICE;
  static Level INFO;
  static Level DEBUG;

public:
  virtual ~Log() { }

public:
  const Level& get_level() const {
    return level;
  }

  Stream get_stream() {
    return Stream(inc_ref(), level);
  }

  Stream get_stream(Level level) {
    return Stream(inc_ref(), level);
  }

public:
  static YO_NEW_REF Log& open(std::ostream&, const Level& = ERR);

public:
  void write(const char*, const Level&);
  void write(const string&, const Level&);
  void write(const void*, size_t, const Level&);
  void write(const uint8_t*, size_t, const Level&);
  void write(const char*, size_t, const Level&);
  void write(const Buffer&, const Level&);

public:
  // yield::Object
  Log& inc_ref() {
    return Object::inc_ref(*this);
  }

protected:
  Log(const Level& level);

  virtual void write(const char*, size_t) = 0;

private:
  void write(const void*, size_t);
  void write(const uint8_t*, size_t);

private:
  Level level;
};
}

#endif
