// fs_event_test.cpp

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

#include "yield/assert.hpp"
#include "yield/fs/poll/fs_event.hpp"
#include "yunit.hpp"

#include <sstream>

TEST_SUITE(FSEvent);

namespace yield {
namespace fs {
namespace poll {
TEST(FSEvent, constructors) {
  FSEvent("path", FSEvent::TYPE_DIRECTORY_ADD);
  FSEvent("old_path", "new_path", FSEvent::TYPE_DIRECTORY_RENAME);
}

TEST(FSEvent, get_new_path) {
  throw_assert_eq(
    FSEvent("old_path", "new_path", FSEvent::TYPE_FILE_RENAME).get_new_path(),
    Path("new_path")
  );
}

TEST(FSEvent, get_old_path) {
  throw_assert_eq(
    FSEvent("old_path", "new_path", FSEvent::TYPE_FILE_RENAME).get_old_path(),
    Path("old_path")
  );
}

TEST(FSEvent, get_path) {
  throw_assert_eq(
    FSEvent("path", FSEvent::TYPE_FILE_ADD).get_path(),
    Path("path")
  );
}

TEST(FSEvent, get_type) {
  throw_assert_eq(
    FSEvent("path", FSEvent::TYPE_FILE_ADD).get_type(),
    FSEvent::TYPE_FILE_ADD
  );
}

TEST(FSEvent, get_type_id) {
  throw_assert_eq(
    FSEvent("path", FSEvent::TYPE_FILE_ADD).get_type_id(),
    FSEvent::TYPE_ID
  );
}

TEST(FSEvent, get_type_name) {
  throw_assert_eq(
    strcmp(
      FSEvent("path", FSEvent::TYPE_FILE_ADD).get_type_name(),
      "yield::fs::poll::FSEvent"
    ),
    0
  );
}

TEST(FSEvent, print) {
  std::ostringstream oss;
  oss << FSEvent("path", FSEvent::TYPE_FILE_ADD);
  throw_assert_ne(oss.str().size(), 0);
}
}
}
}
