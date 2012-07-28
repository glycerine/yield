// yield/fs/poll/fs_event.hpp

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

#ifndef _YIELD_FS_POLL_FS_EVENT_HPP_
#define _YIELD_FS_POLL_FS_EVENT_HPP_

#include "yield/event.hpp"
#include "yield/fs/path.hpp"

namespace yield {
namespace fs {
namespace poll {
/**
  Event subclass describing file system change events (directory or file
    add/modify/remove/rename).
*/
class FSEvent  : public Event {
public:
  const static uint32_t TYPE_ID;

public:
  /**
    Type enum. An integer instead of a proper enum to allow
      FSEventQueue::associate to take a bitmask of desired types.
  */
  typedef uint8_t Type;

  /**
    A mask of all of the below types.
  */
  const static Type TYPE_ALL;

  /**
    File system change event type: (sub)directory addition.
  */
  const static Type TYPE_DIRECTORY_ADD;

  /**
    File system change event type: directory modification.
  */
  const static Type TYPE_DIRECTORY_MODIFY;

  /**
    File system change event type: directory removal.
  */
  const static Type TYPE_DIRECTORY_REMOVE;

  /**
    File system change event type: directory rename.
  */
  const static Type TYPE_DIRECTORY_RENAME;

  /**
    File system change event type: file addition.
  */
  const static Type TYPE_FILE_ADD;

  /**
    File system change event type: file modification.
  */

  const static Type TYPE_FILE_MODIFY;

  /**
    File system change event type: file removal.
  */
  const static Type TYPE_FILE_REMOVE;

  /**
    File system change event type: file rename.
  */
  const static Type TYPE_FILE_RENAME;

public:
  /**
    Construct an FSEvent with a single path.
    @param path path to the added/modified/removed directory or file
    @param type TYPE_DIRECTORY_ADD, TYPE_DIRECTORY_MODIFY,
      TYPE_DIRECTORY_REMOVE, TYPE_FILE_ADD, TYPE_FILE_MODIFY, TYPE_FILE_REMOVE
  */
  FSEvent(const Path& path, Type type);

  /**
    Construct an FSEvent with two paths i.e., a rename.
    @param old_path path to the directory or file before the rename
    @param new_path path to the directory or file after the rename
    @param type TYPE_DIRECTORY_RENAME or TYPE_FILE_RENAME
  */
  FSEvent(const Path& old_path, const Path& new_path, Type type);

public:
  /**
    Get the new path of a renamed directory or file, when get_type() ==
      TYPE_DIRECTORY_RENAME or TYPE_FILE_RENAME.
    @return the new path of a renamed directory or file
  */
  const Path& get_new_path() const {
    return new_path;
  }

  /**
    Get the old path of a renamed directory or file, when get_type() ==
      TYPE_DIRECTORY_RENAME or TYPE_FILE_RENAME.
    @return the old path of a renamed directory or file
  */
  const Path& get_old_path() const {
    return old_path;
  }

  /**
    Get the path of an added/modified/removed directory or file, when
      get_type() == TYPE_(DIRECTORY|FILE)_(ADD|MODIFY|REMOVE)
    @return the path of an added/modified/removed directory or file
  */
  const Path& get_path() const {
    return old_path;
  }

  /**
    Get the type of file system change event (one of the TYPE_* constants).
    @return the type of file system change event
  */
  Type get_type() const {
    return type;
  }

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::fs::poll::FSEvent";
  }

private:
  Path new_path, old_path;
  Type type;
};

std::ostream& operator<<(std::ostream&, const FSEvent&);
}
}
}

#endif
