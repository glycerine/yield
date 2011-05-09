// yield/fs/poll/win32/directory_watch.cpp

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

#include "directory_watch.hpp"
#include "yield/assert.hpp"
#include "yield/log.hpp"
#include "yield/event_handler.hpp"
#include "yield/fs/file_system.hpp"

#include <Windows.h>

namespace yield {
namespace fs {
namespace poll {
namespace win32 {
DirectoryWatch::DirectoryWatch(
  YO_NEW_REF Directory& directory,
  FSEvent::Type fs_event_types,
  const Path& path,
  Log* log
) : yield::fs::poll::Watch(fs_event_types, log, path),
    directory(&directory) {
  static_assert(sizeof(overlapped) == sizeof(::OVERLAPPED), "");
  memset(&overlapped, 0, sizeof(overlapped));
  this_ = this;

  notify_filter = 0;

  if (
    fs_event_types & FSEvent::TYPE_DIRECTORY_ADD
    ||
    fs_event_types & FSEvent::TYPE_DIRECTORY_REMOVE
    ||
    fs_event_types & FSEvent::TYPE_DIRECTORY_RENAME
  )
    notify_filter |= FILE_NOTIFY_CHANGE_DIR_NAME;

  if (
    fs_event_types & FSEvent::TYPE_DIRECTORY_MODIFY
    ||
    fs_event_types & FSEvent::TYPE_FILE_MODIFY
  ) {
    notify_filter |= FILE_NOTIFY_CHANGE_ATTRIBUTES |
                      FILE_NOTIFY_CHANGE_CREATION |
                      FILE_NOTIFY_CHANGE_LAST_ACCESS |
                      FILE_NOTIFY_CHANGE_LAST_WRITE |
                      FILE_NOTIFY_CHANGE_SIZE;
  }

  if (
    fs_event_types & FSEvent::TYPE_FILE_ADD
    ||
    fs_event_types & FSEvent::TYPE_FILE_REMOVE
    ||
    fs_event_types & FSEvent::TYPE_FILE_RENAME
  )
    notify_filter |= FILE_NOTIFY_CHANGE_FILE_NAME;

  Directory::Entry* dirent = directory.read();
  if (dirent != NULL) {
    do {
      if (dirent->ISDIR() && !dirent->is_hidden() && !dirent->is_special())
        subdirectory_names.push_back(dirent->get_name());
    } while (directory.read(*dirent));

    Directory::Entry::dec_ref(*dirent);
    directory.rewind();
  }
}

DirectoryWatch::~DirectoryWatch() {
  close();
}

DirectoryWatch& DirectoryWatch::cast(::OVERLAPPED& lpOverlapped) {
  DirectoryWatch* directory_watch;

  memcpy_s(
    &directory_watch,
    sizeof(directory_watch),
    reinterpret_cast<char*>(&lpOverlapped) + sizeof(::OVERLAPPED),
    sizeof(directory_watch)
  );

  return *directory_watch;
}

void DirectoryWatch::close() {
  if (directory != NULL) {
    CancelIoEx(*directory, *this);
    directory->close();
    Directory::dec_ref(*directory);
    directory = NULL;
  }
}

bool DirectoryWatch::is_closed() const {
  return directory == NULL;
}

DirectoryWatch::operator ::OVERLAPPED* () {
  return reinterpret_cast<::OVERLAPPED*>(&overlapped);
}

void
DirectoryWatch::read(
  const FILE_NOTIFY_INFORMATION& file_notify_info,
  EventHandler& fs_event_handler
) {
  Path name(
         file_notify_info.FileName,
         file_notify_info.FileNameLength / sizeof(wchar_t)
       );
  Path path = this->get_path() / name;

  if (get_log() != NULL) {
    get_log()->get_stream(Log::Level::DEBUG) <<
      "yield::fs::poll::win32::DirectoryWatch(" <<
        "path=" << get_path() <<
      ")" <<
      ": read FILE_NOTIFY_INFORMATION(" <<
        "Action=" << file_notify_info.Action <<
        ", "
        "FileName=" << name <<
      ")";
  }

  if (file_notify_info.Action == FILE_ACTION_RENAMED_OLD_NAME) {
    for (
      vector<Path>::iterator subdirectory_name_i
      = subdirectory_names.begin();
      subdirectory_name_i != subdirectory_names.end();
      ++subdirectory_name_i
    ) {
      if (*subdirectory_name_i == name) {
        subdirectory_names.erase(subdirectory_name_i);
        break;
      }
    }

    old_paths.push(path);
  } else {
    if (file_notify_info.Action == FILE_ACTION_RENAMED_NEW_NAME) {
      debug_assert_false(old_paths.empty());

      FSEvent::Type fs_event_type;
      if (FileSystem().isdir(path)) {
        fs_event_type = FSEvent::TYPE_DIRECTORY_RENAME;
        subdirectory_names.push_back(path);
      } else
        fs_event_type = FSEvent::TYPE_FILE_RENAME;

      if ((get_fs_event_types() & fs_event_type) == fs_event_type) {
        FSEvent* fs_event = new FSEvent(old_paths.top(), path, fs_event_type);
        log_fs_event(*fs_event);
        fs_event_handler.handle(*fs_event);
      }

      old_paths.pop();
    } else {
      FSEvent::Type fs_event_type;

      switch (file_notify_info.Action) {
      case FILE_ACTION_ADDED: {
        if (FileSystem().isdir(path)) {
          fs_event_type = FSEvent::TYPE_DIRECTORY_ADD;
          subdirectory_names.push_back(name);
        } else
          fs_event_type = FSEvent::TYPE_FILE_ADD;
      }
      break;

      case FILE_ACTION_MODIFIED: {
        if (FileSystem().isdir(path))
          fs_event_type = FSEvent::TYPE_DIRECTORY_MODIFY;
        else
          fs_event_type = FSEvent::TYPE_FILE_MODIFY;
      }
      break;

      case FILE_ACTION_REMOVED: {
        fs_event_type = FSEvent::TYPE_FILE_REMOVE;

        for (
          vector<Path>::iterator subdirectory_name_i
            = subdirectory_names.begin();
          subdirectory_name_i != subdirectory_names.end();
          ++subdirectory_name_i
        ) {
          if (*subdirectory_name_i == name) {
            subdirectory_names.erase(subdirectory_name_i);
            fs_event_type = FSEvent::TYPE_DIRECTORY_REMOVE;
            break;
          }
        }
      }
      break;

      default: {
        fs_event_type = FSEvent::TYPE_DIRECTORY_ADD;
        debug_break();
      }
      break;
      }

      if ((get_fs_event_types() & fs_event_type) == fs_event_type) {
        FSEvent* fs_event = new FSEvent(path, fs_event_type);
        log_fs_event(*fs_event);
        fs_event_handler.handle(*fs_event);
      }
    }
  }
}
}
}
}
}