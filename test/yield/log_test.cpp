// log_test.cpp

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

#include "yield/assert.hpp"
#include "yield/log.hpp"
#include "yunit.hpp"

#include <sstream>


TEST_SUITE(Log);

namespace yield {
class LogTest : public yunit::Test {
public:
  LogTest()
    : test_string("log_test")
  { }

  void setup() {
    log = &Log::open(log_oss, Log::DEBUG);
  }

  void teardown() {
    Log::dec_ref(log);
  }

protected:
  Log& get_log() const {
    return *log;
  }
  std::ostringstream& get_log_oss() {
    return log_oss;
  }
  const string& get_test_string() {
    return test_string;
  }

private:
  Log* log;
  std::ostringstream log_oss;
  string test_string;
};

TEST_EX(Log, get_stream, LogTest) {
  get_log().get_stream() << get_test_string();
  throw_assert_ge(get_log_oss().str().size(), get_test_string().size());
}

TEST_EX(Log, get_stream_ignore, LogTest) {
  get_log().get_stream(Log::Level(8)) << get_test_string();
  throw_assert_eq(get_log_oss().str().size(), 0);
}

TEST_EX(Log, Stream_copy, LogTest) {
  Log::Stream log_stream(get_log().get_stream());
}

//TEST_EX(Log, write_buffer, LogTest) {
//  get_log().write(StringBuffer(1024), Log::EMERG);
//  throw_assert_ge(get_log_oss().str().size(), get_test_string().size());
//
//  get_log().write(StringBuffer(get_test_string()), Log::EMERG);
//  throw_assert_ge(get_log_oss().str().size(), get_test_string().size());
//}

TEST_EX(Log, write_ignore, LogTest) {
  get_log().write(get_test_string(), Log::Level(8));
  throw_assert_eq(get_log_oss().str().size(), 0);
}

TEST_EX(Log, write_c_string, LogTest) {
  get_log().write(get_test_string().c_str(), Log::EMERG);
  throw_assert_eq(get_log_oss().str().size(), get_test_string().size());
}

TEST_EX(Log, write_string, LogTest) {
  get_log().write(get_test_string(), Log::EMERG);
  throw_assert_eq(get_log_oss().str().size(), get_test_string().size());
}

TEST_EX(Log, write_unprintable, LogTest) {
  uint8_t unprintable_string[8];
  char printable_string[8];
  for (uint8_t i = 0; i < get_test_string().size(); i++) {
    unprintable_string[i] = i;
    printable_string[i] = '.';
  }

  get_log().write
  (
    unprintable_string,
    get_test_string().size(),
    Log::EMERG
  );

  throw_assert_ge(get_log_oss().str().size(), get_test_string().size());
}

TEST(Log, Level_compare) {
  throw_assert_eq(Log::EMERG, Log::EMERG);
  throw_assert_lt(Log::EMERG, Log::ALERT);
  throw_assert_le(Log::EMERG, Log::ALERT);
  throw_assert_gt(Log::ALERT, Log::EMERG);
  throw_assert_ge(Log::ALERT, Log::EMERG);
}

TEST(Log, Level_from_int) {
  throw_assert_eq(Log::Level(static_cast<uint8_t>(0)), Log::EMERG);
  throw_assert_eq(Log::Level(static_cast<uint8_t>(1)), Log::ALERT);
  throw_assert_eq(Log::Level(static_cast<uint8_t>(2)), Log::CRIT);
  throw_assert_eq(Log::Level(static_cast<uint8_t>(3)), Log::ERR);
  throw_assert_eq(Log::Level(static_cast<uint8_t>(4)), Log::WARNING);
  throw_assert_eq(Log::Level(static_cast<uint8_t>(5)), Log::NOTICE);
  throw_assert_eq(Log::Level(static_cast<uint8_t>(6)), Log::INFO);
  throw_assert_eq(Log::Level(static_cast<uint8_t>(7)), Log::DEBUG);
}

TEST(Log, Level_from_c_string) {
  throw_assert_eq(Log::Level("EMERG"), Log::EMERG);
  throw_assert_eq(Log::Level("ALERT"), Log::ALERT);
  throw_assert_eq(Log::Level("CRIT"), Log::CRIT);
  throw_assert_eq(Log::Level("ERR"), Log::ERR);
  throw_assert_eq(Log::Level("WARNING"), Log::WARNING);
  throw_assert_eq(Log::Level("NOTICE"), Log::NOTICE);
  throw_assert_eq(Log::Level("INFO"), Log::INFO);
  throw_assert_eq(Log::Level("DEBUG"), Log::DEBUG);
}

TEST(Log, Level_from_string) {
  throw_assert_eq(Log::Level(string("EMERG")), Log::EMERG);
}
}
