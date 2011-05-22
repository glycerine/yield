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

/**
  Abstract base class for log implementations (files, console, etc.)
*/
class Log : public Object {
public:
  /**
    Describes log levels. Adapted from syslog.
  */
  class Level {
  public:
    static Level EMERG;
    static Level ALERT;
    static Level CRIT;
    static Level ERR;
    static Level WARNING;
    static Level NOTICE;
    static Level INFO;
    static Level DEBUG;

  public:
    /**
      Return a string representation of this level.
      @return a string representation of this level
    */
    operator const char* () const {
      return level_string;
    }

    /**
      Return the integer rank of this level.
      @return the integer rank of this level
    */
    inline operator uint8_t() const {
      return level_uint8;
    }

  public:
    /**
      Compare this level to another.
      @param other the other level to compare this one to
      @return true if this level is less than the other
    */
    bool operator<(const Level& other) const;

    /**
      Compare this level to another.
      @param other the other level to compare this one to
      @return true if this level is less than or equal to the other
    */
    bool operator<=(const Level& other) const;

    /**
      Compare this level to another.
      @param other the other level to compare this one to
      @return true if this level is equal to the other
    */
    bool operator==(const Level& other) const;

    /**
      Compare this level to another.
      @param other the other level to compare this one to
      @return true if this level is greater than the other
    */
    bool operator>(const Level& other) const;

    /**
      Compare this level to another.
      @param other the other level to compare this one to
      @return true if this level is greater than or equal to the other
    */
    bool operator>=(const Level& other) const;

  private:
    Level(const char* level_string, uint8_t level_uint8);

  private:
    const char* level_string;
    uint8_t level_uint8;
  };

public:
  /**
    A log stream that acts like a std::ostream.
    Will write(..., Level) to the log on destruction.
  */
  class Stream : public std::ostream {
  public:
    Stream(Log& log, Level);
    Stream(const Stream& other);
    ~Stream();

  private:
    std::stringbuf buf;
    Log& log;
    Level level;
  };

public:
  /**
    Empty virtual destructor.
  */
  virtual ~Log() { }

public:
  /**
    Get the level of this log, i.e. the level at which write(..., Level)'s to
      the log will actually be written.
    @return the level of this log
  */
  const Level& get_level() const {
    return level;
  }

  /**
    Get a new Stream on this log with the log's default level.
    @return a new Stream on this log with the log's default level
  */
  Stream get_stream() {
    return Stream(inc_ref(), level);
  }

  /**
    Get a new Stream on this log with the the specified level.
    @param level level of the new stream
    @return a new Stream on this log with the specified level
  */
  Stream get_stream(Level level) {
    return Stream(inc_ref(), level);
  }

public:
  /**
    Wrap a std::ostream with a Log
    @param os std::ostream to wrap
    @param level level of the new Log
    @return a new reference to the constructed Log
  */
  static YO_NEW_REF Log&
  open(
    std::ostream& os,
    const Level& level = Level::DEBUG
  );

public:
  /**
    Write a string message to the log at the specified level.
    @param message message to write
    @param level level of the message
  */
  void write(const char* message, const Level& level);

  /**
    Write a string message to the log at the specified level.
    @param message message to write
    @param level level of the message
  */
  void write(const string& message, const Level& level);

  /**
    Write a Buffer message to the log at the specified level.
    Replaces unprintable characters.
    @param message message to write
    @param level level of the message
  */
  void write(const Buffer& message, const Level& level);

  /**
    Write arbitrary bytes to the log at the specified level.
    Replaces unprintable characters.
    @param message message to write
    @param message_len length of message, in bytes
    @param level level of the message
  */
  void write(const void* message, size_t message_len, const Level& level);

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

private:
  Level level;
};
}

#endif
