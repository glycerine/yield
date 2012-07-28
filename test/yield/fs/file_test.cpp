// file_test.cpp

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

#include "../channel_test.hpp"
#include "yield/auto_object.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/stat.hpp"

#include <fcntl.h> // For O_*

namespace yield {
namespace fs {
class FilePair : public ChannelPair {
public:
  FilePair() : path("file_test.txt") {
    read_file = write_file = NULL;
  }

  ~FilePair() {
    File::dec_ref(read_file);
    File::dec_ref(write_file);
    FileSystem().unlink(path);
  }

public:
  File& get_read_file() {
    if (read_file == NULL) {
      get_write_file();

      read_file = FileSystem().open(path, O_RDONLY);
      if (read_file == NULL) {
        throw Exception();
      }
    }

    return *read_file;
  }

  File& get_write_file() {
    if (write_file == NULL) {
      write_file = FileSystem().open(path, O_CREAT | O_TRUNC | O_WRONLY);
      if (write_file == NULL) {
        throw Exception();
      }
    }

    return *write_file;
  }

public:
  // yield::ChannelPair
  Channel& get_read_channel() {
    return get_read_file();
  }

  Channel& get_write_channel() {
    return get_write_file();
  }

private:
  Path path;
  File* read_file, *write_file;
};


class FileTest : public ChannelTest<FilePair> {
public:
  FileTest() {
  }

public:
  File& get_read_file() {
    return static_cast<File&>(get_read_channel());
  }

  File& get_write_file() {
    return static_cast<File&>(get_write_channel());
  }
};

TEST_F(FileTest, datasync) {
  get_write_file().write(
    get_test_string().data(),
    get_test_string().size()
  );

  if (!get_write_file().datasync()) {
    throw Exception();
  }

  ASSERT_GE(
    get_write_file().stat()->get_size(),
    get_test_string().size()
  );
}

TEST_F(FileTest, dup) {
  File* dup_file = get_read_file().dup();
  if (dup_file != NULL) {
    File::dec_ref(*dup_file);
  } else {
    throw Exception();
  }

  dup_file = File::dup(static_cast<fd_t>(get_read_file()));
  if (dup_file != NULL) {
    File::dec_ref(*dup_file);
  } else {
    throw Exception();
  }
}

#ifndef _WIN32
TEST_F(FileTest, getlk) {
  if (!get_write_file().setlk(File::Lock(0, 256))) {
    throw Exception();
  }

  File::Lock* lock = get_write_file().getlk(File::Lock(0, 256));
  if (lock != NULL) {
    File::Lock::dec_ref(*lock);
  } else {
    throw Exception();
  }
}
#endif

TEST_F(FileTest, get_type_id) {
  ASSERT_EQ(get_write_file().get_type_id(), File::TYPE_ID);
}

TEST_F(FileTest, get_type_name) {
  ASSERT_EQ(
    strcmp(get_write_file().get_type_name(), "yield::fs::File"),
    0
  );
}

TEST_F(FileTest, map) {
  auto_Object<File::Map> mmf = get_read_file().mmap();
}

//class FileMapReadTest : public FileTest {
//public:
//  FileMapReadTest(FilePairFactory& file_pair_factory)
//    : FileTest(file_pair_factory)
//  { }
//
//  // ::testing::Test
//  void run() {
//    {
//      auto_Object<File::Map> mmf = get_test_file().mmap();
//
//      mmf->reserve(get_test_string().size());
//
//      memcpy_s
//      (
//        *mmf,
//        get_test_string().size(),
//        get_test_string().c_str(),
//        get_test_string().size()
//      );
//
//      if (mmf->sync())
//        mmf->close();
//      else
//        throw Exception();
//    }
//
//    {
//      auto_Object<File> test_file = FileSystem().open(get_test_file_name());
//
//      auto_Object<File::Map> mmf
//      = get_test_file().mmap(
//          SIZE_MAX,
//          0,
//          true,
//          false
//        );
//
//      ASSERT_EQ(mmf->capacity(), get_test_string().size());
//      ASSERT_EQ
//      (
//        strncmp(*mmf, get_test_string().data(), get_test_string().size()),
//        0
//      );
//    }
//  }
//};
//
//
//class FileMapWriteTest : public FileTest {
//public:
//  FileMapWriteTest(FilePairFactory& file_pair_factory)
//    : FileTest(file_pair_factory)
//  { }
//
//  // ::testing::Test
//  void run() {
//    auto_Object<File::Map> mmf = get_test_file().mmap();
//
//    mmf->reserve(get_test_string().size());
//
//    memcpy_s
//    (
//      *mmf,
//      get_test_string().size(),
//      get_test_string().data(),
//      get_test_string().size()
//    );
//
//    if (!mmf->sync(static_cast<size_t>(0), get_test_string().size()))
//      throw Exception();
//  }
//};

TEST_F(FileTest, pread) {
  write();

  string _string;
  _string.resize(7);
  ssize_t pread_ret
  = get_read_file().pread(
      const_cast<char*>(_string.data()),
      _string.size(),
      4
    );

  if (pread_ret >= 0) {
    ASSERT_EQ(static_cast<size_t>(pread_ret), _string.size());
    ASSERT_EQ(_string, " string");
  } else {
    throw Exception();
  }
}

TEST_F(FileTest, pread_Buffer) {
  write();

  auto_Object<Buffer> buffer = new Buffer(Buffer::getpagesize(), 7);
  ssize_t pread_ret = get_read_file().pread(*buffer, 4);
  if (pread_ret >= 0) {
    ASSERT_EQ(pread_ret, 7);
    ASSERT_EQ(*buffer, " string");
  } else {
    throw Exception();
  }
}

TEST_F(FileTest, pread_Buffers) {
  write();

  auto_Object<Buffer> _str = new Buffer(Buffer::getpagesize(), 4);
  auto_Object<Buffer> ing = new Buffer(Buffer::getpagesize(), 3);
  _str->set_next_buffer(ing->inc_ref());

  ssize_t pread_ret = get_read_file().pread(*_str, 4);
  if (pread_ret >= 0) {
    ASSERT_EQ(pread_ret, 7);
    ASSERT_EQ(*_str, " str");
    ASSERT_EQ(*ing, "ing");
  } else {
    throw Exception();
  }
}

TEST_F(FileTest, preadv_one) {
  write();

  string _string;
  _string.resize(7);
  iovec iov;
  iov.iov_base = const_cast<char*>(_string.data());
  iov.iov_len = _string.size();

  ssize_t preadv_ret = get_read_file().preadv(&iov, 1, 4);

  if (preadv_ret >= 0) {
    ASSERT_EQ(static_cast<size_t>(preadv_ret), _string.size());
    ASSERT_EQ(_string, " string");
  } else {
    throw Exception();
  }
}

TEST_F(FileTest, preadv_two) {
  write();

  string _string;
  _string.resize(7);
  iovec iov[2];
  iov[0].iov_base = const_cast<char*>(_string.data());
  iov[0].iov_len = 4;
  iov[1].iov_base = const_cast<char*>(_string.data()) + 4;
  iov[1].iov_len = 3;

  ssize_t preadv_ret = get_read_file().preadv(iov, 2, 4);

  if (preadv_ret >= 0) {
    ASSERT_EQ(static_cast<size_t>(preadv_ret), _string.size());
    ASSERT_EQ(_string, " string");
  } else {
    throw Exception();
  }
}

TEST_F(FileTest, pwrite) {
  ssize_t pwrite_ret = get_write_file().pwrite(" string", 7, 4);
  if (pwrite_ret >= 0) {
    ASSERT_EQ(pwrite_ret, 7);
  } else {
    throw Exception();
  }

  pwrite_ret = get_write_file().pwrite("test", 4, 0);
  if (pwrite_ret >= 0) {
    ASSERT_EQ(pwrite_ret, 4);
  } else {
    throw Exception();
  }

  read();
}

TEST_F(FileTest, pwrite_Buffer) {
  auto_Object<Buffer> buffer = Buffer::copy(" string");
  ssize_t pwrite_ret = get_write_file().pwrite(*buffer, 4);
  if (pwrite_ret >= 0) {
    ASSERT_EQ(pwrite_ret, 7);
  } else {
    throw Exception();
  }

  pwrite_ret = get_write_file().pwrite("test", 4, 0);
  if (pwrite_ret >= 0) {
    ASSERT_EQ(pwrite_ret, 4);
  } else {
    throw Exception();
  }

  read();
}

TEST_F(FileTest, pwrite_Buffers) {
  auto_Object<Buffer> _str = Buffer::copy(" str");
  auto_Object<Buffer> ing = Buffer::copy("ing");
  _str->set_next_buffer(ing->inc_ref());

  ssize_t pwrite_ret = get_write_file().pwrite(*_str, 4);
  if (pwrite_ret >= 0) {
    ASSERT_EQ(pwrite_ret, 7);
  } else {
    throw Exception();
  }

  pwrite_ret = get_write_file().pwrite("test", 4, 0);
  if (pwrite_ret >= 0) {
    ASSERT_EQ(pwrite_ret, 4);
  } else {
    throw Exception();
  }

  read();
}

TEST_F(FileTest, pwritev_one) {
  iovec iov;
  iov.iov_base = const_cast<char*>(" string");
  iov.iov_len = 7;
  ssize_t pwritev_ret = get_write_file().pwritev(&iov, 1, 4);
  if (pwritev_ret >= 0) {
    ASSERT_EQ(pwritev_ret, 7);
  } else {
    throw Exception();
  }

  iov.iov_base = const_cast<char*>("test");
  iov.iov_len = 4;
  pwritev_ret = get_write_file().pwritev(&iov, 1, 0);
  if (pwritev_ret >= 0) {
    ASSERT_EQ(pwritev_ret, 4);
  } else {
    throw Exception();
  }

  read();
}

TEST_F(FileTest, pwritev_two) {
  ssize_t pwrite_ret = get_write_file().pwrite(" string", 7, 4);
  if (pwrite_ret >= 0) {
    ASSERT_EQ(pwrite_ret, 7);
  } else {
    throw Exception();
  }

  iovec iov[2];
  iov[0].iov_base = const_cast<char*>("te");
  iov[0].iov_len = 2;
  iov[1].iov_base = const_cast<char*>("st");
  iov[1].iov_len = 2;
  ssize_t pwritev_ret = get_write_file().pwritev(iov, 2, 0);
  if (pwritev_ret >= 0) {
    ASSERT_EQ(pwritev_ret, 4);
  } else {
    throw Exception();
  }

  read();
}

TEST_F(FileTest, seek) {
  write();

  // SEEK_SET
  uint64_t seek_ret = get_read_file().seek(4);
  ASSERT_EQ(seek_ret, 4);

  // SEEK_CUR
  seek_ret = get_read_file().seek(2, SEEK_CUR);
  ASSERT_EQ(seek_ret, 6);

  // SEEK_END
  seek_ret = get_read_file().seek(-1, SEEK_END);
  ASSERT_EQ(seek_ret, get_test_string().size() - 1);

  seek_ret = get_read_file().seek(1, SEEK_END);
  ASSERT_EQ(seek_ret, get_test_string().size() + 1);

  // seek and read
  seek_ret = get_read_file().seek(4);
  string _string;
  _string.resize(7);
  ssize_t read_ret
  = get_read_file().read(
      const_cast<char*>(_string.data()),
      _string.size()
    );
  if (read_ret >= 0) {
    ASSERT_EQ(read_ret, 7);
    ASSERT_EQ(_string, " string");
  } else {
    throw Exception();
  }
}

TEST_F(FileTest, setlk) {
  if (!get_write_file().setlk(File::Lock(0, 256))) {
    throw Exception();
  }
}

TEST_F(FileTest, setlkw) {
  if (!get_write_file().setlkw(File::Lock(0, 256))) {
    throw Exception();
  }
}

TEST_F(FileTest, stat) {
  DateTime now = DateTime::now();
  auto_Object<Stat> stbuf = get_write_file().stat();
  ASSERT_NE(stbuf->get_atime(), DateTime::INVALID_DATE_TIME);
  ASSERT_LE(stbuf->get_atime(), now);
  ASSERT_NE(stbuf->get_ctime(), DateTime::INVALID_DATE_TIME);
  ASSERT_LE(stbuf->get_ctime(), now);
  ASSERT_NE(stbuf->get_mtime(), DateTime::INVALID_DATE_TIME);
  ASSERT_LE(stbuf->get_mtime(), now);
  ASSERT_EQ(stbuf->get_nlink(), 1);
  ASSERT_EQ(stbuf->get_size(), 0);
}

TEST_F(FileTest, sync) {
  get_write_file().write(
    get_test_string().data(),
    get_test_string().size()
  );

  if (!get_write_file().sync()) {
    throw Exception();
  }

  ASSERT_GE(
    get_write_file().stat()->get_size(),
    get_test_string().size()
  );
}

TEST_F(FileTest, tell) {
  ASSERT_EQ(get_read_file().tell(), 0);

  off_t seek_ret = get_read_file().seek(1024);
  ASSERT_EQ(seek_ret, 1024);
  ASSERT_EQ(get_read_file().tell(), seek_ret);

  seek_ret = get_read_file().seek(0);
  ASSERT_EQ(seek_ret, 0);
  ASSERT_EQ(get_read_file().tell(), seek_ret);
}

TEST_F(FileTest, truncate) {
  write();

  if (!get_write_file().truncate(0)) {
    throw Exception();
  }

  ASSERT_EQ(get_write_file().stat()->get_size(), 0);
}

TEST_F(FileTest, unlk) {
  if (get_write_file().setlkw(File::Lock(0, 256))) {
    if (!get_write_file().unlk(File::Lock(0, 256))) {
      throw Exception();
    }
  } else {
    throw Exception();
  }
}
}
}
