// yield/fs/posix/file_system_test.cpp

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

#include "extended_attributes_test.hpp"
#include "../file_system_test.hpp"
#include "file_system.hpp"


TEST_SUITE_EX(POSIXFileSystem, yield::fs::FileSystemTestSuite);

namespace yield {
namespace fs {
namespace posix {
TEST_EX(POSIXFileSystem, access, FileSystemTest) {
  throw_assert(FileSystem().access(get_test_file_name(), O_RDONLY));
}

TEST_EX(POSIXFileSystem, chmod, FileSystemTest) {
  mode_t mode = FileSystem::FILE_MODE_DEFAULT;
  if (!FileSystem().chmod(get_test_file_name(), mode))
    throw Exception();
}

TEST_EX(POSIXFileSystem, chown, FileSystemTest) {
  uid_t uid = getuid();
  gid_t gid = getgid();

  if (!FileSystem().chown(get_test_file_name(), uid, gid))
    throw Exception();
}

TEST_EX(POSIXFileSystem, getxattr, FileSystemTest) {
  ExtendedAttributesGetTest().run(
    FileSystem().openxattrs(gettest_file_name())
  );
}

TEST_EX(POSIXFileSystem, listxattr, FileSystemTest) {
  ExtendedAttributesListTest().run(
    FileSystem().openxattrs(get_test_file_name())
  );
}

TEST_EX(POSIXFileSystem, readlink, FileSystemTest) {
  if (FileSystem().symlink(get_test_file_name(), get_test_link_name())) {
    Path target_path;
    if (FileSystem().readlink(get_test_link_name(), target_path)) {
      throw_assert_eq(target_path, get_test_file_name());
      return;
    }
  }

  throw Exception();
}

TEST_EX(POSIXFileSystem, removexattr, FileSystemTest) {
  ExtendedAttributesRemoveTest().run(
    FileSystem().openxattrs(get_test_file_name())
  );
}

TEST_EX(POSIXFileSystem, setxattr, FileSystemTest) {
  ExtendedAttributesSetTest().run(
    FileSystem().openxattrs(get_test_file_name())
  );
}

TEST_EX(POSIXFileSystem, symlink, FileSystemTest) {
    if (!FileSystem().symlink(get_test_file_name(), get_test_link_name()))
      throw Exception();
}
}
}
}