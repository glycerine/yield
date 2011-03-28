// yield/log.cpp

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
#include "yield/log.hpp"

#include <iostream>
#include <sstream>
#ifndef _WIN32
#include <stdlib.h> // For atoi
#endif


namespace yield {
class ostreamLog : public Log {
public:
  ostreamLog(std::ostream& os, const Level& level)
    : Log(level), os(os)
  { }

  // Log
  void write(const char* str, size_t str_len) {
    os.write(str, str_len);
  }

private:
  std::ostream& os;
};


Log::Level Log::EMERG("EMERG", 0);
Log::Level Log::ALERT("ALERT", 1);
Log::Level Log::CRIT("CRIT", 2);
Log::Level Log::ERR("ERR", 3);
Log::Level Log::WARNING("WARNING", 4);
Log::Level Log::NOTICE("NOTICE", 5);
Log::Level Log::INFO("INFO", 6);
Log::Level Log::DEBUG("DEBUG", 7);


Log::Log(const Level& level)
  : level(level)
{ }

Log& Log::open(std::ostream& os, const Level& level) {
  return *new ostreamLog(os, level);
}

void Log::write(const char* str, const Level& level) {
  write(str, strnlen(str, UINT16_MAX), level);
}

void Log::write(const string& str, const Level& level) {
  write(str.c_str(), str.size(), level);
}

void Log::write(const char* str, size_t str_len, const Level& level) {
  if (level <= this->level)
    write(str, str_len);
}

void Log::write(const void* str, size_t str_len, const Level& level) {
  if (level <= this->level)
    write(str, str_len);
}

void Log::write(const uint8_t* str, size_t str_len, const Level& level) {
  if (level <= this->level)
    write(str, str_len);
}

//void
//Log::write
//(
//  const iovec* iov,
//  int iovlen,
//  const Level& level
//)
//{
//  if ( level <= this->level )
//  {
//    for ( int iov_i = 0; iov_i < iovlen; iov_i++ )
//      write( iov[iov_i].iov_base, iov[iov_i].iov_len, level );
//  }
//}

//void
//Log::write
//(
//  const iovec* iov,
//  int iovlen,
//  size_t count,
//  const Level& level
//)
//{
//  if ( level <= this->level )
//  {
//    for ( int iov_i = 0; iov_i < iovlen; iov_i++ )
//    {
//      if ( iov[iov_i].iov_len <= count )
//      {
//        write( iov[iov_i].iov_base, iov[iov_i].iov_len );
//        count -= iov[iov_i].iov_len;
//      }
//      else
//      {
//        write( iov[iov_i].iov_base, count );
//        break;
//      }
//    }
//  }
//}

//void Log::write(const Buffer& buffer, const Level& level) {
//  if (level <= this->level) {
//    std::ostringstream str;
//
//    str << "Buffer( ";
//    str << "capacity=" << buffer.capacity() << ", ";
//    str << "size=" << buffer.size();
//
//    if (this->level == DEBUG) {
//      if (buffer.empty()) {
//        str << ", data='' )";
//        write(str.str().data(), str.str().size());
//      } else {
//        str << ", data='\n";
//        write(str.str().data(), str.str().size());
//        write(static_cast<const uint8_t*>(buffer), buffer.size());
//        if (static_cast<const char*>(buffer)[buffer.size() - 1] == '\n')
//          write("')\n", 3);
//        else
//          write("\n')\n", 4);
//      }
//    } else {
//      str << " )";
//      write(str.str().data(), str.str().size());
//    }
//  }
//}

void Log::write(const void* str, size_t str_len) {
  write(static_cast<const uint8_t*>(str), str_len);
}

void Log::write(const uint8_t* str, size_t str_len) {
  bool str_is_printable = true;
  for (size_t str_i = 0; str_i < str_len; str_i++) {
    if
    (
      str[str_i] == '\r' ||
      str[str_i] == '\n' ||
      (str[str_i] >= 32 && str[str_i] <= 126)
    )
      continue;
    else {
      str_is_printable = false;
      break;
    }
  }

  if (str_is_printable)
    write(reinterpret_cast<const char*>(str), str_len);
  else {
    char* printable_str = new char[str_len * 3];
    size_t printable_str_len = 0;

    for (size_t str_i = 0; str_i < str_len; str_i++) {
      char hex_digit = (str[str_i] >> 4) & 0x0F;
      if (hex_digit >= 0 && hex_digit <= 9)
        printable_str[printable_str_len++] = '0' + hex_digit;
      else
        printable_str[printable_str_len++] = 'A' + hex_digit - 10;

      hex_digit = str[str_i] & 0x0F;
      if (hex_digit >= 0 && hex_digit <= 9)
        printable_str[printable_str_len++] = '0' + hex_digit;
      else
        printable_str[printable_str_len++] = 'A' + hex_digit - 10;

      printable_str[printable_str_len++] = ' ';
    }

    write(printable_str, printable_str_len);

    delete [] printable_str;
  }
}


Log::Level::Level(const char* level)
  : level_string(level) {
  init(level, strlen(level));
}

Log::Level::Level(const string& level)
  : level_string(level) {
  init(level.data(), level.size());
}


Log::Level::Level(uint8_t level)
  : level_uint8(level) {
  switch (level) {
  case 0:
    level_string = "EMERG";
    break;
  case 1:
    level_string = "ALERT";
    break;
  case 2:
    level_string = "CRIT";
    break;
  case 3:
    level_string = "ERR";
    break;
  case 4:
    level_string = "WARNING";
    break;
  case 5:
    level_string = "NOTICE";
    break;
  case 6:
    level_string = "INFO";
    break;
  default:
    level_string = "DEBUG";
    break;
  }
}

Log::Level::Level(const char* level_string, uint8_t level_uint8)
  : level_string(level_string), level_uint8(level_uint8)
{ }

Log::Level::Level(const Level& other)
  : level_string(other.level_string), level_uint8(other.level_uint8)
{ }

void Log::Level::init(const char* level, size_t level_len) {
  level_uint8 = static_cast<uint8_t>(atoi(level));

  if (level_uint8 == 0) {
    if
    (
      (level_len == 5 && memcmp(level, "EMERG", 5) == 0) ||
      (level_len == 9 && memcmp(level, "LOG_EMERG", 9) == 0) ||
      (level_len == 9 && memcmp(level, "EMERGENCY", 9) == 0) ||
      (level_len == 5 && memcmp(level, "FATAL", 5) == 0) ||
      (level_len == 4 && memcmp(level, "FAIL", 4) == 0)
    )
      level_uint8 = EMERG;

    else if
    (
      (level_len == 5 && memcmp(level, "ALERT", 5) == 0) ||
      (level_len == 9 && memcmp(level, "LOG_ALERT", 9) == 0)
    )
      level_uint8 = ALERT;

    else if
    (
      (level_len == 4 && memcmp(level, "CRIT", 4) == 0) ||
      (level_len == 8 && memcmp(level, "LOG_CRIT", 8) == 0) ||
      (level_len == 8 && memcmp(level, "CRITICAL", 8) == 0)
    )
      level_uint8 = CRIT;

    else if
    (
      (level_len == 3 && memcmp(level, "ERR", 3) == 0) ||
      (level_len == 7 && memcmp(level, "LOG_ERR", 7) == 0) ||
      (level_len == 5 && memcmp(level, "ERROR", 5) == 0)
    )
      level_uint8 = ERR;

    else if
    (
      (level_len == 7 && memcmp(level, "WARNING", 7) == 0) ||
      (level_len == 11 && memcmp(level, "LOG_WARNING", 11) == 0) ||
      (level_len == 4 && memcmp(level, "WARN", 4) == 0)
    )
      level_uint8 = WARNING;

    else if
    (
      (level_len == 6 && memcmp(level, "NOTICE", 6) == 0) ||
      (level_len == 10 && memcmp(level, "LOG_NOTICE", 10) == 0)
    )
      level_uint8 = NOTICE;

    else if
    (
      (level_len == 4 && memcmp(level, "INFO", 4) == 0) ||
      (level_len == 8 && memcmp(level, "LOG_INFO", level_len) == 0)
    )
      level_uint8 = INFO;

    else
      level_uint8 = DEBUG;
  }
}

bool Log::Level::operator<(const Level& other) const {
  return level_uint8 < other.level_uint8;
}

bool Log::Level::operator<=(const Level& other) const {
  return level_uint8 <= other.level_uint8;
}

bool Log::Level::operator==(const Level& other) const {
  return level_uint8 == other.level_uint8;
}

bool Log::Level::operator>(const Level& other) const {
  return level_uint8 > other.level_uint8;
}

bool Log::Level::operator>=(const Level& other) const {
  return level_uint8 >= other.level_uint8;
}


Log::Stream::Stream(Log& log, Log::Level level)
  : log(log), level(level)
{ }

Log::Stream::Stream(const Stream& other)
  : log(other.log.inc_ref()), level(other.level)
{ }

Log::Stream::~Stream() {
  if (level <= log.get_level() && !oss.str().empty()) {
    std::ostringstream stamped_oss;
    stamped_oss << static_cast<uint32_t>(DateTime::now().as_unix_date_time_s());
    stamped_oss << " ";
    stamped_oss << static_cast<const char*>(log.get_level());
    stamped_oss << ": ";
    stamped_oss << oss.str();
    stamped_oss << std::endl;

    log.write(stamped_oss.str(), level);
  }

  Log::dec_ref(log);
}
}
