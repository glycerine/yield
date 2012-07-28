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

#include "yield/auto_object.hpp"
#include "yield/date_time.hpp"
#include "yield/exception.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/path.hpp"
#include "yield/fs/stat.hpp"
#include "gtest/gtest.h"

#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace yield {
namespace fs {
class StatTest : public ::testing::Test {
public:
  StatTest() : test_dir_name("stat_test"), test_file_name("stat_test.txt")
  { }

  // ::testing::Test
  void SetUp() {
    TearDown();
  }

  void TearDown() {
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


TEST_F(StatTest, get_atime) {
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }

  DateTime now = DateTime::now();
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  ASSERT_NE(stbuf->get_atime(), DateTime::INVALID_DATE_TIME);
  ASSERT_LE(stbuf->get_atime(), now);
}

TEST_F(StatTest, get_ctime) {
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }
  DateTime now = DateTime::now();
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  ASSERT_NE(stbuf->get_ctime(), DateTime::INVALID_DATE_TIME);
  ASSERT_LE(stbuf->get_ctime(), now);
}

TEST_F(StatTest, get_mtime) {
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }
  DateTime now = DateTime::now();
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  ASSERT_NE(stbuf->get_mtime(), DateTime::INVALID_DATE_TIME);
  ASSERT_LE(stbuf->get_mtime(), now);
}

TEST_F(StatTest, get_nlink) {
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  ASSERT_EQ(stbuf->get_nlink(), 1);
}

TEST_F(StatTest, get_size) {
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  ASSERT_EQ(stbuf->get_size(), 0);
}

#ifdef __linux__
TEST(Stat, ISBLK) {
  auto_Object<Stat> stbuf = FileSystem().stat("/dev/cdrom");
  ASSERT_TRUE(stbuf->ISBLK());
}

TEST(Stat, ISCHR) {
  auto_Object<Stat> stbuf = FileSystem().stat("/dev/tty");
  ASSERT_TRUE(stbuf->ISCHR());
}
#endif

TEST_F(StatTest, ISDIR) {
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_dir_name());
  ASSERT_TRUE(stbuf->ISDIR());
}

#ifndef _WIN32
TEST_F(StatTest, ISFIFO) {
  if (!FileSystem().mkfifo(get_test_file_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
  ASSERT_TRUE(stbuf->ISFIFO());
}

TEST_F(StatTest, ISLNK) {
  if (!FileSystem().symlink(get_test_dir_name(), get_test_file_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf = FileSystem().lstat(get_test_file_name());
  ASSERT_TRUE(stbuf->ISLNK());
}
#endif

TEST_F(StatTest, ISREG) {
  if (!FileSystem().touch(get_test_file_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
  ASSERT_TRUE(stbuf->ISREG());
}

TEST_F(StatTest, operator_equals) {
  if (!FileSystem().touch(get_test_file_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf1 = FileSystem().stat(get_test_file_name());
  auto_Object<Stat> stbuf2 = FileSystem().stat(get_test_file_name());
  ASSERT_EQ(*stbuf1, *stbuf2);
}

#ifdef _WIN32
TEST_F(StatTest, operator_as_BY_HANDLE_FILE_INFORMATION) {
  if (!FileSystem().touch(get_test_file_name())) {
    throw Exception();
  }

  auto_Object<Stat> stbuf1 = FileSystem().stat(get_test_file_name());

  BY_HANDLE_FILE_INFORMATION bhfi = *stbuf1;

  ASSERT_NE(bhfi.dwFileAttributes, static_cast<DWORD>(0));

  ASSERT_NE(DateTime(bhfi.ftCreationTime), 0);

  ASSERT_GE(
    DateTime(bhfi.ftLastAccessTime),
    DateTime(bhfi.ftCreationTime)
  );

  ASSERT_GE(
    DateTime(bhfi.ftLastWriteTime),
    DateTime(bhfi.ftCreationTime)
  );

  ASSERT_EQ(bhfi.nFileSizeLow, 0);
  ASSERT_EQ(bhfi.nFileSizeHigh, 0);

  ASSERT_EQ(bhfi.nNumberOfLinks, 1);

  ASSERT_EQ(bhfi.nFileIndexLow, 0);
  ASSERT_EQ(bhfi.nFileIndexHigh, 0);

  Stat stbuf2(bhfi);
  ASSERT_EQ(stbuf2, *stbuf1);

  stbuf2 = bhfi;
  ASSERT_EQ(stbuf2, *stbuf1);
}

TEST_F(StatTest, operator_as_WIN32_FILE_ATTRIBUTE_DATA) {
  if (!FileSystem().touch(get_test_file_name())) {
    throw Exception();
  }

  auto_Object<Stat> stbuf1 = FileSystem().stat(get_test_file_name());

  WIN32_FILE_ATTRIBUTE_DATA fad = *stbuf1;

  ASSERT_NE(fad.dwFileAttributes, static_cast<DWORD>(0));

  ASSERT_NE(DateTime(fad.ftCreationTime), 0);

  ASSERT_GE(
    DateTime(fad.ftLastAccessTime),
    DateTime(fad.ftCreationTime)
  );

  ASSERT_GE(
    DateTime(fad.ftLastWriteTime),
    DateTime(fad.ftCreationTime)
  );

  ASSERT_EQ(fad.nFileSizeLow, 0);
  ASSERT_EQ(fad.nFileSizeHigh, 0);

  Stat stbuf2(fad);
  ASSERT_EQ(stbuf2, *stbuf1);

  stbuf2 = fad;
  ASSERT_EQ(stbuf2, *stbuf1);
}

TEST_F(StatTest, operator_as_WIN32_FIND_DATA) {
  if (!FileSystem().touch(get_test_file_name())) {
    throw Exception();
  }

  auto_Object<Stat> stbuf1 = FileSystem().stat(get_test_file_name());

  WIN32_FIND_DATA fd = *stbuf1;

  ASSERT_NE(fd.dwFileAttributes, static_cast<DWORD>(0));

  ASSERT_NE(DateTime(fd.ftCreationTime), 0);

  ASSERT_GE(
    DateTime(fd.ftLastAccessTime),
    DateTime(fd.ftCreationTime)
  );

  ASSERT_GE(
    DateTime(fd.ftLastWriteTime),
    DateTime(fd.ftCreationTime)
  );

  ASSERT_EQ(fd.nFileSizeLow, 0);
  ASSERT_EQ(fd.nFileSizeHigh, 0);

  ASSERT_EQ(fd.cFileName[0], 0);
  ASSERT_EQ(fd.cAlternateFileName[0], 0);

  Stat stbuf2(fd);
  ASSERT_EQ(stbuf2, *stbuf1);

  stbuf2 = fd;
  ASSERT_EQ(stbuf2, *stbuf1);
}
#else
TEST_F(StatTest, operator_as_struct_stat) {
  if (!FileSystem().touch(get_test_file_name())) {
    throw Exception();
  }

  auto_Object<Stat> stbuf1 = FileSystem().stat(get_test_file_name());

  struct stat ss = static_cast<struct stat>(*stbuf1);
  ASSERT_EQ(ss.st_blksize, stbuf1->get_blksize());
  ASSERT_EQ(ss.st_blocks, stbuf1->get_blocks());
  ASSERT_EQ(ss.st_dev, stbuf1->get_dev());
  ASSERT_EQ(ss.st_gid, stbuf1->get_gid());
  ASSERT_EQ(ss.st_ino, stbuf1->get_ino());
  ASSERT_EQ(ss.st_mode, stbuf1->get_mode());
  ASSERT_EQ(ss.st_nlink, stbuf1->get_nlink());
  ASSERT_EQ(ss.st_rdev, stbuf1->get_rdev());
  ASSERT_EQ(ss.st_size, stbuf1->get_size());
  ASSERT_EQ(ss.st_uid, stbuf1->get_uid());

  Stat stbuf2(ss);
  ASSERT_EQ(stbuf2, *stbuf1);

}
#endif
}
}
