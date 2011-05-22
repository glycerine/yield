// file_system_test.cpp

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

#include "yield/auto_object.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/stat.hpp"
#include "yunit.hpp"

#include <fcntl.h>
#ifndef _WIN32
#include <sys/statvfs.h>
#endif

TEST_SUITE(FileSystem);

namespace yield {
namespace fs {
class FileSystemTest : public yunit::Test {
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

  // yunit::Test
  void setup() {
    teardown();
    FileSystem().touch(get_test_file_name());
  }

  void teardown() {
    FileSystem().rmtree(get_test_dir_name());
    FileSystem().unlink(get_test_file_name());
    FileSystem().unlink(get_test_link_name());
  }

private:
  Path test_dir_name, test_file_name, test_link_name;
};


TEST_EX(FileSystem, creat, FileSystemTest) {
  File* file = FileSystem().creat(get_test_file_name());
  if (file != NULL)
    File::dec_ref(*file);
  else
    throw Exception();
}

TEST_EX(FileSystem, exists, FileSystemTest) {
  if (!FileSystem().exists(get_test_file_name()))
    throw Exception();

  throw_assert_false(FileSystem().exists(Path("some other file.txt")));
}

TEST_EX(FileSystem, isdir, FileSystemTest) {
  FileSystem().mkdir(get_test_dir_name());
  throw_assert(FileSystem().isdir(get_test_dir_name()));
  throw_assert_false(FileSystem().isdir(get_test_file_name()));
  throw_assert_false(FileSystem().isdir(Path("nodir")));
}

TEST_EX(FileSystem, isfile, FileSystemTest) {
  throw_assert(FileSystem().isfile(get_test_file_name()));
  FileSystem().mkdir(get_test_dir_name());
  throw_assert_false(FileSystem().isfile(get_test_dir_name()));
  throw_assert_false(FileSystem().isfile(Path("nofile.txt")));
}

TEST_EX(FileSystem, link, FileSystemTest) {
  if (!FileSystem().link(get_test_file_name(), get_test_link_name()))
    throw Exception();
}

TEST_EX(FileSystem, mkdir, FileSystemTest) {
  if (!FileSystem().mkdir(get_test_dir_name()))
    throw Exception();
}

TEST_EX(FileSystem, mktree, FileSystemTest) {
  Path subdir_path(Path("file_system_test") + Path("subdir"));
  if (!FileSystem().mktree(subdir_path)) throw Exception();
  throw_assert(FileSystem().exists(subdir_path));
}

TEST_EX(FileSystem, open, FileSystemTest) {
  auto_Object<File> file = FileSystem().open(get_test_file_name());

  File* no_file = FileSystem().open(Path("nofile.txt"), O_RDONLY);
  if (no_file != NULL) {
    File::dec_ref(*no_file);
    throw_assert(false);
  }
}

TEST_EX(FileSystem, realpath, FileSystemTest) {
  Path realpath;
  if (!FileSystem().realpath(get_test_file_name(), realpath))
    throw Exception();
  throw_assert_false(realpath.empty());
  throw_assert_ne(get_test_file_name(), realpath);
  throw_assert_lt(get_test_file_name().size(), realpath.size());
}

TEST_EX(FileSystem, rename, FileSystemTest) {
  if (!FileSystem().rename(get_test_file_name(), Path("file_system_test2.txt")))
    throw Exception();
  FileSystem().unlink(Path("file_system_test2.txt"));
}

TEST_EX(FileSystem, rmdir, FileSystemTest) {
  FileSystem().mkdir(get_test_dir_name());
  if (!FileSystem().rmdir(get_test_dir_name()))
    throw Exception();
}

TEST_EX(FileSystem, rmtree, FileSystemTest) {
  if (!FileSystem().mkdir(get_test_dir_name()))
    throw Exception();

  if (!FileSystem().touch(get_test_dir_name() / get_test_file_name()))
    throw Exception();

  if (!FileSystem().rmtree(get_test_dir_name()))
    throw Exception();

  throw_assert_false(FileSystem().exists(get_test_dir_name()));
}

TEST_EX(FileSystem, stat, FileSystemTest) {
  DateTime now = DateTime::now();
  auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
  throw_assert_ne(stbuf->get_atime(), DateTime::INVALID_DATE_TIME);
  throw_assert_le(stbuf->get_atime(), now);
  throw_assert_ne(stbuf->get_ctime(), DateTime::INVALID_DATE_TIME);
  throw_assert_le(stbuf->get_ctime(), now);
  throw_assert_ne(stbuf->get_mtime(), DateTime::INVALID_DATE_TIME);
  throw_assert_le(stbuf->get_mtime(), now);
  throw_assert_eq(stbuf->get_nlink(), 1);
  throw_assert_eq(stbuf->get_size(), 0);
  throw_assert(stbuf->ISREG());
}

TEST_EX(FileSystem, statvfs, FileSystemTest) {
  FileSystem().mkdir(get_test_dir_name());
  struct statvfs stbuf;
  if (!FileSystem().statvfs(get_test_dir_name(), stbuf))
    throw Exception();
  throw_assert_gt(stbuf.f_bsize, 0);
  throw_assert_gt(stbuf.f_blocks, 0);
  throw_assert_gt(stbuf.f_bfree, 0);
  throw_assert_ge(stbuf.f_blocks, stbuf.f_bfree);
  throw_assert_ge(stbuf.f_namemax, 0);
}

TEST_EX(FileSystem, touch, FileSystemTest) {
  if (!FileSystem().touch(get_test_file_name()))
    throw Exception();

  if (FileSystem().touch(get_test_dir_name() / get_test_file_name())) {
    throw_assert(false);
  }
}

TEST_EX(FileSystem, unlink, FileSystemTest) {
  if (!FileSystem().unlink(get_test_file_name()))
    throw Exception();
}

TEST_EX(FileSystem, utime, FileSystemTest) {
  DateTime atime = DateTime::now(),
            mtime = DateTime::now();

  if (FileSystem().utime(get_test_file_name(), atime, mtime)) {
    auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
    throw_assert_le(stbuf->get_atime() - atime, Time::NS_IN_S);
    throw_assert_le(stbuf->get_mtime() - mtime, Time::NS_IN_S);
  } else
    throw Exception();
}

#ifdef _WIN32
TEST_EX(FileSystem, utime_win32, FileSystemTest) {
  DateTime atime = DateTime::now();
  DateTime mtime = DateTime::now();
  DateTime ctime = DateTime::now();

  if (FileSystem().utime(get_test_file_name(), atime, mtime, ctime)) {
    auto_Object<Stat> stbuf = FileSystem().stat(get_test_file_name());
    throw_assert_le(stbuf->get_atime() - atime, Time::NS_IN_S);
    throw_assert_le(stbuf->get_mtime() - mtime, Time::NS_IN_S);
    throw_assert_le(stbuf->get_ctime() - ctime, Time::NS_IN_S);
  } else if (Exception::get_last_error_code() != ENOTSUP)
    throw Exception();
}
#endif
}
}
