// file_system_change_event_queue_test.cpp

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

#include <memory>
using std::auto_ptr;

#include "yield/platform.h"
using namespace yield::platform;

#include "yunit.h"


namespace yield {
namespace platform {
class VolumeChangeEventQueueTest : public yunit::Test {
public:
  VolumeChangeEventQueueTest() {
    test_root_path = Path(".");
    test_directory_path = test_root_path / "dce_test";
    test_file_path = test_root_path / "dce_test.txt";
    volume_change_event_queue = NULL;
  }

  // yunit::Test
  void setup() {
    volume = Volume::create();
    if (volume != NULL) {
      volume_change_event_queue = &VolumeChangeEventQueue::create();
      volume->rmtree(get_test_directory_path());
      if (volume->exists(get_test_directory_path())) FAIL();
      volume->unlink(get_test_file_path());
      if (volume->exists(get_test_file_path())) FAIL();
    }
  }

  void teardown() {
    if (volume != NULL) {
      delete volume_change_event_queue;
      volume->rmtree(get_test_directory_path());
      volume->unlink(get_test_file_path());
      Volume::dec_ref(*volume);
    }
  }

protected:
  VolumeChangeEventQueue& get_volume_change_event_queue() {
    return *volume_change_event_queue;
  }

  void
  associate
  (
    VolumeChangeEvent::Type events = VolumeChangeEvent::TYPE_ALL
  ) {
    if (!volume_change_event_queue->associate(test_root_path, events))
      throw Exception();
  }

  const Path& get_test_directory_path() const {
    return test_directory_path;
  }

  const Path& get_test_file_path() const {
    return test_file_path;
  }
  const Path& get_test_root_path() const {
    return test_root_path;
  }
  Volume& get_volume() const {
    return *volume;
  }

  void
  poll
  (
    const Path& expected_path,
    VolumeChangeEvent::Type expected_type
  ) {
    VolumeChangeEvent volume_change_event;
    int poll_ret
    = get_volume_change_event_queue().poll(volume_change_event);

    ASSERT_EQ(poll_ret, 1);
    ASSERT_EQ(volume_change_event.get_path(), expected_path);
    ASSERT_EQ(volume_change_event.get_type(), expected_type);
  }

  void
  poll
  (
    const Path& expected_old_path,
    const Path& expected_new_path,
    VolumeChangeEvent::Type expected_type
  ) {
    VolumeChangeEvent volume_change_event;
    int poll_ret
    = get_volume_change_event_queue().poll(volume_change_event);

    ASSERT_EQ(poll_ret, 1);
    ASSERT_EQ(volume_change_event.get_old_path(), expected_old_path);
    ASSERT_EQ(volume_change_event.get_new_path(), expected_new_path);
    ASSERT_EQ(volume_change_event.get_type(), expected_type);
  }

private:
  Path test_directory_path, test_file_path, test_root_path;
  Volume* volume;
  VolumeChangeEventQueue* volume_change_event_queue;
};
};
};


TEST_SUITE(VolumeChangeEventQueue);

TEST_EX
(
  VolumeChangeEventQueue,
  associate,
  VolumeChangeEventQueueTest
) {
  associate();
}

TEST_EX
(
  VolumeChangeEventQueue,
  directory_add,
  VolumeChangeEventQueueTest
) {
  associate();

  if (!get_volume().mkdir(get_test_directory_path()))
    throw Exception();

  poll(get_test_directory_path(), VolumeChangeEvent::TYPE_DIRECTORY_ADD);
}

TEST_EX
(
  VolumeChangeEventQueue,
  directory_add_recursive,
  VolumeChangeEventQueueTest
) {
  if (!get_volume().mkdir(get_test_directory_path()))
    throw Exception();

  associate();

  Path subdir_path = get_test_directory_path() / "subdir";
  if (!get_volume().mkdir(subdir_path))
    throw Exception();

  poll(subdir_path, VolumeChangeEvent::TYPE_DIRECTORY_ADD);
}

TEST_EX
(
  VolumeChangeEventQueue,
  directory_remove,
  VolumeChangeEventQueueTest
) {
  if (!get_volume().mkdir(get_test_directory_path()))
    throw Exception();

  associate();

  if (!get_volume().rmdir(get_test_directory_path()))
    throw Exception();

  poll(get_test_directory_path(), VolumeChangeEvent::TYPE_DIRECTORY_REMOVE);
}

TEST_EX
(
  VolumeChangeEventQueue,
  directory_remove_recursive,
  VolumeChangeEventQueueTest
) {
  if (!get_volume().mkdir(get_test_directory_path()))
    throw Exception();

  Path subdir_path = get_test_directory_path() / "subdir";
  if (!get_volume().mkdir(subdir_path))
    throw Exception();

  associate();

  if (!get_volume().rmdir(subdir_path))
    throw Exception();

  poll(subdir_path, VolumeChangeEvent::TYPE_DIRECTORY_REMOVE);
}

TEST_EX
(
  VolumeChangeEventQueue,
  directory_rename,
  VolumeChangeEventQueueTest
) {
  if (!get_volume().mkdir(get_test_directory_path()))
    throw Exception();

  Path new_test_directory_path = get_test_root_path() / "test_dir_renamed";
  if (get_volume().exists(new_test_directory_path))
    if (!get_volume().rmdir(new_test_directory_path))
      throw Exception();

  associate();

  if (!get_volume().rename(get_test_directory_path(), new_test_directory_path))
    throw Exception();

  poll
  (
    get_test_directory_path(),
    new_test_directory_path,
    VolumeChangeEvent::TYPE_DIRECTORY_RENAME
  );

  get_volume().rmdir(new_test_directory_path);
}

TEST_EX
(
  VolumeChangeEventQueue,
  dissociate,
  VolumeChangeEventQueueTest
) {
  associate();

  if (!get_volume().touch(get_test_file_path()))
    throw Exception();

  get_volume_change_event_queue().dissociate(get_test_root_path());

  Time timeout(static_cast<uint64_t>(0));
  VolumeChangeEvent volume_change_event;
  int poll_ret
  = get_volume_change_event_queue().poll(&volume_change_event, 1, timeout);
  ASSERT_EQ(poll_ret, 0);
}

TEST_EX
(
  VolumeChangeEventQueue,
  file_add,
  VolumeChangeEventQueueTest
) {
  associate();

  if (!get_volume().touch(get_test_file_path()))
    throw Exception();

  poll(get_test_file_path(), VolumeChangeEvent::TYPE_FILE_ADD);
}

TEST_EX
(
  VolumeChangeEventQueue,
  file_add_recursive,
  VolumeChangeEventQueueTest
) {
  if (!get_volume().mkdir(get_test_directory_path()))
    throw Exception();

  associate();

  Path file_path = get_test_directory_path() / "file.txt";
  if (!get_volume().touch(file_path))
    throw Exception();

  poll(file_path, VolumeChangeEvent::TYPE_FILE_ADD);
}

TEST_EX
(
  VolumeChangeEventQueue,
  file_modify,
  VolumeChangeEventQueueTest
) {
  if (!get_volume().touch(get_test_file_path()))
    throw Exception();

  associate();

  if (!get_volume().utime(get_test_file_path(), DateTime(), DateTime()))
    throw Exception();

  poll(get_test_file_path(), VolumeChangeEvent::TYPE_FILE_MODIFY);
}

TEST_EX
(
  VolumeChangeEventQueue,
  file_remove,
  VolumeChangeEventQueueTest
) {
  if (!get_volume().touch(get_test_file_path()))
    throw Exception();

  associate();

  if (!get_volume().unlink(get_test_file_path()))
    throw Exception();

  poll(get_test_file_path(), VolumeChangeEvent::TYPE_FILE_REMOVE);
}

TEST_EX
(
  VolumeChangeEventQueue,
  file_remove_recursive,
  VolumeChangeEventQueueTest
) {
  if (!get_volume().mkdir(get_test_directory_path()))
    throw Exception();

  Path file_path = get_test_directory_path() / "file.txt";
  if (!get_volume().touch(file_path))
    throw Exception();

  associate();

  if (!get_volume().unlink(file_path))
    throw Exception();

  poll(file_path, VolumeChangeEvent::TYPE_FILE_REMOVE);
}

TEST_EX
(
  VolumeChangeEventQueue,
  file_rename,
  VolumeChangeEventQueueTest
) {
  if (!get_volume().touch(get_test_file_path()))
    throw Exception();

  Path new_test_file_path = get_test_root_path() / "test_file_renamed.txt";
  if (get_volume().exists(new_test_file_path))
    if (!get_volume().unlink(new_test_file_path))
      throw Exception();

  associate();

  if (!get_volume().rename(get_test_file_path(), new_test_file_path))
    throw Exception();

  poll
  (
    get_test_file_path(),
    new_test_file_path,
    VolumeChangeEvent::TYPE_FILE_RENAME
  );

  get_volume().unlink(new_test_file_path);
}
