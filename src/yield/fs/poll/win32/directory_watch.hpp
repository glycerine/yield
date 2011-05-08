// yield/fs/poll/win32/directory_watch.hpp

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

#ifndef _YIELD_FS_POLL_WIN32_DIRECTORY_WATCH_HPP_
#define _YIELD_FS_POLL_WIN32_DIRECTORY_WATCH_HPP_

#include "../directory_watch.hpp"
#include "yield/aio/win32/aiocb.hpp"
#include "yield/fs/directory.hpp"

#include <stack>

struct _FILE_NOTIFY_INFORMATION;
typedef struct _FILE_NOTIFY_INFORMATION FILE_NOTIFY_INFORMATION;

namespace yield {
namespace fs {
namespace poll {
namespace win32 {
class DirectoryWatch
  : public yield::aio::win32::AIOCB,
    public yield::fs::poll::DirectoryWatch {
public:
  const static uint32_t TYPE_ID = 497230685;

public:
  DirectoryWatch(
    YO_NEW_REF Directory& directory,
    FSEvent::Type fs_event_types,
    const Path& path,
    Log* log = NULL
  );

  ~DirectoryWatch();

public:
  void close();
  bool is_closed() const;

public:
  char* get_buffer() {
    return &buffer[0];
  }

  size_t get_buffer_length() const {
    return sizeof(buffer);
  }

  unsigned long get_notify_filter() const {
    return notify_filter;
  }

public:
  void read(const FILE_NOTIFY_INFORMATION&, EventHandler& fs_event_handler);

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::fs::poll::win32::DirectoryWatch";
  }

private:
  Directory* directory;
  unsigned long notify_filter;
  std::stack<Path> old_paths;
  vector<Path> subdirectory_names;
  char buffer[(12 + 260 * sizeof(wchar_t)) * 16];
};
}
}
}
}

#endif
