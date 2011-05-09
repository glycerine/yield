// yield/fs/poll/watch.cpp

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

#include "watch.hpp"
#include "yield/assert.hpp"
#include "yield/log.hpp"

namespace yield {
namespace fs {
namespace poll {
Watch::Watch(FSEvent::Type fs_event_types, Log* log, const Path& path)
  : fs_event_types(fs_event_types),
    log(Object::inc_ref(log)),
    path(path) {
}

Watch::Watch(const Watch&) {
  debug_break();
}

Watch::~Watch() {
  Log::dec_ref(log);
}

bool Watch::equals(const Stat& left, const Stat& right) {
  return type(left) == type(right)
         &&
         left.get_size() == right.get_size();
}

void Watch::log_read(const FSEvent& fs_event) const {
  if (log != NULL) {
    FSEvent::Type fs_event_types = get_fs_event_types();
    std::string fs_event_types_str;
    if (fs_event_types == FSEvent::TYPE_ALL)
      fs_event_types_str = "TYPE_ALL";
    else {
      std::ostringstream fs_event_types_oss;
      if (fs_event_types & FSEvent::TYPE_DIRECTORY_ADD) {
        fs_event_types_oss << "TYPE_DIRECTORY_ADD|";
        fs_event_types ^= FSEvent::TYPE_DIRECTORY_ADD;
      }

      if (fs_event_types & FSEvent::TYPE_DIRECTORY_MODIFY) {
        fs_event_types_oss << "TYPE_DIRECTORY_MODIFY|";
        fs_event_types ^= FSEvent::TYPE_DIRECTORY_MODIFY;
      }

      if (fs_event_types & FSEvent::TYPE_DIRECTORY_REMOVE) {
        fs_event_types_oss << "TYPE_DIRECTORY_REMOVE|";
        fs_event_types ^= FSEvent::TYPE_DIRECTORY_REMOVE;
      }

      if (fs_event_types & FSEvent::TYPE_DIRECTORY_RENAME) {
        fs_event_types_oss << "TYPE_DIRECTORY_RENAME|";
        fs_event_types ^= FSEvent::TYPE_DIRECTORY_RENAME;
      } 

      if (fs_event_types & FSEvent::TYPE_FILE_ADD) {
        fs_event_types_oss << "TYPE_FILE_ADD|";
        fs_event_types ^= FSEvent::TYPE_FILE_ADD;
      } 

      if (fs_event_types & FSEvent::TYPE_FILE_MODIFY) {
        fs_event_types_oss << "TYPE_FILE_MODIFY|";
        fs_event_types ^= FSEvent::TYPE_FILE_MODIFY;
      } 
      
      if (fs_event_types & FSEvent::TYPE_FILE_REMOVE) {
        fs_event_types_oss << "TYPE_FILE_REMOVE|";
        fs_event_types ^= FSEvent::TYPE_FILE_REMOVE;
      } 
      
      if (fs_event_types & FSEvent::TYPE_FILE_RENAME) {
        fs_event_types_oss << "TYPE_FILE_RENAME|";
        fs_event_types ^= FSEvent::TYPE_FILE_RENAME;
      }

      debug_assert_eq(fs_event_types, 0);

      fs_event_types_str = fs_event_types_oss.str();
      if (!fs_event_types_str.empty()) {
        fs_event_types_str
          = fs_event_types_str.substr(0, fs_event_types_str.size() - 1);
      }
    }

    log->get_stream(Log::Level::DEBUG) <<
    "yield::fs::poll::" <<
      (is_directory_watch() ? "DirectoryWatch" : "FileWatch") <<
      "(" <<
        "fs_event_types=" << fs_event_types_str <<
        ", " <<
        "path=" << get_path() <<
      ")" <<
      ": read " << fs_event;
  }
}

Directory::Entry::Type Watch::type(const Stat& stbuf) {
#ifdef _WIN32
  if (stbuf.ISDEV())
    return Directory::Entry::TYPE_DEV;
  else if (stbuf.ISDIR())
    return Directory::Entry::TYPE_DIR;
  else if (stbuf.ISREG())
    return Directory::Entry::TYPE_REG;
#else
  if (stbuf.ISBLK())
    return Directory::Entry::TYPE_BLK;
  else if (stbuf.ISCHR())
    return Directory::Entry::TYPE_CHR;
  else if (stbuf.ISDIR())
    return Directory::Entry::TYPE_DIR;
  else if (stbuf.ISFIFO())
    return Directory::Entry::TYPE_FIFO;
  else if (stbuf.ISLNK())
    return Directory::Entry::TYPE_LNK;
  else if (stbuf.ISREG())
    return Directory::Entry::TYPE_REG;
  else if (stbuf.ISSOCK())
    return Directory::Entry::TYPE_SOCK;
#endif
  else {
    debug_break();
    return Directory::Entry::TYPE_REG;
  }
}
}
}
}