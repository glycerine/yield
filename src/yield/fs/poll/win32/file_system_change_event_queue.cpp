// yield/fs/poll/win32/file_system_change_event_queue.cpp

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

#include "file_system_change_event_queue.hpp"
#include "yield/aio/win32/aiocb.hpp"
#include "yield/fs/win32/directory.hpp"
#include "yield/fs/win32/file_system.hpp"

#include <stack>

#include <Windows.h>

namespace yield {
namespace fs {
namespace poll {
namespace win32 {
using std::stack;
using yield::aio::win32::AIOCB;
using yield::aio::win32::AIOQueue;
using yield::fs::win32::Directory;
using yield::fs::win32::FileSystem;

class FileSystemChangeEventQueue::Watch : public AIOCB {
public:
  Watch(
    YO_NEW_REF Directory& directory,
    const Path& directory_path,
    FileSystemChangeEvent::Type events,
    bool recursive
  ) : directory(&directory) {
    bWatchSubtree = recursive ? TRUE : FALSE;

    dwNotifyFilter = 0;

    if
    (
      events & FileSystemChangeEvent::TYPE_DIRECTORY_ADD
      ||
      events & FileSystemChangeEvent::TYPE_DIRECTORY_REMOVE
      ||
      events & FileSystemChangeEvent::TYPE_DIRECTORY_RENAME
    )
      dwNotifyFilter |= FILE_NOTIFY_CHANGE_DIR_NAME;

    if
    (
      events & FileSystemChangeEvent::TYPE_DIRECTORY_MODIFY
      ||
      events & FileSystemChangeEvent::TYPE_FILE_MODIFY
    ) {
      dwNotifyFilter |= FILE_NOTIFY_CHANGE_ATTRIBUTES |
                        FILE_NOTIFY_CHANGE_CREATION |
                        FILE_NOTIFY_CHANGE_LAST_ACCESS |
                        FILE_NOTIFY_CHANGE_LAST_WRITE |
                        FILE_NOTIFY_CHANGE_SIZE;
    }

    if
    (
      events & FileSystemChangeEvent::TYPE_FILE_ADD
      ||
      events & FileSystemChangeEvent::TYPE_FILE_REMOVE
      ||
      events & FileSystemChangeEvent::TYPE_FILE_RENAME
    )
      dwNotifyFilter |= FILE_NOTIFY_CHANGE_FILE_NAME;

    dwReadUntilBufferOffset = 0;

    gather_known_directory_paths(directory, directory_path);
  }

  ~Watch() {
    close();
  }

  void close() {
    if (directory != NULL) {
      CancelIoEx(*directory, *this);
      directory->close();
      directory = NULL;
    }
  }

  bool is_closed() const {
    return directory == NULL;
  }

  //bool read(FileSystemChangeEvent& volume_change_event) {
  //  bool read_volume_change_event = false;

  //  while (dwReadUntilBufferOffset != static_cast<DWORD>(~0)) {
  //    const FILE_NOTIFY_INFORMATION* file_notify_info
  //    = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>
  //      (
  //        &file_notify_information[dwReadUntilBufferOffset]
  //      );

  //    const wchar_t* name = file_notify_info->FileName;
  //    size_t name_len = file_notify_info->FileNameLength;
  //    name_len /= sizeof(wchar_t);
  //    Path path = get_path() / Path(name, name_len);

  //    if (file_notify_info->Action == FILE_ACTION_RENAMED_OLD_NAME) {
  //      for
  //      (
  //        vector<Path>::iterator known_directory_path_i
  //        = known_directory_paths.begin();
  //        known_directory_path_i != known_directory_paths.end();
  //        ++known_directory_path_i
  //      ) {
  //        if (*known_directory_path_i == path) {
  //          known_directory_paths.erase(known_directory_path_i);
  //          break;
  //        }
  //      }

  //      old_paths.push(path);
  //    } else {
  //      if (file_notify_info->Action == FILE_ACTION_RENAMED_NEW_NAME) {
  //        if (old_paths.empty()) DebugBreak();

  //        FileSystemChangeEvent::Type type;
  //        if (win32::Volume().isdir(path)) {
  //          type = FileSystemChangeEvent::TYPE_DIRECTORY_RENAME;
  //          known_directory_paths.push_back(path);
  //        } else
  //          type = FileSystemChangeEvent::TYPE_FILE_RENAME;

  //        if ((events & type) == type) {
  //          volume_change_event.set_new_path(path);
  //          volume_change_event.set_old_path(old_paths.top());
  //          volume_change_event.set_type(type);
  //          read_volume_change_event = true;
  //        }

  //        old_paths.pop();
  //      } else {
  //        FileSystemChangeEvent::Type type;

  //        switch (file_notify_info->Action) {
  //        case FILE_ACTION_ADDED: {
  //          if (win32::Volume().isdir(path)) {
  //            type = FileSystemChangeEvent::TYPE_DIRECTORY_ADD;
  //            known_directory_paths.push_back(path);
  //          } else
  //            type = FileSystemChangeEvent::TYPE_FILE_ADD;
  //        }
  //        break;

  //        case FILE_ACTION_MODIFIED: {
  //          if (win32::Volume().isdir(path))
  //            type = FileSystemChangeEvent::TYPE_DIRECTORY_MODIFY;
  //          else
  //            type = FileSystemChangeEvent::TYPE_FILE_MODIFY;
  //        }
  //        break;

  //        case FILE_ACTION_REMOVED: {
  //          type = FileSystemChangeEvent::TYPE_FILE_REMOVE;

  //          for
  //          (
  //            vector<Path>::iterator known_directory_path_i
  //            = known_directory_paths.begin();
  //            known_directory_path_i != known_directory_paths.end();
  //            ++known_directory_path_i
  //          ) {
  //            if (*known_directory_path_i == path) {
  //              known_directory_paths.erase(known_directory_path_i);
  //              type = FileSystemChangeEvent::TYPE_DIRECTORY_REMOVE;
  //              break;
  //            }
  //          }
  //        }
  //        break;

  //        default: {
  //          type = FileSystemChangeEvent::TYPE_DIRECTORY_ADD;
  //          DebugBreak();
  //        }
  //        break;
  //        }

  //        if ((get_events() & type) == type) {
  //          volume_change_event.set_path(path);
  //          volume_change_event.set_type(type);
  //          read_volume_change_event = true;
  //        }
  //      }
  //    }

  //    if (file_notify_info->NextEntryOffset > 0) {
  //      dwReadUntilBufferOffset += file_notify_info->NextEntryOffset;

  //      if (read_volume_change_event)
  //        break;
  //      else
  //        continue;
  //    } else {
  //      dwReadUntilBufferOffset = static_cast<DWORD>(~0);
  //      break;
  //    }
  //  }

  //  return read_volume_change_event;
  //}

public:
  // yield::aio::win32::AIOCB
  bool issue(AIOQueue&) {
    dwReadUntilBufferOffset = 0;

    return ReadDirectoryChangesW(
             *directory,
             file_notify_information,
             sizeof(file_notify_information),
             bWatchSubtree,
             dwNotifyFilter,
             NULL,
             *this,
             NULL
           ) == TRUE;
  }

private:
  void
  gather_known_directory_paths(
    Directory& directory,
    const Path& directory_path
  ) {
    Directory::Entry* dirent = directory.read();
    if (dirent != NULL) {
      do {
        // test type
        if (!dirent->is_hidden() && !dirent->is_special()) {
          Path subdirectory_path = directory_path / dirent->get_name();
          known_directory_paths.push_back(subdirectory_path);
          Directory* subdirectory = FileSystem().opendir(subdirectory_path);
          if (subdirectory != NULL) {
            gather_known_directory_paths(*subdirectory, subdirectory_path);
            Directory::dec_ref(*subdirectory);
          }
        }
      } while (directory.read(*dirent));

      Directory::Entry::dec_ref(*dirent);
      directory.rewind();
    }
  }

private:
  BOOL bWatchSubtree;
  Directory* directory;
  DWORD dwNotifyFilter, dwReadUntilBufferOffset;

  char file_notify_information[
    (
      sizeof(FILE_NOTIFY_INFORMATION)
      +
      (MAX_PATH - 1) * sizeof(wchar_t)
    )
    *
    16
  ];

private:
  vector<Path> known_directory_paths;
  stack<Path> old_paths;
};


FileSystemChangeEventQueue::~FileSystemChangeEventQueue() {
  for(
    std::map<Path, Watch*>::iterator watch_i = watches.begin();
    watch_i != watches.end();
    ++watch_i
  ) {
    watch_i->second->close();
    delete watch_i->second;
  }
}

bool
FileSystemChangeEventQueue::associate(
  const Path& path,
  FileSystemChangeEvent::Type events,
  bool recursive
) {
  std::map<Path, Watch*>::iterator watch_i = watches.find(path);
  if (watch_i == watches.end()) {
    Directory* directory = FileSystem().opendir(path);
    if (directory != NULL) {
      if (AIOQueue::associate(*directory)) {
        Watch* watch = new Watch(*directory, path, events, recursive);
        if (watch->issue(*this)) {
          watches.insert(path, watch);
          return true;
        } else {
          Watch::dec_ref(*watch);
          return false;
        }
      } else
        return false;
    } else
      return false;
  } else {
    DebugBreak(); // Modify the existing watch
    return false;
  }

  //Watch* watch = watches.find(path);
  //if (watch != NULL)
  //  DebugBreak(); // Modify the existing watch
  //else {
  //  watch = Watch::create(events, flags, path);
  //  if (watch != NULL) {
  //    if (io_completion_port.associate(watch->get_fd())) {
  //      watches.insert(path, *watch);
  //      watch->initiate();
  //      return true;
  //    } else
  //      delete watch;
  //  }
  //}

  return false;
}

bool FileSystemChangeEventQueue::dissociate(const Path& path) {
  std::map<Path, Watch*>::iterator watch_i = watches.find(path);
  if (watch_i != watches.end()) {
    Watch* watch = watch_i->second;
    watch->close(); // Don't delete until it comes back to the completion port
    watches.erase(watch_i);
    return true;
  } else
    return false;
}

//int
//FileSystemChangeEventQueue::poll
//(
//  FileSystemChangeEvent* volume_change_events,
//  int volume_change_events_len,
//  const Time& _timeout
//) {
//  Time timeout(_timeout);
//
//  int volume_change_event_i
//  = FileSystemChangeEventQueue::get_leftover_volume_change_events
//    (
//      volume_change_events,
//      volume_change_events_len
//    );
//
//  IOCompletionStatus io_completion_status;
//  while
//  (
//    volume_change_event_i < volume_change_events_len
//    &&
//    io_completion_port.poll(io_completion_status, timeout)
//  ) {
//#ifdef _DEBUG
//    if (io_completion_status.get_error_code() != 0) DebugBreak();
//#endif
//    Watch* watch = Watch::cast(io_completion_status.get_overlapped());
//
//    if (!watch->is_closed()) {
//      volume_change_event_i +=
//        read
//        (
//          volume_change_events + volume_change_event_i,
//          volume_change_events_len - volume_change_event_i,
//          *watch
//        );
//
//      if ((watch->get_flags() & FLAG_ONESHOT) != FLAG_ONESHOT) {
//        watch->reset();
//        watch->initiate();
//      } else
//        dissociate(watch->get_path());
//    } else
//      delete watch;
//
//    timeout = static_cast<uint64_t>(0);
//  }
//
//  return volume_change_event_i;
//}
}
}
}
}
