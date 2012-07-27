// stat_test.cpp

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
  }

  void teardown() {
    FileSystem().rmdir(get_test_dir_name());
    FileSystem().unlink(get_test_file_name());
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
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }

  DateTime now = DateTime::now();
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  throw_assert_ne(stbuf->get_atime(), DateTime::INVALID_DATE_TIME);
  throw_assert_le(stbuf->get_atime(), now);
}

TEST_EX(Stat, get_ctime, StatTest) {
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }
  DateTime now = DateTime::now();
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  throw_assert_ne(stbuf->get_ctime(), DateTime::INVALID_DATE_TIME);
  throw_assert_le(stbuf->get_ctime(), now);
}

TEST_EX(Stat, get_mtime, StatTest) {
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }
  DateTime now = DateTime::now();
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  throw_assert_ne(stbuf->get_mtime(), DateTime::INVALID_DATE_TIME);
  throw_assert_le(stbuf->get_mtime(), now);
}

TEST_EX(Stat, get_nlink, StatTest) {
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  throw_assert_eq(stbuf->get_nlink(), 1);
}

TEST_EX(Stat, get_size, StatTest) {
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  throw_assert_eq(stbuf->get_size(), 0);
}

#ifdef __linux__
TEST(Stat, ISBLK) {
  auto_Object<Stat> stbuf = FileSystem().stat("/dev/cdrom");
  throw_assert_true(stbuf->ISBLK());
}

TEST(Stat, ISCHR) {
  auto_Object<Stat> stbuf = FileSystem().stat("/dev/tty");
  throw_assert_true(stbuf->ISCHR());
}
#endif

TEST_EX(Stat, ISDIR, StatTest) {
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  throw_assert(stbuf->ISDIR());
}

#ifndef _WIN32
TEST_EX(Stat, ISFIFO, StatTest) {
  if (!FileSystem().mkfifo(get_test_file_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
  throw_assert(stbuf->ISFIFO());
}

TEST_EX(Stat, ISLNK, StatTest) {
  if (!FileSystem().symlink(get_test_dir_name(), get_test_file_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf = FileSystem().lstat(get_test_file_name());
  throw_assert_true(stbuf->ISLNK());
}
#endif

TEST_EX(Stat, ISREG, StatTest) {
  if (!FileSystem().touch(get_test_file_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
  throw_assert(stbuf->ISREG());
}

TEST_EX(Stat, operator_equals, StatTest) {
  if (!FileSystem().touch(get_test_file_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf1 = FileSystem().stat(get_test_file_name());
  auto_Object<Stat> stbuf2 = FileSystem().stat(get_test_file_name());
  throw_assert_eq(*stbuf1, *stbuf2);
}

#ifdef _WIN32
TEST_EX(Stat, operator_as_BY_HANDLE_FILE_INFORMATION, StatTest) {
  if (!FileSystem().touch(get_test_file_name())) {
    throw Exception();
  }

  auto_Object<Stat> stbuf1 = FileSystem().stat(get_test_file_name());

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
  if (!FileSystem().touch(get_test_file_name())) {
    throw Exception();
  }

  auto_Object<Stat> stbuf1 = FileSystem().stat(get_test_file_name());

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
  if (!FileSystem().touch(get_test_file_name())) {
    throw Exception();
  }

  auto_Object<Stat> stbuf1 = FileSystem().stat(get_test_file_name());

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
#else
TEST_EX(Stat, operator_as_struct_stat, StatTest) {
  if (!FileSystem().touch(get_test_file_name())) {
    throw Exception();
  }

  auto_Object<Stat> stbuf1 = FileSystem().stat(get_test_file_name());

  struct stat ss = static_cast<struct stat>(*stbuf1);
  throw_assert_eq(ss.st_blksize, stbuf1->get_blksize());
  throw_assert_eq(ss.st_blocks, stbuf1->get_blocks());
  throw_assert_eq(ss.st_dev, stbuf1->get_dev());
  throw_assert_eq(ss.st_gid, stbuf1->get_gid());
  throw_assert_eq(ss.st_ino, stbuf1->get_ino());
  throw_assert_eq(ss.st_mode, stbuf1->get_mode());
  throw_assert_eq(ss.st_nlink, stbuf1->get_nlink());
  throw_assert_eq(ss.st_rdev, stbuf1->get_rdev());
  throw_assert_eq(ss.st_size, stbuf1->get_size());
  throw_assert_eq(ss.st_uid, stbuf1->get_uid());

  Stat stbuf2(ss);
  throw_assert_eq(stbuf2, *stbuf1);

}
#endif
}
}
