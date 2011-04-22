// yield/fs/poll/fs_event.hpp

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

#ifndef _YIELD_FS_POLL_FS_EVENT_HPP_
#define _YIELD_FS_POLL_FS_EVENT_HPP_

#include "yield/event.hpp"
#include "yield/fs/path.hpp"

namespace yield {
namespace fs {
namespace poll {
class FSEvent  : public Event {
public:
  const static uint32_t TYPE_ID = 1640119324;

public:
  typedef uint8_t Type;
  const static Type TYPE_ALL = static_cast<Type>(~0);
  const static Type TYPE_DIRECTORY_ADD = 1;
  const static Type TYPE_DIRECTORY_MODIFY = 2;
  const static Type TYPE_DIRECTORY_REMOVE = 4;
  const static Type TYPE_DIRECTORY_RENAME = 8;
  const static Type TYPE_FILE_ADD = 16;
  const static Type TYPE_FILE_MODIFY = 32;
  const static Type TYPE_FILE_REMOVE = 64;
  const static Type TYPE_FILE_RENAME = 128;

public:
  FSEvent()
    : type(TYPE_FILE_ADD)
  { }

  FSEvent(const Path& path, Type type)
    : old_path(path), type(type)
  { }

  FSEvent(
    const Path& old_path,
    const Path& new_path,
    Type type
  ) : new_path(new_path), old_path(old_path), type(type)
  { }

public:
  const Path& get_new_path() const {
    return new_path;
  }

  const Path& get_old_path() const {
    return old_path;
  }

  const Path& get_path() const {
    return old_path;
  }

  Type get_type() const {
    return type;
  }

public:
  void set_new_path(const Path& new_path) {
    this->new_path = new_path;
  }

  void set_old_path(const Path& old_path) {
    this->old_path = old_path;
  }

  void set_path(const Path& path) {
    this->old_path = path;
  }

  void set_type(Type type) {
    this->type = type;
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
}
}
}

#endif