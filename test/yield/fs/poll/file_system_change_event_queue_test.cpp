// fsce_queue_test.cpp

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
#include "yield/fs/file_system.hpp"
#include "yield/fs/poll/file_system_change_event_queue.hpp"
#include "yunit.hpp"

TEST_SUITE(FileSystemChangeEventQueue);

namespace yield {
namespace fs {
namespace poll {
class FileSystemChangeEventQueueTest : public yunit::Test {
public:
  FileSystemChangeEventQueueTest() {
    test_root_path = Path(".");
    test_directory_path = test_root_path / "dce_test";
    test_file_path = test_root_path / "dce_test.txt";
  }

  // yunit::Test
  void setup() {
    teardown();
  }

  void teardown() {
    FileSystem().rmtree(get_test_directory_path());
    throw_assert_false(FileSystem().exists(get_test_directory_path()));
    FileSystem().unlink(get_test_file_path());
    throw_assert_false(FileSystem().exists(get_test_file_path()));
  }

protected:
  const Path& get_test_directory_path() const {
    return test_directory_path;
  }

  const Path& get_test_file_path() const {
    return test_file_path;
  }

  const Path& get_test_root_path() const {
    return test_root_path;
  }

private:
  Path test_directory_path, test_file_path, test_root_path;
};


TEST_EX(
  FileSystemChangeEventQueue,
  associate,
  FileSystemChangeEventQueueTest
) {
  throw_assert(
    FileSystemChangeEventQueue().associate(get_test_directory_path())
  );
}

TEST_EX(
  FileSystemChangeEventQueue,
  directory_add,
  FileSystemChangeEventQueueTest
) {
  associate();

  if (!FileSystem().mkdir(get_test_directory_path()))
    throw Exception();

  poll(get_test_directory_path(), FileSystemChangeEvent::TYPE_DIRECTORY_ADD);
}

TEST_EX(
  FileSystemChangeEventQueue,
  directory_add_recursive,
  FileSystemChangeEventQueueTest
) {
  if (!FileSystem().mkdir(get_test_directory_path()))
    throw Exception();

  FileSystemChangeEventQueue fsce_queue;
  throw_assert(
    fsce_queue.associate(
      get_test_directory_path(),
      FileSystemChangeEvent::TYPE_ALL,
      true
    )
  );

  Path subdir_path = get_test_directory_path() / "subdir";
  if (!FileSystem().mkdir(subdir_path))
    throw Exception();

  auto_Object<FileSystemChangeEvent> fsce
    = fsce_queue->dequeue();
  throw_assert_eq(
    fsce->get_type(),
    FileSystemChangeEvent::TYPE_DIRECTORY_ADD
  );
}

TEST_EX(
  FileSystemChangeEventQueue,
  directory_remove,
  FileSystemChangeEventQueueTest
) {
  if (!FileSystem().mkdir(get_test_directory_path()))
    throw Exception();

  associate();

  if (!FileSystem().rmdir(get_test_directory_path()))
    throw Exception();

  poll(get_test_directory_path(), FileSystemChangeEvent::TYPE_DIRECTORY_REMOVE);
}

TEST_EX
(
  FileSystemChangeEventQueue,
  directory_remove_recursive,
  FileSystemChangeEventQueueTest
) {
  if (!FileSystem().mkdir(get_test_directory_path()))
    throw Exception();

  Path subdir_path = get_test_directory_path() / "subdir";
  if (!FileSystem().mkdir(subdir_path))
    throw Exception();

  associate();

  if (!FileSystem().rmdir(subdir_path))
    throw Exception();

  poll(subdir_path, FileSystemChangeEvent::TYPE_DIRECTORY_REMOVE);
}

TEST_EX(
  FileSystemChangeEventQueue,
  directory_rename,
  FileSystemChangeEventQueueTest
) {
  if (!FileSystem().mkdir(get_test_directory_path()))
    throw Exception();

  Path new_test_directory_path = get_test_root_path() / "test_dir_renamed";
  if (FileSystem().exists(new_test_directory_path))
    if (!FileSystem().rmdir(new_test_directory_path))
      throw Exception();

  associate();

  if (!FileSystem().rename(get_test_directory_path(), new_test_directory_path))
    throw Exception();

  poll
  (
    get_test_directory_path(),
    new_test_directory_path,
    FileSystemChangeEvent::TYPE_DIRECTORY_RENAME
  );

  FileSystem().rmdir(new_test_directory_path);
}

TEST_EX
(
  FileSystemChangeEventQueue,
  dissociate,
  FileSystemChangeEventQueueTest
) {
  associate();

  if (!FileSystem().touch(get_test_file_path()))
    throw Exception();

  get_volume_change_event_queue().dissociate(get_test_root_path());

  Time timeout(static_cast<uint64_t>(0));
  FileSystemChangeEvent volume_change_event;
  int poll_ret
  = get_volume_change_event_queue().poll(&volume_change_event, 1, timeout);
  ASSERT_EQ(poll_ret, 0);
}

TEST_EX(
  FileSystemChangeEventQueue,
  file_add,
  FileSystemChangeEventQueueTest
) {
  associate();

  if (!FileSystem().touch(get_test_file_path()))
    throw Exception();

  poll(get_test_file_path(), FileSystemChangeEvent::TYPE_FILE_ADD);
}

TEST_EX(
  FileSystemChangeEventQueue,
  file_add_recursive,
  FileSystemChangeEventQueueTest
) {
  if (!FileSystem().mkdir(get_test_directory_path()))
    throw Exception();

  associate();

  Path file_path = get_test_directory_path() / "file.txt";
  if (!FileSystem().touch(file_path))
    throw Exception();

  poll(file_path, FileSystemChangeEvent::TYPE_FILE_ADD);
}

TEST_EX(
  FileSystemChangeEventQueue,
  file_modify,
  FileSystemChangeEventQueueTest
) {
  if (!FileSystem().touch(get_test_file_path()))
    throw Exception();

  associate();

  if (!FileSystem().utime(get_test_file_path(), DateTime(), DateTime()))
    throw Exception();

  poll(get_test_file_path(), FileSystemChangeEvent::TYPE_FILE_MODIFY);
}

TEST_EX(
  FileSystemChangeEventQueue,
  file_remove,
  FileSystemChangeEventQueueTest
) {
  if (!FileSystem().touch(get_test_file_path()))
    throw Exception();

  associate();

  if (!FileSystem().unlink(get_test_file_path()))
    throw Exception();

  poll(get_test_file_path(), FileSystemChangeEvent::TYPE_FILE_REMOVE);
}

TEST_EX(
  FileSystemChangeEventQueue,
  file_remove_recursive,
  FileSystemChangeEventQueueTest
) {
  if (!FileSystem().mkdir(get_test_directory_path()))
    throw Exception();

  Path file_path = get_test_directory_path() / "file.txt";
  if (!FileSystem().touch(file_path))
    throw Exception();

  associate();

  if (!FileSystem().unlink(file_path))
    throw Exception();

  poll(file_path, FileSystemChangeEvent::TYPE_FILE_REMOVE);
}

TEST_EX(
  FileSystemChangeEventQueue,
  file_rename,
  FileSystemChangeEventQueueTest
) {
  if (!FileSystem().touch(get_test_file_path()))
    throw Exception();

  Path new_test_file_path = get_test_root_path() / "test_file_renamed.txt";
  if (FileSystem().exists(new_test_file_path))
    if (!FileSystem().unlink(new_test_file_path))
      throw Exception();

  associate();

  if (!FileSystem().rename(get_test_file_path(), new_test_file_path))
    throw Exception();

  poll
  (
    get_test_file_path(),
    new_test_file_path,
    FileSystemChangeEvent::TYPE_FILE_RENAME
  );

  FileSystem().unlink(new_test_file_path);
}
}
}
}
