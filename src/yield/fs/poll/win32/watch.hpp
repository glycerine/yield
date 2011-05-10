// yield/fs/poll/win32/watch.hpp

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

#ifndef _YIELD_FS_POLL_WIN32_WATCH_HPP_
#define _YIELD_FS_POLL_WIN32_WATCH_HPP_

#include "../watch.hpp"
#include "yield/fs/directory.hpp"

#include <stack>

struct _FILE_NOTIFY_INFORMATION;
typedef struct _FILE_NOTIFY_INFORMATION FILE_NOTIFY_INFORMATION;
struct _OVERLAPPED;
typedef struct _OVERLAPPED OVERLAPPED;

namespace yield {
class EventHandler;

namespace fs {
namespace poll {
namespace win32 {
class Watch : public yield::fs::poll::Watch {
public:
  virtual ~Watch();

public:
  static Watch& cast(::OVERLAPPED& lpOverlapped);

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

  Directory& get_directory() {
    return *directory;
  }

  unsigned long get_notify_filter() const {
    return notify_filter;
  }

public:
  operator ::OVERLAPPED* ();

public:
  virtual void
  read(
    const FILE_NOTIFY_INFORMATION&,
    EventHandler& fs_event_handler
  ) = 0;

protected:
  Watch(
    YO_NEW_REF Directory& directory,
    FSEvent::Type fs_event_types,
    Log* log,
    const Path& path
  );

protected:
  void log_read(const FILE_NOTIFY_INFORMATION&);

private:
  char buffer[(12 + 260 * sizeof(wchar_t)) * 16];
  Directory* directory;
  unsigned long notify_filter;

  struct {
    unsigned long* Internal;
    unsigned long* InternalHigh;
#pragma warning( push )
#pragma warning( disable: 4201 )
    union {
      struct {
        unsigned long Offset;
        unsigned long OffsetHigh;
      };
      void* Pointer;
    };
#pragma warning( pop )
    void* hEvent;
  } overlapped;
  Watch* this_;
};
}
}
}
}

#endif
