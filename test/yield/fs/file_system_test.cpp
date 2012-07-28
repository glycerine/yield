// file_system_test.cpp

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
#include "yield/exception.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/stat.hpp"
#include "gtest/gtest.h"

#include <fcntl.h>
#ifndef _WIN32
#include <sys/statvfs.h>
#endif

namespace yield {
namespace fs {
class FileSystemTest : public ::testing::Test {
public:
  virtual ~FileSystemTest() { }

protected:
  FileSystemTest()
    : test_dir_name("file_system_test"),
      test_file_name("file_system_test.txt"),
      test_link_name("file_system_test_link.txt")
  { }

  const Path& get_test_dir_name() const {
    return test_dir_name;
  }

  const Path& get_test_file_name() const {
    return test_file_name;
  }

  const Path& get_test_link_name() const {
    return test_link_name;
  }

  // ::testing::Test
  void SetUp() {
    TearDown();
    FileSystem().touch(get_test_file_name());
  }

  void TearDown() {
    FileSystem().rmtree(get_test_dir_name());
    FileSystem().unlink(get_test_file_name());
    FileSystem().unlink(get_test_link_name());
  }

private:
  Path test_dir_name, test_file_name, test_link_name;
};


#ifndef _WIN32
TEST_F(FileSystemTest, access) {
  ASSERT_TRUE(FileSystem().access(get_test_file_name(), F_OK));
}

TEST_F(FileSystemTest, chmod) {
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
  ASSERT_TRUE(
    FileSystem().chmod(get_test_file_name(), stbuf->get_mode())
  );
}

TEST_F(FileSystemTest, chown) {
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());

  ASSERT_TRUE(
    FileSystem().chown(get_test_file_name(), stbuf->get_uid())
  );

  ASSERT_TRUE(
    FileSystem().chown(
      get_test_file_name(),
      stbuf->get_uid(),
      stbuf->get_gid()
    )
  );
}
#endif

TEST_F(FileSystemTest, creat) {
  File* file = FileSystem().creat(get_test_file_name());
  if (file != NULL) {
    File::dec_ref(*file);
  } else {
    throw Exception();
  }
}

TEST_F(FileSystemTest, exists) {
  if (!FileSystem().exists(get_test_file_name())) {
    throw Exception();
  }

  ASSERT_FALSE(FileSystem().exists(Path("some other file.txt")));
}

TEST_F(FileSystemTest, isdir) {
  FileSystem().mkdir(get_test_dir_name());
  ASSERT_TRUE(FileSystem().isdir(get_test_dir_name()));
  ASSERT_FALSE(FileSystem().isdir(get_test_file_name()));
  ASSERT_FALSE(FileSystem().isdir(Path("nodir")));
}

TEST_F(FileSystemTest, isreg) {
  ASSERT_TRUE(FileSystem().isreg(get_test_file_name()));
  FileSystem().mkdir(get_test_dir_name());
  ASSERT_FALSE(FileSystem().isreg(get_test_dir_name()));
  ASSERT_FALSE(FileSystem().isreg(Path("nofile.txt")));
}

TEST_F(FileSystemTest, link) {
  if (!FileSystem().link(get_test_file_name(), get_test_link_name())) {
    throw Exception();
  }
}

#ifndef _WIN32
TEST_F(FileSystemTest, lstat) {
  if (!FileSystem().symlink(get_test_file_name(), get_test_link_name())) {
    throw Exception();
  }
  auto_Object<Stat> stbuf = FileSystem().lstat(get_test_link_name());
  ASSERT_TRUE(stbuf->ISLNK());
}
#endif

TEST_F(FileSystemTest, mkdir) {
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }
}

TEST_F(FileSystemTest, mktree) {
  Path test_subdir_path(get_test_dir_name() / get_test_dir_name());
  if (!FileSystem().mktree(test_subdir_path)) {
    throw Exception();
  }
  ASSERT_TRUE(FileSystem().exists(test_subdir_path));
}

TEST_F(FileSystemTest, open) {
  auto_Object<File> file = FileSystem().open(get_test_file_name());

  File* no_file = FileSystem().open(Path("nofile.txt"), O_RDONLY);
  if (no_file != NULL) {
    File::dec_ref(*no_file);
    ASSERT_TRUE(false);
  }
}

#ifndef _WIN32
TEST_F(FileSystemTest, readlink) {
  if (!FileSystem().symlink(get_test_file_name(), get_test_link_name())) {
    throw Exception();
  }

  Path target_path;
  if (FileSystem().readlink(get_test_link_name(), target_path)) {
    ASSERT_EQ(target_path, get_test_file_name());
  } else {
    throw Exception();
  }
}
#endif

TEST_F(FileSystemTest, realpath) {
  Path realpath;
  if (!FileSystem().realpath(get_test_file_name(), realpath)) {
    throw Exception();
  }
  ASSERT_FALSE(realpath.empty());
  ASSERT_NE(get_test_file_name(), realpath);
  ASSERT_LT(get_test_file_name().size(), realpath.size());
}

TEST_F(FileSystemTest, rename) {
  if (!FileSystem().rename(get_test_file_name(), Path("file_system_test2.txt"))) {
    throw Exception();
  }
  FileSystem().unlink(Path("file_system_test2.txt"));
}

TEST_F(FileSystemTest, rmdir) {
  FileSystem().mkdir(get_test_dir_name());
  if (!FileSystem().rmdir(get_test_dir_name())) {
    throw Exception();
  }
}

TEST_F(FileSystemTest, rmtree) {
  if (!FileSystem().mkdir(get_test_dir_name())) {
    throw Exception();
  }
  if (!FileSystem().mkdir(get_test_dir_name() / get_test_dir_name())) {
    throw Exception();
  }
  if (!FileSystem().touch(get_test_dir_name() / get_test_file_name())) {
    throw Exception();
  }

  if (!FileSystem().rmtree(get_test_dir_name())) {
    throw Exception();
  }

  ASSERT_FALSE(FileSystem().exists(get_test_dir_name()));
}

TEST_F(FileSystemTest, stat) {
  DateTime now = DateTime::now();
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
  ASSERT_NE(stbuf->get_atime(), DateTime::INVALID_DATE_TIME);
  ASSERT_LE(stbuf->get_atime(), now);
  ASSERT_NE(stbuf->get_ctime(), DateTime::INVALID_DATE_TIME);
  ASSERT_LE(stbuf->get_ctime(), now);
  ASSERT_NE(stbuf->get_mtime(), DateTime::INVALID_DATE_TIME);
  ASSERT_LE(stbuf->get_mtime(), now);
  ASSERT_EQ(stbuf->get_nlink(), 1);
  ASSERT_EQ(stbuf->get_size(), 0);
  ASSERT_TRUE(stbuf->ISREG());
}

TEST_F(FileSystemTest, statvfs) {
  FileSystem().mkdir(get_test_dir_name());
  struct statvfs stbuf;
  if (!FileSystem().statvfs(get_test_dir_name(), stbuf)) {
    throw Exception();
  }
  ASSERT_GT(stbuf.f_bsize, 0u);
  ASSERT_GT(stbuf.f_blocks, 0);
  ASSERT_GT(stbuf.f_bfree, 0);
  ASSERT_GE(stbuf.f_blocks, stbuf.f_bfree);
}

#ifndef _WIN32
TEST_F(FileSystemTest, symlink) {
  if (!FileSystem().symlink(get_test_file_name(), get_test_link_name())) {
    throw Exception();
  }
}
#endif

TEST_F(FileSystemTest, touch) {
  if (!FileSystem().touch(get_test_file_name())) {
    throw Exception();
  }

  if (FileSystem().touch(get_test_dir_name() / get_test_file_name())) {
    ASSERT_TRUE(false);
  }
}

TEST_F(FileSystemTest, unlink) {
  if (!FileSystem().unlink(get_test_file_name())) {
    throw Exception();
  }
}

TEST_F(FileSystemTest, utime) {
  DateTime atime = DateTime::now(),
           mtime = DateTime::now();

  if (FileSystem().utime(get_test_file_name(), atime, mtime)) {
    auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
    ASSERT_LE(stbuf->get_atime() - atime, Time::NS_IN_S);
    ASSERT_LE(stbuf->get_mtime() - mtime, Time::NS_IN_S);
  } else {
    throw Exception();
  }
}

#ifdef _WIN32
TEST_F(FileSystemTest, utime_win32) {
  DateTime atime = DateTime::now();
  DateTime mtime = DateTime::now();
  DateTime ctime = DateTime::now();

  if (FileSystem().utime(get_test_file_name(), atime, mtime, ctime)) {
    auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
    ASSERT_LE(stbuf->get_atime() - atime, Time::NS_IN_S);
    ASSERT_LE(stbuf->get_mtime() - mtime, Time::NS_IN_S);
    ASSERT_LE(stbuf->get_ctime() - ctime, Time::NS_IN_S);
  } else if (Exception::get_last_error_code() != ENOTSUP) {
    throw Exception();
  }
}
#endif
}
}
