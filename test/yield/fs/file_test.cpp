// file_test.cpp

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

#include "../channel_test.hpp"
#include "yield/auto_object.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/stat.hpp"

#include <fcntl.h> // For O_*

TEST_SUITE(File);

namespace yield {
namespace fs {
class FilePair : public ChannelPair {
public:
  FilePair(const Path& path) : path(path) {
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
      if (read_file == NULL) throw Exception();
    }

    return *read_file;
  }

  File& get_write_file() {
    if (write_file == NULL) {
      write_file = FileSystem().open(path, O_CREAT | O_TRUNC | O_WRONLY);
      if (write_file == NULL) throw Exception();
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


class FilePairFactory : public ChannelPairFactory {
public:
  FilePairFactory(const Path& path) : path(path) {
  }

public:
  // yield::ChannelPairFactory
  ChannelPair& create_channel_pair() {
    return *new FilePair(path);
  }

private:
  Path path;
};


class FileTest : public ChannelTest {
public:
  FileTest()
    : ChannelTest(*new FilePairFactory("file_test.txt"))
  { }

public:
  File& get_read_file() {
    return static_cast<File&>(get_read_channel());
  }

  File& get_write_file() {
    return static_cast<File&>(get_write_channel());
  }
};


TEST_EX(File, datasync, FileTest) {
  get_write_file().write(
    get_test_string().data(),
    get_test_string().size()
  );

  if (!get_write_file().datasync())
    throw Exception();

  throw_assert_ge(
    get_write_file().stat()->get_size(),
    get_test_string().size()
  );
}

TEST_EX(File, dup, FileTest) {
  File* dup_file = get_read_file().dup();
  if (dup_file != NULL)
    File::dec_ref(*dup_file);
  else
    throw Exception();

  dup_file = File::dup(static_cast<fd_t>(get_read_file()));
  if (dup_file != NULL)
    File::dec_ref(*dup_file);
  else
    throw Exception();
}

#ifndef _WIN32
TEST_EX(File, getlk, FileTest) {
  if (!get_write_file().setlk(File::Lock(0, 256)))
    throw Exception();

  File::Lock* lock = get_write_file().getlk(File::Lock(0, 256));
  if (lock != NULL)
    File::Lock::dec_ref(*lock);
  else
    throw Exception();
}
#endif

TEST_EX(File, map, FileTest) {
  auto_Object<File::Map> mmf = get_read_file().mmap();
}

//class FileMapReadTest : public FileTest {
//public:
//  FileMapReadTest(FilePairFactory& file_pair_factory)
//    : FileTest(file_pair_factory)
//  { }
//
//  // yunit::Test
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
//      throw_assert_eq(mmf->capacity(), get_test_string().size());
//      throw_assert_eq
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
//  // yunit::Test
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

TEST_EX(File, pread, FileTest) {
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
    throw_assert_eq(static_cast<size_t>(pread_ret), _string.size());
    throw_assert_eq(_string, " string");
  } else
    throw Exception();
}

TEST_EX(File, pread_Buffer, FileTest) {
  write();

  auto_Object<Buffer> buffer = new Buffer(Buffer::getpagesize(), 7);
  ssize_t pread_ret = get_read_file().pread(*buffer, 4);
  if (pread_ret >= 0) {
    throw_assert_eq(pread_ret, 7);
    throw_assert_eq(*buffer, " string");
  } else
    throw Exception();
}

TEST_EX(File, pread_Buffers, FileTest) {
  auto_Object<Buffer> _str = new Buffer(Buffer::getpagesize(), 4);
  auto_Object<Buffer> ing = new Buffer(Buffer::getpagesize(), 7);
  _str->set_next_buffer(ing->inc_ref());

  ssize_t pread_ret = get_read_file().pread(*_str, 4);
  if (pread_ret >= 0) {
    throw_assert_eq(pread_ret, 7);
    throw_assert_eq(*_str, " str");
    throw_assert_eq(*ing, "ing");
  } else
    throw Exception();
}

TEST_EX(File, preadv_one, FileTest) {
  write();

  string _string;
  _string.resize(7);
  iovec iov;
  iov.iov_base = const_cast<char*>(_string.data());
  iov.iov_len = _string.size();

  ssize_t preadv_ret = get_read_file().preadv(&iov, 1, 4);

  if (preadv_ret >= 0) {
    throw_assert_eq(static_cast<size_t>(preadv_ret), _string.size());
    throw_assert_eq(_string, " string");
  } else
    throw Exception();
}

TEST_EX(File, preadv_two, FileTest) {
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
    throw_assert_eq(static_cast<size_t>(preadv_ret), _string.size());
    throw_assert_eq(_string, " string");
  } else
    throw Exception();
}

TEST_EX(File, pwrite, FileTest) {
  ssize_t pwrite_ret = get_write_file().pwrite(" string", 7, 4);
  if (pwrite_ret >= 0) {
    throw_assert_eq(pwrite_ret, 7);
  } else
    throw Exception();

  pwrite_ret = get_write_file().pwrite("test", 4, 0);
  if (pwrite_ret >= 0) {
    throw_assert_eq(pwrite_ret, 4);
  } else
    throw Exception();

  read();
}

TEST_EX(File, pwrite_Buffer, FileTest) {
  auto_Object<Buffer> buffer = Buffer::copy(" string");
  ssize_t pwrite_ret = get_write_file().pwrite(*buffer, 4);
  if (pwrite_ret >= 0) {
    throw_assert_eq(pwrite_ret, 7);
  } else
    throw Exception();

  pwrite_ret = get_write_file().pwrite("test", 4, 0);
  if (pwrite_ret >= 0) {
    throw_assert_eq(pwrite_ret, 4);
  } else
    throw Exception();

  read();
}

TEST_EX(File, pwrite_Buffers, FileTest) {
  auto_Object<Buffer> _str = Buffer::copy(" str");
  auto_Object<Buffer> ing = Buffer::copy("ing");
  _str->set_next_buffer(ing->inc_ref());

  ssize_t pwrite_ret = get_write_file().pwrite(*_str, 4);
  if (pwrite_ret >= 0) {
    throw_assert_eq(pwrite_ret, 7);
  } else
    throw Exception();

  pwrite_ret = get_write_file().pwrite("test", 4, 0);
  if (pwrite_ret >= 0) {
    throw_assert_eq(pwrite_ret, 4);
  } else
    throw Exception();

  read();
}

TEST_EX(File, pwritev_one, FileTest) {
  iovec iov;
  iov.iov_base = const_cast<char*>(" string");
  iov.iov_len = 7;
  ssize_t pwritev_ret = get_write_file().pwritev(&iov, 1, 4);
  if (pwritev_ret >= 0) {
    throw_assert_eq(pwritev_ret, 7);
  } else
    throw Exception();

  iov.iov_base = const_cast<char*>("test");
  iov.iov_len = 4;
  pwritev_ret = get_write_file().pwritev(&iov, 1, 0);
  if (pwritev_ret >= 0) {
    throw_assert_eq(pwritev_ret, 4);
  } else
    throw Exception();

  read();
}

TEST_EX(File, pwritev_two, FileTest) {
  ssize_t pwrite_ret = get_write_file().pwrite(" string", 7, 4);
  if (pwrite_ret >= 0) {
    throw_assert_eq(pwrite_ret, 7);
  } else
    throw Exception();

  iovec iov[2];
  iov[0].iov_base = const_cast<char*>("te");
  iov[0].iov_len = 2;
  iov[1].iov_base = const_cast<char*>("st");
  iov[1].iov_len = 2;
  ssize_t pwritev_ret = get_write_file().pwritev(iov, 2, 0);
  if (pwritev_ret >= 0) {
    throw_assert_eq(pwritev_ret, 4);
  } else
    throw Exception();

  read();
}

TEST_EX(File, seek, FileTest) {
  write();

  // SEEK_SET
  uint64_t seek_ret = get_read_file().seek(4);
  throw_assert_eq(seek_ret, 4);

  // SEEK_CUR
  seek_ret = get_read_file().seek(2, SEEK_CUR);
  throw_assert_eq(seek_ret, 6);

  // SEEK_END
  seek_ret = get_read_file().seek(-1, SEEK_END);
  throw_assert_eq(seek_ret, get_test_string().size() - 1);

  seek_ret = get_read_file().seek(1, SEEK_END);
  throw_assert_eq(seek_ret, get_test_string().size() + 1);

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
    throw_assert_eq(read_ret, 7);
    throw_assert_eq(_string, " string");
  } else
    throw Exception();
}

TEST_EX(File, setlk, FileTest) {
  if (!get_write_file().setlk(File::Lock(0, 256)))
    throw Exception();
}

TEST_EX(File, setlkw, FileTest) {
  if (!get_write_file().setlkw(File::Lock(0, 256)))
    throw Exception();
}

TEST_EX(File, stat, FileTest) {
  DateTime now = DateTime::now();
  auto_Object<Stat> stbuf = get_write_file().stat();
  throw_assert_ne(stbuf->get_atime(), DateTime::INVALID_DATE_TIME);
  throw_assert_le(stbuf->get_atime(), now);
  throw_assert_ne(stbuf->get_ctime(), DateTime::INVALID_DATE_TIME);
  throw_assert_le(stbuf->get_ctime(), now);
  throw_assert_ne(stbuf->get_mtime(), DateTime::INVALID_DATE_TIME);
  throw_assert_le(stbuf->get_mtime(), now);
  throw_assert_eq(stbuf->get_nlink(), 1);
  throw_assert_eq(stbuf->get_size(), 0);
}

TEST_EX(File, sync, FileTest) {
  get_write_file().write(
    get_test_string().data(),
    get_test_string().size()
  );

  if (!get_write_file().sync())
    throw Exception();

  throw_assert_ge(
    get_write_file().stat()->get_size(),
    get_test_string().size()
  );
}

TEST_EX(File, tell, FileTest) {
  throw_assert_eq(get_read_file().tell(), 0);

  off_t seek_ret = get_read_file().seek(1024);
  throw_assert_eq(seek_ret, 1024);
  throw_assert_eq(get_read_file().tell(), seek_ret);

  seek_ret = get_read_file().seek(0);
  throw_assert_eq(seek_ret, 0);
  throw_assert_eq(get_read_file().tell(), seek_ret);
}

TEST_EX(File, truncate, FileTest) {
  write();

  if (!get_write_file().truncate(0))
    throw Exception();

  throw_assert_eq(get_write_file().stat()->get_size(), 0);
}

TEST_EX(File, unlk, FileTest) {
  if (get_write_file().setlkw(File::Lock(0, 256))) {
    if (!get_write_file().unlk(File::Lock(0, 256)))
      throw Exception();
  } else
    throw Exception();
}
}
}
