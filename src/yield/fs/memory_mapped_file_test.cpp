// yield/fs/memory_mapped_file_test.cpp

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


#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/memory_mapped_file.hpp"
#include "yunit.hpp"

#include <fcntl.h> // For O_*
#ifndef _WIN32
#include <sys/mman.h> // For MAP_* and PROT_*
#endif


TEST_SUITE(MemoryMappedFile);

namespace yield {
namespace fs {
class MemoryMappedFileTest : public yunit::Test {
public:
  virtual ~MemoryMappedFileTest() {
    FileSystem::dec_ref(file_system);
  }

protected:
  MemoryMappedFileTest()
    : test_file_name("mmf_test.bin"),
      test_string("test string") {
    file_system = NULL;
  }

  File& get_test_file() const {
    return *test_file;
  }
  const Path& get_test_file_name() const {
    return test_file_name;
  }
  const string& get_test_string() const {
    return test_string;
  }

  // yunit::Test
  void setup() {
    file_system = FileSystem::create();
    if (file_system != NULL) {
      file_system->unlink(get_test_file_name());

      test_file
      = file_system->open
        (
          get_test_file_name(),
          O_CREAT | O_TRUNC | O_RDWR | O_SYNC
        );

      if (test_file != NULL)
        return;
      else
        FileSystem::dec_ref(*file_system);
    }

    throw Exception();
  }

  void teardown() {
    if (file_system != NULL) {
      test_file->close();
      File::dec_ref(*test_file);
      file_system->unlink(get_test_file_name());
    }
  }

protected:
  FileSystem& get_file_system() const {
    return *file_system;
  }

private:
  File* test_file;
  Path test_file_name;
  string test_string;
  FileSystem* file_system;
};


TEST_EX(MemoryMappedFile, mmap, MemoryMappedFileTest) {
  auto_Object<MemoryMappedFile> mmf
  = get_file_system().mmap(get_test_file().inc_ref());
}

TEST_EX(MemoryMappedFile, read, MemoryMappedFileTest) {
  {
    auto_Object<MemoryMappedFile> mmf
    = get_file_system().mmap(get_test_file().inc_ref());

    mmf->reserve(get_test_string().size());

    memcpy_s
    (
      *mmf,
      get_test_string().size(),
      get_test_string().c_str(),
      get_test_string().size()
    );

    if (mmf->sync())
      mmf->close();
    else
      throw Exception();
  }

  {
    auto_Object<File> test_file = get_file_system().open(get_test_file_name());

    auto_Object<MemoryMappedFile> mmf
    = get_file_system().mmap
      (
        test_file->inc_ref(),
        NULL,
        FileSystem::MMAP_LENGTH_WHOLE_FILE,
        PROT_READ,
        MAP_PRIVATE
      );

    throw_assert_eq(mmf->capacity(), get_test_string().size());
    throw_assert_eq
    (
      strncmp(*mmf, get_test_string().data(), get_test_string().size()),
      0
    );
  }
}

TEST_EX(MemoryMappedFile, write, MemoryMappedFileTest) {
  auto_Object<MemoryMappedFile> mmf
  = get_file_system().mmap(get_test_file().inc_ref());

  mmf->reserve(get_test_string().size());

  memcpy_s
  (
    *mmf,
    get_test_string().size(),
    get_test_string().data(),
    get_test_string().size()
  );

  if (!mmf->sync(static_cast<size_t>(0), get_test_string().size()))
    throw Exception();
}
}
}
