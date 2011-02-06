// yield/fs/posix/file_system_test.hpp

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

#ifndef _YIELD_FS_POSIX_FILE_SYSTEM_TEST_HPP_
#define _YIELD_FS_POSIX_FILE_SYSTEM_TEST_HPP_

#include "extended_attributes_test.hpp"
#include "../file_system_test.hpp"

namespace yield {
namespace fs {
namespace posix {
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
    uid_t uid = getuid();
    gid_t gid = getgid();

    if (get_file_system().chown(get_test_file_name(), uid, gid))
      return;
    else if (Exception::get_last_error_code() != ENOTSUP)
      throw Exception();
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
}

#endif
