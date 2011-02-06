// yield/fs/posix/file_test.hpp

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

#ifndef _YIELD_FS_POSIX_FILE_TEST_HPP_
#define _YIELD_FS_POSIX_FILE_TEST_HPP_

#include "extended_attributes_test.hpp"
#include "../file_test.hpp"

namespace yield {
namespace fs {
namespace posix {
class FileExtendedAttributesGetTest
  : public FileTest,
    private ExtendedAttributesGetTest {
public:
  FileExtendedAttributesGetTest(FilePairFactory& file_pair_factory)
    : FileTest(file_pair_factory)
  { }

  // yunit::Test
  void run() {
    ExtendedAttributesGetTest::run(get_write_file().openxattrs());
  }
};


class FileExtendedAttributesListTest
  : public FileTest,
    private ExtendedAttributesListTest {
public:
  FileExtendedAttributesListTest(FilePairFactory& file_pair_factory)
    : FileTest(file_pair_factory)
  { }

  // yunit::Test
  void run() {
    ExtendedAttributesListTest::run(get_write_file().openxattrs());
  }
};


class FileExtendedAttributesRemoveTest
  : public FileTest,
    private ExtendedAttributesRemoveTest {
public:
  FileExtendedAttributesRemoveTest(FilePairFactory& file_pair_factory)
    : FileTest(file_pair_factory)
  { }

  // yunit::Test
  void run() {
    ExtendedAttributesRemoveTest::run(get_write_file().openxattrs());
  }
};


class FileExtendedAttributesSetTest
  : public FileTest,
    private ExtendedAttributesSetTest {
public:
  FileExtendedAttributesSetTest(FilePairFactory& file_pair_factory)
    : FileTest(file_pair_factory)
  { }

  // yunit::Test
  void run() {
    ExtendedAttributesSetTest::run(get_write_file().openxattrs());
  }
};


template <class FileSystemType>
class FileTestSuite : public yield::fs::FileTestSuite<FileSystemType> {
public:
  FileTestSuite()
    ) {
    FilePairFactory& file_pair_factory
      = static_cast<FilePairFactory&>(get_channel_pair_factory());

    add
    (
      "File::getxattr",
      new FileExtendedAttributesGetTest(file_pair_factory)
    );

    add
    (
      "File::listxattr",
      new FileExtendedAttributesListTest(file_pair_factory)
    );

    add
    (
      "File::removexattr",
      new FileExtendedAttributesRemoveTest(file_pair_factory)
    );

    add
    (
      "File::setxattr",
      new FileExtendedAttributesSetTest(file_pair_factory)
    );
  }
};

}
}

#endif
