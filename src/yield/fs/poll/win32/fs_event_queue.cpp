// yield/fs/poll/win32/fs_event_queue.cpp

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

#include "directory_watch.hpp"
#include "file_watch.hpp"
#include "../watches.hpp"
#include "yield/debug.hpp"
#include "yield/exception.hpp"
#include "yield/log.hpp"
#include "yield/fs/poll/fs_event_queue.hpp"
#include "yield/fs/file_system.hpp"

#include <Windows.h>

namespace yield {
namespace fs {
namespace poll {
using win32::DirectoryWatch;
using win32::FileWatch;

FSEventQueue::FSEventQueue(YO_NEW_REF Log* log) : log(log) {
  hIoCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (hIoCompletionPort != INVALID_HANDLE_VALUE) {
    watches = new Watches<win32::Watch>;
  } else {
    watches = NULL;
    throw Exception();
  }
}

FSEventQueue::~FSEventQueue() {
  Log::dec_ref(log);
  delete watches;
}

bool FSEventQueue::associate(const Path& path, FSEvent::Type fs_event_types) {
  Watch* watch = watches->find(path);
  if (watch != NULL) {
    if (watch->get_fs_event_types() == fs_event_types) {
      return true;
    } else {
      delete watches->erase(path);
    }
  }

  Path directory_path;
  if (FileSystem().isdir(path)) {
    directory_path = path;
  } else {
    directory_path = path.split().first;
  }

  Directory* directory = FileSystem().opendir(directory_path);
  if (directory != NULL) {
    if (
      CreateIoCompletionPort(
        *directory,
        hIoCompletionPort,
        0,
        0
      ) != INVALID_HANDLE_VALUE
    ) {
      win32::Watch* watch;
      if (path == directory_path) {
        watch = new DirectoryWatch(*directory, fs_event_types, path, log);
      } else {
        watch
        = new FileWatch(
          *directory,
          directory_path,
          path,
          fs_event_types,
          log
        );
      }

      DWORD dwBytesRead = 0;
      if (
        ReadDirectoryChangesW(
          *directory,
          watch->get_buffer(),
          watch->get_buffer_length(),
          FALSE,
          watch->get_notify_filter(),
          &dwBytesRead,
          *watch,
          NULL
        )
      ) {
        debug_assert_eq(dwBytesRead, 0);
        watches->insert(path, *watch);
        return true;
      } else {
        delete watch;
        return false;
      }
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool FSEventQueue::dissociate(const Path& path) {
  win32::Watch* watch = watches->erase(path);
  if (watch != NULL) {
    watch->close();
    return true;
  } else {
    return false;
  }
}

bool FSEventQueue::enqueue(YO_NEW_REF Event& event) {
  return PostQueuedCompletionStatus(
           hIoCompletionPort,
           0,
           reinterpret_cast<ULONG_PTR>(&event),
           NULL
         )
         == TRUE;
}

YO_NEW_REF Event* FSEventQueue::timeddequeue(const Time& timeout) {
  DWORD dwBytesTransferred = 0;
  ULONG_PTR ulCompletionKey = 0;
  LPOVERLAPPED lpOverlapped = NULL;

  BOOL bRet
  = GetQueuedCompletionStatus(
      hIoCompletionPort,
      &dwBytesTransferred,
      &ulCompletionKey,
      &lpOverlapped,
      static_cast<DWORD>(timeout.ms())
    );

  if (lpOverlapped != NULL) {
    win32::Watch& watch = win32::Watch::cast(*lpOverlapped);
    if (!watch.is_closed()) {
      debug_assert_eq(bRet, TRUE);
      debug_assert_gt(dwBytesTransferred, 0);

      DWORD dwReadUntilBufferOffset = 0;
      for (;;) {
        const FILE_NOTIFY_INFORMATION* file_notify_info
        = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(
            &watch.get_buffer()[dwReadUntilBufferOffset]
          );

        FSEvent* fs_event = watch.parse(*file_notify_info);
        if (fs_event != NULL) {
          enqueue(*fs_event);
        }

        if (file_notify_info->NextEntryOffset > 0) {
          dwReadUntilBufferOffset += file_notify_info->NextEntryOffset;
        } else {
          break;
        }
      }

      DWORD dwBytesRead;
      //BOOL bRet =
      ReadDirectoryChangesW(
        watch.get_directory(),
        watch.get_buffer(),
        watch.get_buffer_length(),
        FALSE,
        watch.get_notify_filter(),
        &dwBytesRead,
        watch,
        NULL
      );

      return trydequeue();
    } else {
      delete &watch;
      return NULL;
    }
  } else if (ulCompletionKey != 0) {
    return reinterpret_cast<Event*>(ulCompletionKey);
  } else {
    return NULL;
  }
}
}
}
}
