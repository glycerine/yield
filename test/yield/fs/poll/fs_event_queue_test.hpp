// fs_event_queue_test.hpp

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

#ifndef _YIELD_FS_POLL_FS_EVENT_QUEUE_TEST_HPP_
#define _YIELD_FS_POLL_FS_EVENT_QUEUE_TEST_HPP_

#include "../../event_queue_test.hpp"
#include "yield/auto_object.hpp"
#include "yield/assert.hpp"
#include "yield/date_time.hpp"
#include "yield/exception.hpp"
#include "yield/log.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/fs/path.hpp"
#include "yield/fs/poll/fs_event.hpp"
#include "yunit.hpp"

namespace yield {
namespace fs {
namespace poll {
class FSEventQueueTest : public yunit::Test {
public:
  FSEventQueueTest() {
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


template <class FSEventQueueType>
class FSEventQueueAssociateTest : public FSEventQueueTest {
  // yunit::Test
  void run() {
    if (!FSEventQueueType().associate(get_test_root_path()))
      throw Exception();
  }
};


template <class FSEventQueueType>
class FSEventQueueAssociateChangeTest : public FSEventQueueTest {
  // yunit::Test
  void run() {
    FSEventQueueType fs_event_queue(&Log::open(std::cout));

    if (!fs_event_queue.associate(get_test_root_path()))
      throw Exception();

    if (!fs_event_queue.associate(get_test_root_path()))
      throw Exception();

    if (!fs_event_queue.associate(get_test_root_path(), FSEvent::TYPE_FILE_ADD))
      throw Exception();
  }
};


template <class FSEventQueueType>
class FSEventQueueDequeueDirectoryAddTest : public FSEventQueueTest {
  // yunit::Test
  void run() {
    FSEventQueueType fs_event_queue(&Log::open(std::cout));
    if (!fs_event_queue.associate(get_test_root_path()))
      throw Exception();

    if (!FileSystem().mkdir(get_test_directory_path()))
      throw Exception();

    auto_Object<FSEvent> fs_event =
      object_cast<FSEvent>(fs_event_queue.dequeue());
    throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_ADD);
    throw_assert_eq(fs_event->get_path(), get_test_directory_path());
  }
};


template <class FSEventQueueType>
class FSEventQueueDequeueDirectoryModifyTest : public FSEventQueueTest {
  // yunit::Test
  void run() {
    if (!FileSystem().mkdir(get_test_directory_path()))
      throw Exception();

    FSEventQueueType fs_event_queue(&Log::open(std::cout));
    if (!fs_event_queue.associate(get_test_root_path()))
      throw Exception();

    DateTime atime = DateTime::now(), mtime = atime;
    if (!FileSystem().utime(get_test_directory_path(), atime, mtime))
      throw Exception();

    auto_Object<FSEvent> fs_event =
      object_cast<FSEvent>(fs_event_queue.dequeue());
    throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_MODIFY);
    throw_assert_eq(fs_event->get_path(), get_test_directory_path());
  }
};


//TEST_EX(FSEventQueue, directory_add_recursive, FSEventQueueTest) {
//  if (!FileSystem().mkdir(get_test_directory_path()))
//    throw Exception();
//
//  FSEventQueueType fs_event_queue(&Log::open(std::cout));
//  if (!fs_event_queue.associate(get_test_root_path()))
//    throw Exception();
//
//  Path test_subdirectory_path = get_test_directory_path() / "subdir";
//  if (!FileSystem().mkdir(test_subdirectory_path))
//    throw Exception();
//
//  auto_Object<FSEvent> fs_event =
//    object_cast<FSEvent>(fs_event_queue.dequeue());
//  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_ADD);
//  throw_assert_eq(fs_event->get_path(), test_subdirectory_path);
//}


template <class FSEventQueueType>
class FSEventQueueDequeueDirectoryRemoveTest : public FSEventQueueTest {
  // yunit::Test
  void run() {
    if (!FileSystem().mkdir(get_test_directory_path()))
      throw Exception();

    FSEventQueueType fs_event_queue(&Log::open(std::cout));
    if (!fs_event_queue.associate(get_test_root_path()))
      throw Exception();

    if (!FileSystem().rmdir(get_test_directory_path()))
      throw Exception();

    auto_Object<FSEvent> fs_event =
      object_cast<FSEvent>(fs_event_queue.dequeue());
    throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_REMOVE);
    throw_assert_eq(fs_event->get_path(), get_test_directory_path());
  }
};


//TEST_EX(FSEventQueue, directory_remove_recursive, FSEventQueueTest) {
//  if (!FileSystem().mkdir(get_test_directory_path()))
//    throw Exception();
//
//  Path test_subdirectory_path = get_test_directory_path() / "subdir";
//  if (!FileSystem().mkdir(test_subdirectory_path))
//    throw Exception();
//
//  FSEventQueueType fs_event_queue(&Log::open(std::cout));
//  if (!fs_event_queue.associate(get_test_root_path()))
//    throw Exception();
//
//  if (!FileSystem().rmdir(test_subdirectory_path))
//    throw Exception();
//
//  auto_Object<FSEvent> fs_event =
//    object_cast<FSEvent>(fs_event_queue.dequeue());
//  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_REMOVE);
//  throw_assert_eq(fs_event->get_path(), test_subdirectory_path);
//}


template <class FSEventQueueType>
class FSEventQueueDequeueDirectoryRenameTest : public FSEventQueueTest {
  // yunit::Test
  void run() {
    if (!FileSystem().mkdir(get_test_directory_path()))
      throw Exception();

    Path new_test_directory_path = get_test_root_path() / "test_dir_renamed";
    if (FileSystem().exists(new_test_directory_path))
      if (!FileSystem().rmdir(new_test_directory_path))
        throw Exception();

    FSEventQueueType fs_event_queue(&Log::open(std::cout));
    if (!fs_event_queue.associate(get_test_root_path()))
      throw Exception();

    if (!FileSystem().rename(get_test_directory_path(), new_test_directory_path))
      throw Exception();

    auto_Object<FSEvent> fs_event =
      object_cast<FSEvent>(fs_event_queue.dequeue());
    throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_DIRECTORY_RENAME);
    throw_assert_eq(fs_event->get_old_path(), get_test_directory_path());
    throw_assert_eq(fs_event->get_new_path(), new_test_directory_path);

    FileSystem().rmdir(new_test_directory_path);
  }
};


template <class FSEventQueueType>
class FSEventQueueDissociateTest : public FSEventQueueTest {
  // yunit::Test
  void run() {
    FSEventQueueType fs_event_queue(&Log::open(std::cout));
    fs_event_queue.associate(get_test_root_path());

    if (!FileSystem().touch(get_test_file_path()))
      throw Exception();

    if (!fs_event_queue.dissociate(get_test_root_path()))
      throw Exception();

    Event* fs_event = fs_event_queue.trydequeue();
    throw_assert_eq(fs_event, NULL);
  }
};


template <class FSEventQueueType>
class FSEventQueueDequeueFileAddTest : public FSEventQueueTest {
  // yunit::Test
  void run() {
    FSEventQueueType fs_event_queue(&Log::open(std::cout));
    if (!fs_event_queue.associate(get_test_root_path()))
      throw Exception();

    if (!FileSystem().touch(get_test_file_path()))
      throw Exception();

    auto_Object<FSEvent> fs_event =
      object_cast<FSEvent>(fs_event_queue.dequeue());
    throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_FILE_ADD);
    throw_assert_eq(fs_event->get_path(), get_test_file_path());
  }
};


//TEST_EX(FSEventQueue, file_add_recursive, FSEventQueueTest) {
//  if (!FileSystem().mkdir(get_test_directory_path()))
//    throw Exception();
//
//  FSEventQueueType fs_event_queue(&Log::open(std::cout));
//  if (!fs_event_queue.associate(get_test_root_path()))
//    throw Exception();
//
//  Path test_file_path = get_test_directory_path() / "file.txt";
//  if (!FileSystem().touch(test_file_path))
//    throw Exception();
//
//  auto_Object<FSEvent> fs_event =
//    object_cast<FSEvent>(fs_event_queue.dequeue());
//  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_FILE_ADD);
//  throw_assert_eq(fs_event->get_path(), test_file_path);
//}


template <class FSEventQueueType>
class FSEventQueueDequeueFileModifyTest : public FSEventQueueTest {
  // yunit::Test
  void run() {
    if (!FileSystem().touch(get_test_file_path()))
      throw Exception();

    FSEventQueueType fs_event_queue(&Log::open(std::cout));
    if (!fs_event_queue.associate(get_test_root_path()))
      throw Exception();

    DateTime atime = DateTime::now(), mtime = atime;
    if (!FileSystem().utime(get_test_file_path(), atime, mtime))
      throw Exception();

    auto_Object<FSEvent> fs_event =
      object_cast<FSEvent>(fs_event_queue.dequeue());
    throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_FILE_MODIFY);
    throw_assert_eq(fs_event->get_path(), get_test_file_path());
  }
};


template <class FSEventQueueType>
class FSEventQueueDequeueFileRemoveTest : public FSEventQueueTest {
  // yunit::Test
  void run() {
    if (!FileSystem().touch(get_test_file_path()))
      throw Exception();

    FSEventQueueType fs_event_queue(&Log::open(std::cout));
    if (!fs_event_queue.associate(get_test_root_path()))
      throw Exception();

    if (!FileSystem().unlink(get_test_file_path()))
      throw Exception();

    auto_Object<FSEvent> fs_event =
      object_cast<FSEvent>(fs_event_queue.dequeue());
    throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_FILE_REMOVE);
    throw_assert_eq(fs_event->get_path(), get_test_file_path());
  }
};


//TEST_EX(FSEventQueue, file_remove_recursive, FSEventQueueTest) {
//  if (!FileSystem().mkdir(get_test_directory_path()))
//    throw Exception();
//
//  Path test_file_path = get_test_directory_path() / "file.txt";
//  if (!FileSystem().touch(test_file_path))
//    throw Exception();
//
//  FSEventQueueType fs_event_queue(&Log::open(std::cout));
//  if (!fs_event_queue.associate(get_test_root_path()))
//    throw Exception();
//
//  if (!FileSystem().unlink(test_file_path))
//    throw Exception();
//
//  auto_Object<FSEvent> fs_event =
//    object_cast<FSEvent>(fs_event_queue.dequeue());
//  throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_FILE_REMOVE);
//  throw_assert_eq(fs_event->get_path(), test_file_path);
//}


template <class FSEventQueueType>
class FSEventQueueDequeueFileRenameTest : public FSEventQueueTest {
  // yunit::Test
  void run() {
    if (!FileSystem().touch(get_test_file_path()))
      throw Exception();

    Path new_test_file_path = get_test_root_path() / "test_file_renamed.txt";
    if (FileSystem().exists(new_test_file_path))
      if (!FileSystem().unlink(new_test_file_path))
        throw Exception();

    FSEventQueueType fs_event_queue(&Log::open(std::cout));
    if (!fs_event_queue.associate(get_test_root_path()))
      throw Exception();

    if (!FileSystem().rename(get_test_file_path(), new_test_file_path))
      throw Exception();

    auto_Object<FSEvent> fs_event =
      object_cast<FSEvent>(fs_event_queue.dequeue());
    throw_assert_eq(fs_event->get_type(), FSEvent::TYPE_FILE_RENAME);
    throw_assert_eq(fs_event->get_old_path(), get_test_file_path());
    throw_assert_eq(fs_event->get_new_path(), new_test_file_path);

    FileSystem().unlink(new_test_file_path);
  }
};


template <class FSEventQueueType>
class FSEventQueueTestSuite : public EventQueueTestSuite<FSEventQueueType> {
public:
  FSEventQueueTestSuite() {
    add(
      "FSEventQueue::associate",
      new FSEventQueueAssociateTest<FSEventQueueType>
    );

    add(
      "FSEventQueue::associate change",
      new FSEventQueueAssociateChangeTest<FSEventQueueType>
    );

    add(
      "FSEventQueue::dequeue -> FSEvent(TYPE_DIRECTORY_ADD)",
      new FSEventQueueDequeueDirectoryAddTest<FSEventQueueType>
    );

    add(
      "FSEventQueue::dequeue -> FSEvent(TYPE_DIRECTORY_MODIFY)",
      new FSEventQueueDequeueDirectoryModifyTest<FSEventQueueType>
    );

    add(
      "FSEventQueue::dequeue -> FSEvent(TYPE_DIRECTORY_REMOVE)",
      new FSEventQueueDequeueDirectoryRemoveTest<FSEventQueueType>
    );

    add(
      "FSEventQueue::dequeue -> FSEvent(TYPE_DIRECTORY_RENAME)",
      new FSEventQueueDequeueDirectoryRenameTest<FSEventQueueType>
    );

    add(
      "FSEventQueue::dequeue -> FSEvent(TYPE_FILE_ADD)",
      new FSEventQueueDequeueFileAddTest<FSEventQueueType>
    );

    add(
      "FSEventQueue::dequeue -> FSEvent(TYPE_FILE_MODIFY)",
      new FSEventQueueDequeueFileModifyTest<FSEventQueueType>
    );

    add(
      "FSEventQueue::dequeue -> FSEvent(TYPE_FILE_REMOVE)",
      new FSEventQueueDequeueFileRemoveTest<FSEventQueueType>
    );

    add(
      "FSEventQueue::dequeue -> FSEvent(TYPE_FILE_RENAME)",
      new FSEventQueueDequeueFileRenameTest<FSEventQueueType>
    );

    add(
      "FSEventQueue::dissociate",
      new FSEventQueueDissociateTest<FSEventQueueType>
    );
  }
};
}
}
}

#endif
