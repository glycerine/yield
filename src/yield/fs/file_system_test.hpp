// yield/fs/file_system_test.hpp

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


#ifndef _YIELD_FS_FILE_SYSTEM_TEST_HPP_
#define _YIELD_FS_FILE_SYSTEM_TEST_HPP_


#include "extended_attributes_test.hpp"
#include "yield/auto_object.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/stat.hpp"
#include "yield/fs/file_system.hpp"
#include "yunit.hpp"

#include <fcntl.h>
#ifndef _WIN32
#include <sys/statvfs.h>
#endif


namespace yield {
namespace fs {
class FileSystemTest : public yunit::Test {
public:
  virtual ~FileSystemTest() { }

protected:
  FileSystemTest(FileSystem& file_system)
    : test_dir_name("file_system_test"),
      test_file_name("file_system_test.txt"),
      test_link_name("file_system_test_link.txt"),
      file_system(file_system.inc_ref())
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
  FileSystem& get_file_system() const {
    return file_system;
  }

  // yunit::Test
  void setup() {
    teardown();
    file_system.touch(get_test_file_name());
  }

  void teardown() {
    file_system.rmtree(get_test_dir_name());
    file_system.unlink(get_test_file_name());
    file_system.unlink(get_test_link_name());
  }

private:
  Path test_dir_name, test_file_name, test_link_name;
  FileSystem& file_system;
};


class FileSystemAccessTest : public FileSystemTest {
public:
  FileSystemAccessTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    throw_assert(get_file_system().access(get_test_file_name(), O_RDONLY));
  }
};


class FileSystemChmodTest : public FileSystemTest {
public:
  FileSystemChmodTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    mode_t mode = FileSystem::FILE_MODE_DEFAULT;
    if (get_file_system().chmod(get_test_file_name(), mode))
      return;
    else if (Exception::get_last_error_code() != ENOTSUP)
      throw Exception();
  }
};


class FileSystemChownTest : public FileSystemTest {
public:
  FileSystemChownTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    uid_t uid;
    gid_t gid;
#ifdef _WIN32
    uid = 0;
    gid = 0;
#else
    uid = getuid();
    gid = getgid();
#endif

    if (get_file_system().chown(get_test_file_name(), uid, gid))
      return;
    else if (Exception::get_last_error_code() != ENOTSUP)
      throw Exception();
  }
};


class FileSystemCreatTest : public FileSystemTest {
public:
  FileSystemCreatTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    File* file = get_file_system().creat(get_test_file_name());
    if (file != NULL)
      File::dec_ref(*file);
    else
      throw Exception();
  }
};


class FileSystemExistsTest : public FileSystemTest {
public:
  FileSystemExistsTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    if (!get_file_system().exists(get_test_file_name()))
      throw Exception();

    throw_assert_false(get_file_system().exists(Path("some other file.txt")));
  }
};


class FileSystemExtendedAttributesGetTest
  : public FileSystemTest,
    private ExtendedAttributesGetTest {
public:
  FileSystemExtendedAttributesGetTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    ExtendedAttributesGetTest::run
    (
      get_file_system().openxattrs(get_test_file_name())
    );
  }
};


class FileSystemExtendedAttributesListTest
  : public FileSystemTest,
    private ExtendedAttributesListTest {
public:
  FileSystemExtendedAttributesListTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    ExtendedAttributesListTest::run
    (
      get_file_system().openxattrs(get_test_file_name())
    );
  }
};


class FileSystemExtendedAttributesRemoveTest
  : public FileSystemTest,
    private ExtendedAttributesRemoveTest {
public:
  FileSystemExtendedAttributesRemoveTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    ExtendedAttributesRemoveTest::run
    (
      get_file_system().openxattrs(get_test_file_name())
    );
  }
};


class FileSystemExtendedAttributesSetTest
  : public FileSystemTest,
    private ExtendedAttributesSetTest {
public:
  FileSystemExtendedAttributesSetTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    ExtendedAttributesSetTest::run
    (
      get_file_system().openxattrs(get_test_file_name())
    );
  }
};


class FileSystemGetattrTest : public FileSystemTest {
public:
  FileSystemGetattrTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    auto_Object<Stat> stbuf = get_file_system().getattr(get_test_file_name());
  }
};


class FileSystemIsDirTest : public FileSystemTest {
public:
  FileSystemIsDirTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    get_file_system().mkdir(get_test_dir_name());
    throw_assert(get_file_system().isdir(get_test_dir_name()));
    throw_assert_false(get_file_system().isdir(get_test_file_name()));
    throw_assert_false(get_file_system().isdir(Path("nodir")));
  }
};


class FileSystemIsFileTest : public FileSystemTest {
public:
  FileSystemIsFileTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    throw_assert(get_file_system().isfile(get_test_file_name()));
    get_file_system().mkdir(get_test_dir_name());
    throw_assert_false(get_file_system().isfile(get_test_dir_name()));
    throw_assert_false(get_file_system().isfile(Path("nofile.txt")));
  }
};


class FileSystemLinkTest : public FileSystemTest {
public:
  FileSystemLinkTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    if (get_file_system().link(get_test_file_name(), get_test_link_name()))
      return;
    else if (Exception::get_last_error_code() != ENOTSUP)
      throw Exception();
  }
};


class FileSystemMkdirTest : public FileSystemTest {
public:
  FileSystemMkdirTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    if (!get_file_system().mkdir(get_test_dir_name()))
      throw Exception();
  }
};


class FileSystemMktreeTest : public FileSystemTest {
public:
  FileSystemMktreeTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    Path subdir_path(Path("file_system_test") + Path("subdir"));
    if (!get_file_system().mktree(subdir_path)) throw Exception();
    throw_assert(get_file_system().exists(subdir_path));
  }
};


class FileSystemOpenTest : public FileSystemTest {
public:
  FileSystemOpenTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    auto_Object<File> file = get_file_system().open(get_test_file_name());

    File* no_file = get_file_system().open(Path("nofile.txt"), O_RDONLY);
    if (no_file != NULL) {
      File::dec_ref(*no_file);
      throw_assert(false);
    }
  }
};


class FileSystemReadLinkTest : public FileSystemTest {
public:
  FileSystemReadLinkTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    if (get_file_system().symlink(get_test_file_name(), get_test_link_name())) {
      Path target_path;
      if (get_file_system().readlink(get_test_link_name(), target_path)) {
        throw_assert_eq(target_path, get_test_file_name());
      } else if (Exception::get_last_error_code() != ENOTSUP)
        throw Exception();
    } else if (Exception::get_last_error_code() != ENOTSUP)
      throw Exception();
  }
};


class FileSystemRealPathTest : public FileSystemTest {
public:
  FileSystemRealPathTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // yunit::Test
  void run() {
    Path realpath;
    if (!get_file_system().realpath(get_test_file_name(), realpath))
      throw Exception();
    throw_assert_false(realpath.empty());
    throw_assert_ne(get_test_file_name(), realpath);
    throw_assert_lt(get_test_file_name().size(), realpath.size());
  }
};


class FileSystemRenameTest : public FileSystemTest {
public:
  FileSystemRenameTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    if (!get_file_system().rename(get_test_file_name(), Path("file_system_test2.txt")))
      throw Exception();
    get_file_system().unlink(Path("file_system_test2.txt"));
  }
};


class FileSystemRmDirTest : public FileSystemTest {
public:
  FileSystemRmDirTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    get_file_system().mkdir(get_test_dir_name());
    if (!get_file_system().rmdir(get_test_dir_name()))
      throw Exception();
  }
};


class FileSystemRmTreeTest : public FileSystemTest {
public:
  FileSystemRmTreeTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    if (!get_file_system().mkdir(get_test_dir_name()))
      throw Exception();

    if (!get_file_system().touch(get_test_dir_name() / get_test_file_name()))
      throw Exception();

    if (!get_file_system().rmtree(get_test_dir_name()))
      throw Exception();

    throw_assert_false(get_file_system().exists(get_test_dir_name()));
  }
};


class FileSystemStatVFSTest : public FileSystemTest {
public:
  FileSystemStatVFSTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    get_file_system().mkdir(get_test_dir_name());
    struct statvfs stbuf;
    if (!get_file_system().statvfs(get_test_dir_name(), stbuf))
      throw Exception();
    throw_assert_gt(stbuf.f_bsize, 0);
    throw_assert_gt(stbuf.f_blocks, 0);
    throw_assert_gt(stbuf.f_bfree, 0);
    throw_assert_ge(stbuf.f_blocks, stbuf.f_bfree);
    throw_assert_ge(stbuf.f_namemax, 0);
  }
};


class FileSystemTouchTest : public FileSystemTest {
public:
  FileSystemTouchTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    if (!get_file_system().touch(get_test_file_name()))
      throw Exception();

    if (get_file_system().touch(get_test_dir_name() / get_test_file_name())) {
      throw_assert(false);
    }
  }
};


class FileSystemSymlinkTest : public FileSystemTest {
public:
  FileSystemSymlinkTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    if (get_file_system().symlink(get_test_file_name(), get_test_link_name()))
      return;
    else if (Exception::get_last_error_code() != ENOTSUP)
      throw Exception();
  }
};


class FileSystemTruncateTest : public FileSystemTest {
public:
  FileSystemTruncateTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    {
      auto_Object<File> file
      = get_file_system().open(get_test_file_name(), O_WRONLY);
      ssize_t write_ret = file->write("test", 4);
      if (write_ret != 4) throw Exception();
      if (!file->sync()) throw Exception();
      auto_Object<Stat> stbuf = get_file_system().getattr(get_test_file_name());
      throw_assert_eq(stbuf->get_size(), 4);
    }

    if (get_file_system().truncate(get_test_file_name(), 0)) {
      auto_Object<Stat> stbuf = get_file_system().getattr(get_test_file_name());
      throw_assert_eq(stbuf->get_size(), 0);
    } else
      throw Exception();
  }
};


class FileSystemUnlinkTest : public FileSystemTest {
public:
  FileSystemUnlinkTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    if (!get_file_system().unlink(get_test_file_name()))
      throw Exception();
  }
};


class FileSystemUtimeTest : public FileSystemTest {
public:
  FileSystemUtimeTest(FileSystem& file_system)
    : FileSystemTest(file_system)
  { }

  // Test
  void run() {
    DateTime atime = DateTime::now(),
             mtime = DateTime::now();

    if (get_file_system().utime(get_test_file_name(), atime, mtime)) {
      auto_Object<Stat> stbuf = get_file_system().getattr(get_test_file_name());
      throw_assert_le(stbuf->get_atime() - atime, Time::NS_IN_S);
      throw_assert_le(stbuf->get_mtime() - mtime, Time::NS_IN_S);
    } else
      throw Exception();

    atime = DateTime::now();
    mtime = DateTime::now();
    DateTime ctime = DateTime::now();

    if (get_file_system().utime(get_test_file_name(), atime, mtime, ctime)) {
      auto_Object<Stat> stbuf = get_file_system().getattr(get_test_file_name());
      throw_assert_le(stbuf->get_atime() - atime, Time::NS_IN_S);
      throw_assert_le(stbuf->get_mtime() - mtime, Time::NS_IN_S);
      throw_assert_le(stbuf->get_ctime() - ctime, Time::NS_IN_S);
    } else if (Exception::get_last_error_code() != ENOTSUP)
      throw Exception();
  }
};


template <class FileSystemType>
class FileSystemTestSuite : public yunit::TestSuite {
public:
  FileSystemTestSuite(YO_NEW_REF FileSystemType* file_system = NULL) {
    if (file_system == NULL)
      file_system = new FileSystemType;

    add("FileSystem::access", new FileSystemAccessTest(*file_system));
    add("FileSystem::chmod", new FileSystemChmodTest(*file_system));
    add("FileSystem::chown", new FileSystemChownTest(*file_system));

    add("FileSystem::creat", new FileSystemCreatTest(*file_system));

    add("FileSystem::exists", new FileSystemExistsTest(*file_system));

    add("FileSystem::getattr", new FileSystemGetattrTest(*file_system));

    add
    (
      "FileSystem::getxattr",
      new FileSystemExtendedAttributesGetTest(*file_system)
    );

    add("FileSystem::isdir", new FileSystemIsDirTest(*file_system));
    add("FileSystem::isfile", new FileSystemIsFileTest(*file_system));

    add
    (
      "FileSystem::listxattr",
      new FileSystemExtendedAttributesListTest(*file_system)
    );

    add("FileSystem::link", new FileSystemLinkTest(*file_system));

    add("FileSystem::mkdir", new FileSystemMkdirTest(*file_system));
    add("FileSystem::mktree", new FileSystemMktreeTest(*file_system));

    add("FileSystem::open", new FileSystemOpenTest(*file_system));

    add("FileSystem::readlink", new FileSystemReadLinkTest(*file_system));

    add("FileSystem::realpath", new FileSystemRealPathTest(*file_system));

    add
    (
      "FileSystem::removexattr",
      new FileSystemExtendedAttributesRemoveTest(*file_system)
    );

    add("FileSystem::rename", new FileSystemRenameTest(*file_system));

    add("FileSystem::rmdir", new FileSystemRmDirTest(*file_system));
    add("FileSystem::rmtree", new FileSystemRmTreeTest(*file_system));

    add
    (
      "FileSystem::setxattr",
      new FileSystemExtendedAttributesSetTest(*file_system)
    );

    add("FileSystem::statvfs", new FileSystemStatVFSTest(*file_system));

    add("FileSystem::symlink", new FileSystemSymlinkTest(*file_system));

    add("FileSystem::touch", new FileSystemTouchTest(*file_system));

    add("FileSystem::truncate", new FileSystemTruncateTest(*file_system));

    add("FileSystem::unlink", new FileSystemUnlinkTest(*file_system));

    add("FileSystem::utime", new FileSystemUtimeTest(*file_system));

    FileSystem::dec_ref(*file_system);
  }
};
}
}


#endif
