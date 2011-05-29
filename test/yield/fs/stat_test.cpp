// stat_test.cpp

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
#include "yield/auto_object.hpp"
#include "yield/date_time.hpp"
#include "yield/exception.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/path.hpp"
#include "yield/fs/stat.hpp"
#include "yunit.hpp"

#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif

TEST_SUITE(Stat);

namespace yield {
namespace fs {
class StatTest : public yunit::Test {
public:
  StatTest() : test_dir_name("stat_test"), test_file_name("stat_test.txt")
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


TEST_EX(Stat, get_atime, StatTest) {
  DateTime now = DateTime::now();
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  throw_assert_ne(stbuf->get_atime(), DateTime::INVALID_DATE_TIME);
  throw_assert_le(stbuf->get_atime(), now);
}

TEST_EX(Stat, get_ctime, StatTest) {
  DateTime now = DateTime::now();
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  throw_assert_ne(stbuf->get_ctime(), DateTime::INVALID_DATE_TIME);
  throw_assert_le(stbuf->get_ctime(), now);
}

TEST_EX(Stat, get_mtime, StatTest) {
  DateTime now = DateTime::now();
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  throw_assert_ne(stbuf->get_mtime(), DateTime::INVALID_DATE_TIME);
  throw_assert_le(stbuf->get_mtime(), now);
}

TEST_EX(Stat, get_nlink, StatTest) {
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  throw_assert_eq(stbuf->get_nlink(), 1);
}

TEST_EX(Stat, get_size, StatTest) {
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  throw_assert_eq(stbuf->get_size(), 0);
}

TEST_EX(Stat, ISDIR, StatTest) {
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  throw_assert(stbuf->ISDIR());
}

TEST_EX(Stat, ISREG, StatTest) {
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
  throw_assert(stbuf->ISREG());
}

TEST_EX(Stat, operator_equals, StatTest) {
  auto_Object<Stat> stbuf1 = FileSystem().stat(get_test_file_name());
  auto_Object<Stat> stbuf2 = FileSystem().stat(get_test_file_name());
  throw_assert_eq(*stbuf1, *stbuf2);
}

#ifdef _WIN32
TEST_EX(Stat, operator_as_BY_HANDLE_FILE_INFORMATION, StatTest) {
  auto_Object<Stat> stbuf1
  = static_cast<Stat*>(FileSystem().stat(get_test_file_name()));

  BY_HANDLE_FILE_INFORMATION bhfi = *stbuf1;

  throw_assert_ne(bhfi.dwFileAttributes, 0);

  throw_assert_ne(DateTime(bhfi.ftCreationTime), 0);

  throw_assert_ge(
    DateTime(bhfi.ftLastAccessTime),
    DateTime(bhfi.ftCreationTime)
  );

  throw_assert_ge(
    DateTime(bhfi.ftLastWriteTime),
    DateTime(bhfi.ftCreationTime)
  );

  throw_assert_eq(bhfi.nFileSizeLow, 0);
  throw_assert_eq(bhfi.nFileSizeHigh, 0);

  throw_assert_eq(bhfi.nNumberOfLinks, 1);

  throw_assert_eq(bhfi.nFileIndexLow, 0);
  throw_assert_eq(bhfi.nFileIndexHigh, 0);

  Stat stbuf2(bhfi);
  throw_assert_eq(stbuf2, *stbuf1);

  stbuf2 = bhfi;
  throw_assert_eq(stbuf2, *stbuf1);
}

TEST_EX(Stat, operator_as_WIN32_FILE_ATTRIBUTE_DATA, StatTest) {
  auto_Object<Stat> stbuf1
  = static_cast<Stat*>(FileSystem().stat(get_test_file_name()));

  WIN32_FILE_ATTRIBUTE_DATA fad = *stbuf1;

  throw_assert_ne(fad.dwFileAttributes, 0);

  throw_assert_ne(DateTime(fad.ftCreationTime), 0);

  throw_assert_ge(
    DateTime(fad.ftLastAccessTime),
    DateTime(fad.ftCreationTime)
  );

  throw_assert_ge(
    DateTime(fad.ftLastWriteTime),
    DateTime(fad.ftCreationTime)
  );

  throw_assert_eq(fad.nFileSizeLow, 0);
  throw_assert_eq(fad.nFileSizeHigh, 0);

  Stat stbuf2(fad);
  throw_assert_eq(stbuf2, *stbuf1);

  stbuf2 = fad;
  throw_assert_eq(stbuf2, *stbuf1);
}

TEST_EX(Stat, operator_as_WIN32_FIND_DATA, StatTest) {
  auto_Object<Stat> stbuf1
  = static_cast<Stat*>(FileSystem().stat(get_test_file_name()));

  WIN32_FIND_DATA fd = *stbuf1;

  throw_assert_ne(fd.dwFileAttributes, 0);

  throw_assert_ne(DateTime(fd.ftCreationTime), 0);

  throw_assert_ge(
    DateTime(fd.ftLastAccessTime),
    DateTime(fd.ftCreationTime)
  );

  throw_assert_ge(
    DateTime(fd.ftLastWriteTime),
    DateTime(fd.ftCreationTime)
  );

  throw_assert_eq(fd.nFileSizeLow, 0);
  throw_assert_eq(fd.nFileSizeHigh, 0);

  throw_assert_eq(fd.cFileName[0], 0);
  throw_assert_eq(fd.cAlternateFileName[0], 0);

  Stat stbuf2(fd);
  throw_assert_eq(stbuf2, *stbuf1);

  stbuf2 = fd;
  throw_assert_eq(stbuf2, *stbuf1);
}
#endif
}
}