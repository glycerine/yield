// fs_event_queue_test.hpp

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

#ifndef _YIELD_FS_POLL_FS_EVENT_QUEUE_TEST_HPP_
#define _YIELD_FS_POLL_FS_EVENT_QUEUE_TEST_HPP_

#include "../../event_queue_test.hpp"
#include "yield/auto_object.hpp"
#include "yield/debug.hpp"
#include "yield/date_time.hpp"
#include "yield/exception.hpp"
#include "yield/log.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/poll/fs_event.hpp"
#include "yield/thread/thread.hpp"
#include "gtest/gtest.h"

namespace yield {
namespace fs {
namespace poll {
template <class TypeParam>
class FSEventQueueTest : public ::testing::Test {
public:
  FSEventQueueTest() {
    test_root_path = Path(".");
    test_directory_path = test_root_path / "dce_test";
    test_file_path = test_root_path / "dce_test.txt";
  }

  // ::testing::Test
  void SetUp() {
    TearDown();
  }

  void TearDown() {
    FileSystem().rmtree(get_test_directory_path());
    ASSERT_FALSE(FileSystem().exists(get_test_directory_path()));
    FileSystem().unlink(get_test_file_path());
    ASSERT_FALSE(FileSystem().exists(get_test_file_path()));
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

TYPED_TEST_CASE_P(FSEventQueueTest);

TYPED_TEST_P(FSEventQueueTest, associate) {
  if (!TypeParam().associate(this->get_test_root_path())) {
    throw Exception();
  }
}

TYPED_TEST_P(FSEventQueueTest, associate_change) {
  TypeParam fs_event_queue(&Log::open(std::cout));

  if (!fs_event_queue.associate(this->get_test_root_path())) {
    throw Exception();
  }

  if (!fs_event_queue.associate(this->get_test_root_path())) {
    throw Exception();
  }

  if (
    !fs_event_queue.associate(
      this->get_test_root_path(),
      FSEvent::TYPE_FILE_ADD
    )
  ) {
    throw Exception();
  }
}

TYPED_TEST_P(FSEventQueueTest, associate_dir_dequeue_dir_add) {
  TypeParam fs_event_queue(&Log::open(std::cout));
  if (
    !fs_event_queue.associate(
      this->get_test_root_path(),
      FSEvent::TYPE_DIRECTORY_ADD
    )
  ) {
    throw Exception();
  }

  if (!FileSystem().mkdir(this->get_test_directory_path())) {
    throw Exception();
  }

  auto_Object<FSEvent> fs_event =
    Object::cast<FSEvent>(fs_event_queue.dequeue());
  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_ADD);
  ASSERT_EQ(fs_event->get_path(), this->get_test_directory_path());
}

TYPED_TEST_P(FSEventQueueTest, associate_dir_dequeue_dir_modify) {
  if (!FileSystem().mkdir(this->get_test_directory_path())) {
    throw Exception();
  }

  TypeParam fs_event_queue(&Log::open(std::cout));
  if (
    !fs_event_queue.associate(
      this->get_test_root_path(),
      FSEvent::TYPE_DIRECTORY_MODIFY
    )
  ) {
    throw Exception();
  }

  yield::thread::Thread::sleep(1.0);

  DateTime atime = DateTime::now(), mtime = atime;
  if (!FileSystem().utime(this->get_test_directory_path(), atime, mtime)) {
    throw Exception();
  }

  auto_Object<FSEvent> fs_event =
    Object::cast<FSEvent>(fs_event_queue.dequeue());
  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_MODIFY);
  ASSERT_EQ(fs_event->get_path(), this->get_test_directory_path());
}

//TEST_EX(FSEventQueue, directory_add_recursive, FSEventQueueTest) {
//  if (!FileSystem().mkdir(get_test_directory_path()))
//    throw Exception();
//
//  TypeParam fs_event_queue(&Log::open(std::cout));
//  if (!fs_event_queue.associate(get_test_root_path()))
//    throw Exception();
//
//  Path test_subdirectory_path = get_test_directory_path() / "subdir";
//  if (!FileSystem().mkdir(test_subdirectory_path))
//    throw Exception();
//
//  auto_Object<FSEvent> fs_event =
//    Object::cast<FSEvent>(fs_event_queue.dequeue());
//  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_ADD);
//  ASSERT_EQ(fs_event->get_path(), test_subdirectory_path);
//}

TYPED_TEST_P(FSEventQueueTest, associate_dir_dequeue_dir_remove) {
  if (!FileSystem().mkdir(this->get_test_directory_path())) {
    throw Exception();
  }

  TypeParam fs_event_queue(&Log::open(std::cout));
  if (
    !fs_event_queue.associate(
      this->get_test_root_path(),
      FSEvent::TYPE_DIRECTORY_REMOVE
    )
  ) {
    throw Exception();
  }

  if (!FileSystem().rmdir(this->get_test_directory_path())) {
    throw Exception();
  }

  auto_Object<FSEvent> fs_event =
    Object::cast<FSEvent>(fs_event_queue.dequeue());
  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_REMOVE);
  ASSERT_EQ(fs_event->get_path(), this->get_test_directory_path());
}

//TEST_EX(FSEventQueue, directory_remove_recursive, FSEventQueueTest) {
//  if (!FileSystem().mkdir(get_test_directory_path()))
//    throw Exception();
//
//  Path test_subdirectory_path = get_test_directory_path() / "subdir";
//  if (!FileSystem().mkdir(test_subdirectory_path))
//    throw Exception();
//
//  TypeParam fs_event_queue(&Log::open(std::cout));
//  if (!fs_event_queue.associate(get_test_root_path(), FSEvent::TYPE_DIRECTORY_REMOVE))
//    throw Exception();
//
//  if (!FileSystem().rmdir(test_subdirectory_path))
//    throw Exception();
//
//  auto_Object<FSEvent> fs_event =
//    Object::cast<FSEvent>(fs_event_queue.dequeue());
//  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_REMOVE);
//  ASSERT_EQ(fs_event->get_path(), test_subdirectory_path);
//}

TYPED_TEST_P(FSEventQueueTest, associate_dir_dequeue_dir_rename) {
  if (!FileSystem().mkdir(this->get_test_directory_path())) {
    throw Exception();
  }

  Path new_test_directory_path = this->get_test_root_path() / "test_dir_renamed";
  if (FileSystem().exists(new_test_directory_path))
    if (!FileSystem().rmdir(new_test_directory_path)) {
      throw Exception();
    }

  TypeParam fs_event_queue(&Log::open(std::cout));
  if (
    !fs_event_queue.associate(
      this->get_test_root_path(),
      FSEvent::TYPE_DIRECTORY_RENAME
    )
  ) {
    throw Exception();
  }

  if (!FileSystem().rename(this->get_test_directory_path(), new_test_directory_path)) {
    throw Exception();
  }

  auto_Object<FSEvent> fs_event =
    Object::cast<FSEvent>(fs_event_queue.dequeue());
  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_RENAME);
  ASSERT_EQ(fs_event->get_old_path(), this->get_test_directory_path());
  ASSERT_EQ(fs_event->get_new_path(), new_test_directory_path);

  FileSystem().rmdir(new_test_directory_path);
}

TYPED_TEST_P(FSEventQueueTest, associate_dir_dequeue_file_add) {
  TypeParam fs_event_queue(&Log::open(std::cout));
  if (
    !fs_event_queue.associate(
      this->get_test_root_path(),
      FSEvent::TYPE_FILE_ADD
    )
  ) {
    throw Exception();
  }

  if (!FileSystem().touch(this->get_test_file_path())) {
    throw Exception();
  }

  auto_Object<FSEvent> fs_event =
    Object::cast<FSEvent>(fs_event_queue.dequeue());
  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_FILE_ADD);
  ASSERT_EQ(fs_event->get_path(), this->get_test_file_path());
}

//TEST_EX(FSEventQueue, file_add_recursive, FSEventQueueTest) {
//  if (!FileSystem().mkdir(get_test_directory_path()))
//    throw Exception();
//
//  TypeParam fs_event_queue(&Log::open(std::cout));
//  if (!fs_event_queue.associate(get_test_root_path()))
//    throw Exception();
//
//  Path test_file_path = get_test_directory_path() / "file.txt";
//  if (!FileSystem().touch(test_file_path))
//    throw Exception();
//
//  auto_Object<FSEvent> fs_event =
//    Object::cast<FSEvent>(fs_event_queue.dequeue());
//  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_FILE_ADD);
//  ASSERT_EQ(fs_event->get_path(), test_file_path);
//}

TYPED_TEST_P(FSEventQueueTest, associate_dir_dequeue_file_modify) {
  if (!FileSystem().touch(this->get_test_file_path())) {
    throw Exception();
  }

  TypeParam fs_event_queue(&Log::open(std::cout));
  if (
    !fs_event_queue.associate(
      this->get_test_root_path(),
      FSEvent::TYPE_FILE_MODIFY
    )
  ) {
    throw Exception();
  }

  yield::thread::Thread::sleep(1.0);

  DateTime atime = DateTime::now(), mtime = atime;
  if (!FileSystem().utime(this->get_test_file_path(), atime, mtime)) {
    throw Exception();
  }

  auto_Object<FSEvent> fs_event =
    Object::cast<FSEvent>(fs_event_queue.dequeue());
  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_FILE_MODIFY);
  ASSERT_EQ(fs_event->get_path(), this->get_test_file_path());
}

TYPED_TEST_P(FSEventQueueTest, associate_dir_dequeue_file_remove) {
  if (!FileSystem().touch(this->get_test_file_path())) {
    throw Exception();
  }

  TypeParam fs_event_queue(&Log::open(std::cout));
  if (
    !fs_event_queue.associate(
      this->get_test_root_path(),
      FSEvent::TYPE_FILE_REMOVE
    )
  ) {
    throw Exception();
  }

  if (!FileSystem().unlink(this->get_test_file_path())) {
    throw Exception();
  }

  auto_Object<FSEvent> fs_event =
    Object::cast<FSEvent>(fs_event_queue.dequeue());
  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_FILE_REMOVE);
  ASSERT_EQ(fs_event->get_path(), this->get_test_file_path());
}

//TEST_EX(FSEventQueue, file_remove_recursive, FSEventQueueTest) {
//  if (!FileSystem().mkdir(get_test_directory_path()))
//    throw Exception();
//
//  Path test_file_path = get_test_directory_path() / "file.txt";
//  if (!FileSystem().touch(test_file_path))
//    throw Exception();
//
//  TypeParam fs_event_queue(&Log::open(std::cout));
//  if (!fs_event_queue.associate(get_test_root_path(), FSEvent::TYPE_FILE_REMOVE))
//    throw Exception();
//
//  if (!FileSystem().unlink(test_file_path))
//    throw Exception();
//
//  auto_Object<FSEvent> fs_event =
//    Object::cast<FSEvent>(fs_event_queue.dequeue());
//  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_FILE_REMOVE);
//  ASSERT_EQ(fs_event->get_path(), test_file_path);
//}

TYPED_TEST_P(FSEventQueueTest, associate_dir_dequeue_file_rename) {
  if (!FileSystem().touch(this->get_test_file_path())) {
    throw Exception();
  }

  Path new_test_file_path = this->get_test_root_path() / "test_file_renamed.txt";
  if (FileSystem().exists(new_test_file_path))
    if (!FileSystem().unlink(new_test_file_path)) {
      throw Exception();
    }

  TypeParam fs_event_queue(&Log::open(std::cout));
  if (
    !fs_event_queue.associate(
      this->get_test_root_path(),
      FSEvent::TYPE_FILE_RENAME
    )
  ) {
    throw Exception();
  }

  if (!FileSystem().rename(this->get_test_file_path(), new_test_file_path)) {
    throw Exception();
  }

  auto_Object<FSEvent> fs_event =
    Object::cast<FSEvent>(fs_event_queue.dequeue());
  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_FILE_RENAME);
  ASSERT_EQ(fs_event->get_old_path(), this->get_test_file_path());
  ASSERT_EQ(fs_event->get_new_path(), new_test_file_path);

  FileSystem().unlink(new_test_file_path);
}

TYPED_TEST_P(FSEventQueueTest, associate_file_dequeue_file_modify) {
  if (!FileSystem().touch(this->get_test_file_path())) {
    throw Exception();
  }

  TypeParam fs_event_queue(&Log::open(std::cout));
  if (
    !fs_event_queue.associate(
      this->get_test_file_path(),
      FSEvent::TYPE_FILE_MODIFY
    )
  ) {
    throw Exception();
  }

  yield::thread::Thread::sleep(1.0);

  DateTime atime = DateTime::now(), mtime = atime;
  if (!FileSystem().utime(this->get_test_file_path(), atime, mtime)) {
    throw Exception();
  }

  auto_Object<FSEvent> fs_event =
    Object::cast<FSEvent>(fs_event_queue.dequeue());
  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_FILE_MODIFY);
  ASSERT_EQ(fs_event->get_path(), this->get_test_file_path());
}

TYPED_TEST_P(FSEventQueueTest, associate_file_dequeue_file_remove) {
  if (!FileSystem().touch(this->get_test_file_path())) {
    throw Exception();
  }

  TypeParam fs_event_queue(&Log::open(std::cout));
  if (
    !fs_event_queue.associate(
      this->get_test_file_path(),
      FSEvent::TYPE_FILE_REMOVE
    )
  ) {
    throw Exception();
  }

  if (!FileSystem().unlink(this->get_test_file_path())) {
    throw Exception();
  }

  auto_Object<FSEvent> fs_event =
    Object::cast<FSEvent>(fs_event_queue.dequeue());
  ASSERT_EQ(fs_event->get_type(), FSEvent::TYPE_FILE_REMOVE);
  ASSERT_EQ(fs_event->get_path(), this->get_test_file_path());
}

TYPED_TEST_P(FSEventQueueTest, dissociate) {
  TypeParam fs_event_queue(&Log::open(std::cout));
  fs_event_queue.associate(this->get_test_root_path());

  if (!FileSystem().touch(this->get_test_file_path())) {
    throw Exception();
  }

  if (!fs_event_queue.dissociate(this->get_test_root_path())) {
    throw Exception();
  }

  Event* fs_event = fs_event_queue.timeddequeue(0);
  ASSERT_EQ(fs_event, static_cast<Event*>(NULL));
}

REGISTER_TYPED_TEST_CASE_P(
  FSEventQueueTest,
  associate,
  associate_change,
  associate_dir_dequeue_dir_add,
  associate_dir_dequeue_dir_modify,
  associate_dir_dequeue_dir_remove,
  associate_dir_dequeue_dir_rename,
  associate_dir_dequeue_file_add,
  associate_dir_dequeue_file_modify,
  associate_dir_dequeue_file_remove,
  associate_dir_dequeue_file_rename,
  associate_file_dequeue_file_modify,
  associate_file_dequeue_file_remove,
  dissociate
);
}
}
}

#endif
