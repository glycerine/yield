// stat_test.hpp

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

#ifndef _YIELD_FS_STAT_TEST_HPP_
#define _YIELD_FS_STAT_TEST_HPP_

#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/date_time.hpp"
#include "yield/exception.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/path.hpp"
#include "yield/fs/stat.hpp"
#include "yunit.hpp"

#include <sstream>


TEST_SUITE(Stat)

namespace yield {
namespace fs {
class StatTest : public yunit::Test {
public:
  StatTest()
    : test_dir_name("stat_test"),
      test_file_name("stat_test.txt")
  { }

  // yunit::Test
  void setup() {
    teardown();

    if (!FileSystem().touch(get_test_file_name()))
      throw Exception();

    if (!FileSystem().mkdir(get_test_dir_name()))
      throw Exception();
  }

  void teardown() {
    FileSystem().unlink(get_test_file_name());
    FileSystem().rmdir(get_test_dir_name());
  }

protected:
  const Path& get_test_dir_name() const {
    return test_dir_name;
  }

  const Path& get_test_file_name() const {
    return test_file_name;
  }

private:
  Path test_dir_name, test_file_name;
};


class StatGetAtimeTest : public StatTest {
public:
  // yunit::Test
  void run() {
    DateTime now = DateTime::now();
    auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
    throw_assert_ne(stbuf->get_atime(), DateTime::INVALID_DATE_TIME);
    throw_assert_le(stbuf->get_atime(), now);
  }
};


class StatGetCtimeTest : public StatTest {
public:
  // yunit::Test
  void run() {
    DateTime now = DateTime::now();
    auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
    throw_assert_ne(stbuf->get_ctime(), DateTime::INVALID_DATE_TIME);
    throw_assert_le(stbuf->get_ctime(), now);
  }
};


class StatGetMtimeTest : public StatTest {
public:
  // yunit::Test
  void run() {
    DateTime now = DateTime::now();
    auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
    throw_assert_ne(stbuf->get_mtime(), DateTime::INVALID_DATE_TIME);
    throw_assert_le(stbuf->get_mtime(), now);
  }
};


class StatGetNlinkTest : public StatTest {
public:
  // yunit::Test
  void run() {
    auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
    throw_assert_eq(stbuf->get_nlink(), 1);
  }
};


class StatGetSizeTest : public StatTest {
public:
  // yunit::Test
  void run() {
    auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
    throw_assert_eq(stbuf->get_size(), 0);
  }
};


class StatISDIRTest : public StatTest {
public:
  // yunit::Test
  void run() {
    auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
    throw_assert(stbuf->ISDIR());
  }
};


class StatISREGTest : public StatTest {
public:
  // yunit::Test
  void run() {
    auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
    throw_assert(stbuf->ISREG());
  }
};


class StatOperatorEqualsEqualsTest : public StatTest {
public:
  // yunit::Test
  void run() {
    auto_Object<Stat> stbuf1 = FileSystem().stat(get_test_file_name());
    auto_Object<Stat> stbuf2 = FileSystem().stat(get_test_file_name());
    throw_assert_eq(*stbuf1, *stbuf2);
  }
};


class StatTestSuite : public yunit::TestSuite {
public:
  StatTestSuite() {
    add("Stat::get_atime", new StatGetAtimeTest);
    add("Stat::get_ctime", new StatGetCtimeTest);
    add("Stat::get_mtime", new StatGetMtimeTest);
    add("Stat::get_nlink", new StatGetNlinkTest);
    add("Stat::get_size", new StatGetSizeTest);
    add("Stat::ISDIR", new StatISDIRTest);
    add("Stat::ISREG", new StatISREGTest);
    add("Stat::operator==", new StatOperatorEqualsEqualsTest);
  }
};
}
}

#endif
