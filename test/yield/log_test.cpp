// log_test.cpp

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

#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "yield/log.hpp"
#include "gtest/gtest.h"

#include <sstream>

namespace yield {
class LogTest : public ::testing::Test {
public:
  LogTest()
    : test_string("log_test")
  { }

  void SetUp() {
    log = &Log::open(log_oss, Log::Level::DEBUG);
  }

  void TearDown() {
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

TEST_F(LogTest, get_stream) {
  get_log().get_stream() << get_test_string().c_str();
  ASSERT_GE(get_log_oss().str().size(), get_test_string().size());
}

//TEST_F(LogTest, get_stream_ignore) {
//  get_log().get_stream(Log::Level(8)) << get_test_string().c_str();
//  ASSERT_EQ(get_log_oss().str().size(), 0);
//}

TEST_F(LogTest, Stream_copy) {
  Log::Stream log_stream(get_log().get_stream());
}

TEST_F(LogTest, write_Buffer) {
  auto_Object<Buffer> buffer = Buffer::copy(get_test_string());
  get_log().write(*buffer, Log::Level::EMERG);
  ASSERT_EQ(get_log_oss().str().size(), get_test_string().size());
}

//TEST_F(LogTest, write_ignore) {
//  get_log().write(get_test_string(), Log::Level(8));
//  ASSERT_EQ(get_log_oss().str().size(), 0);
//}

TEST_F(LogTest, write_c_string) {
  get_log().write(get_test_string().c_str(), Log::Level::EMERG);
  ASSERT_EQ(get_log_oss().str().size(), get_test_string().size());
}

TEST_F(LogTest, write_string) {
  get_log().write(get_test_string(), Log::Level::EMERG);
  ASSERT_EQ(get_log_oss().str().size(), get_test_string().size());
}

TEST_F(LogTest, write_unprintable) {
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
    Log::Level::EMERG
  );

  ASSERT_GE(get_log_oss().str().size(), get_test_string().size());
}

TEST(Log, Level_compare) {
  ASSERT_EQ(Log::Level::EMERG, Log::Level::EMERG);
  ASSERT_LT(Log::Level::EMERG, Log::Level::ALERT);
  ASSERT_LE(Log::Level::EMERG, Log::Level::ALERT);
  ASSERT_GT(Log::Level::ALERT, Log::Level::EMERG);
  ASSERT_GE(Log::Level::ALERT, Log::Level::EMERG);
}

//TEST(Log, Level_from_int) {
//  ASSERT_EQ(Log::Level(static_cast<uint8_t>(0)), Log::Level::EMERG);
//  ASSERT_EQ(Log::Level(static_cast<uint8_t>(1)), Log::Level::ALERT);
//  ASSERT_EQ(Log::Level(static_cast<uint8_t>(2)), Log::Level::CRIT);
//  ASSERT_EQ(Log::Level(static_cast<uint8_t>(3)), Log::Level::ERR);
//  ASSERT_EQ(Log::Level(static_cast<uint8_t>(4)), Log::Level::WARNING);
//  ASSERT_EQ(Log::Level(static_cast<uint8_t>(5)), Log::Level::NOTICE);
//  ASSERT_EQ(Log::Level(static_cast<uint8_t>(6)), Log::Level::INFO);
//  ASSERT_EQ(Log::Level(static_cast<uint8_t>(7)), Log::Level::DEBUG);
//}

//TEST(Log, Level_from_c_string) {
//  ASSERT_EQ(Log::Level("EMERG"), Log::Level::EMERG);
//  ASSERT_EQ(Log::Level("ALERT"), Log::Level::ALERT);
//  ASSERT_EQ(Log::Level("CRIT"), Log::Level::CRIT);
//  ASSERT_EQ(Log::Level("ERR"), Log::Level::ERR);
//  ASSERT_EQ(Log::Level("WARNING"), Log::Level::WARNING);
//  ASSERT_EQ(Log::Level("NOTICE"), Log::Level::NOTICE);
//  ASSERT_EQ(Log::Level("INFO"), Log::Level::INFO);
//  ASSERT_EQ(Log::Level("DEBUG"), Log::Level::DEBUG);
//}

//TEST(Log, Level_from_string) {
//  ASSERT_EQ(Log::Level(string("EMERG")), Log::Level::EMERG);
//}
}
