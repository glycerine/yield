// directory_test.cpp

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
#include "yield/fs/directory.hpp"
#include "yield/fs/file_system.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace fs {
class DirectoryTest : public ::testing::Test {
protected:
  DirectoryTest()
    : test_dir_name("directory_test"),
      test_file_name("directory_test.txt"),
      test_file_path(test_dir_name / test_file_name) {
    directory = NULL;
  }

  // ::testing::Test
  void SetUp() {
    TearDown();

    if (!FileSystem().mkdir(get_test_dir_name())) {
      throw Exception();
    }

    if (!FileSystem().touch(get_test_file_path())) {
      throw Exception();
    }

    directory = FileSystem().opendir(get_test_dir_name());
    if (directory == NULL) {
      throw Exception();
    }
  }

  void TearDown() {
    Directory::dec_ref(directory);
    directory = NULL;

    if (FileSystem().exists(get_test_dir_name())) {
      if (!FileSystem().rmtree(get_test_dir_name())) {
        throw Exception();
      }
    }
  }

protected:
  Directory& get_directory() const {
    return *directory;
  }

  const Path& get_test_dir_name() const {
    return test_dir_name;
  }

  const Path& get_test_file_name() const {
    return test_file_name;
  }

  const Path& get_test_file_path() const {
    return test_file_path;
  }

private:
  Directory* directory;
  Path test_dir_name, test_file_name, test_file_path;
};


TEST_F(DirectoryTest, close) {
  if (!get_directory().close()) {
    throw Exception();
  }
}

#ifndef _WIN32
TEST_F(DirectoryTest, read_dev) {
  auto_Object<Directory> directory = FileSystem().opendir("/dev");
  Directory::Entry* dentry = directory->read();
  while (dentry != NULL) {
    Directory::Entry::dec_ref(*dentry);
    dentry = directory->read();
  }
}
#endif

TEST_F(DirectoryTest, read) {
  for (uint8_t i = 0; i < 3; i++) {
    auto_Object<Directory::Entry> dentry = get_directory().read();
    if (dentry->get_name() == Path::CURRENT_DIRECTORY) {
      ASSERT_TRUE(dentry->ISDIR());
      ASSERT_FALSE(dentry->is_hidden());
      ASSERT_TRUE(dentry->is_special());
    } else if (dentry->get_name() == Path::PARENT_DIRECTORY) {
      ASSERT_TRUE(dentry->ISDIR());
      ASSERT_FALSE(dentry->is_hidden());
      ASSERT_TRUE(dentry->is_special());
    } else {
      ASSERT_EQ(dentry->get_name(), get_test_file_name());
      ASSERT_TRUE(dentry->ISREG());
      ASSERT_FALSE(dentry->is_hidden());
      ASSERT_FALSE(dentry->is_special());
    }
  }
}

TEST_F(DirectoryTest, rewind) {
  for (uint8_t i = 0; i < 2; i++) {
    for (uint8_t j = 0; j < 3; j++) {
      auto_Object<Directory::Entry> dentry = get_directory().read();
      ASSERT_TRUE(
        dentry->get_name() == Path::CURRENT_DIRECTORY
        ||
        dentry->get_name() == Path::PARENT_DIRECTORY
        ||
        dentry->get_name() == get_test_file_name()
      );
    }

    get_directory().rewind();
  }
}
}
}
