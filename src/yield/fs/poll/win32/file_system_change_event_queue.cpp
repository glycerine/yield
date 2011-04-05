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

#include "directory.h"
#include "directory_change_event_queue.h"
using yield::fs::VolumeChangeEvent;
using yield::fs::win32::DirectoryChangeEventQueue;
#include "yield/time.h"
using yield::Time;
#include "yield/fs/path.h"
using yield::fs::Path;
#include "yield/poll/win32/io_completion_port.h"
using yield::poll::win32::IOCompletionPort;
#include "yield/poll/win32/io_completion_status.h"
using yield::poll::win32::IOCompletionStatus;
#include "yield/poll/win32/overlapped.h"
using yield::poll::win32::OVERLAPPED;
#include "volume.h"

#include <stack>
using std::stack;

#include <windows.h>


class DirectoryChangeEventQueue::Watch
  : public OVERLAPPED,
    public VolumeChangeEventQueue::Watch {
public:
  ~Watch() {
    close();
  }

  void close() {
    if (directory != NULL) {
      CancelIoEx(*directory, this);
      directory->close();
      directory = NULL;
    }
  }

  static Watch* cast(LPOVERLAPPED lpOverlapped) {
    return static_cast<Watch*>(OVERLAPPED::cast(lpOverlapped));
  }

  static Watch*
  create
  (
    VolumeChangeEvent::Type events,
    Flag flags,
    const Path& path
  ) {
    win32::Directory* directory
    = static_cast<win32::Directory*>(win32::Volume().opendir(path));

    if (directory != NULL) {
      flags |= FLAG_ONLYDIR;
      return new Watch(*directory, events, flags, path);
    } else
      return NULL;
  }

  bool is_closed() const {
    return directory == NULL;
  }

  bool read(VolumeChangeEvent& volume_change_event) {
    bool read_volume_change_event = false;

    while (dwReadUntilBufferOffset != static_cast<DWORD>(~0)) {
      const FILE_NOTIFY_INFORMATION* file_notify_info
      = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>
        (
          &buffer[dwReadUntilBufferOffset]
        );

      const wchar_t* name = file_notify_info->FileName;
      size_t name_len = file_notify_info->FileNameLength;
      name_len /= sizeof(wchar_t);
      Path path = get_path() / Path(name, name_len);

      if (file_notify_info->Action == FILE_ACTION_RENAMED_OLD_NAME) {
        for
        (
          vector<Path>::iterator known_directory_path_i
          = known_directory_paths.begin();
          known_directory_path_i != known_directory_paths.end();
          ++known_directory_path_i
        ) {
          if (*known_directory_path_i == path) {
            known_directory_paths.erase(known_directory_path_i);
            break;
          }
        }

        old_paths.push(path);
      } else {
        if (file_notify_info->Action == FILE_ACTION_RENAMED_NEW_NAME) {
          if (old_paths.empty()) DebugBreak();

          VolumeChangeEvent::Type type;
          if (win32::Volume().isdir(path)) {
            type = VolumeChangeEvent::TYPE_DIRECTORY_RENAME;
            known_directory_paths.push_back(path);
          } else
            type = VolumeChangeEvent::TYPE_FILE_RENAME;

          if ((get_events() & type) == type) {
            volume_change_event.set_new_path(path);
            volume_change_event.set_old_path(old_paths.top());
            volume_change_event.set_type(type);
            read_volume_change_event = true;
          }

          old_paths.pop();
        } else {
          VolumeChangeEvent::Type type;

          switch (file_notify_info->Action) {
          case FILE_ACTION_ADDED: {
            if (win32::Volume().isdir(path)) {
              type = VolumeChangeEvent::TYPE_DIRECTORY_ADD;
              known_directory_paths.push_back(path);
            } else
              type = VolumeChangeEvent::TYPE_FILE_ADD;
          }
          break;

          case FILE_ACTION_MODIFIED: {
            if (win32::Volume().isdir(path))
              type = VolumeChangeEvent::TYPE_DIRECTORY_MODIFY;
            else
              type = VolumeChangeEvent::TYPE_FILE_MODIFY;
          }
          break;

          case FILE_ACTION_REMOVED: {
            type = VolumeChangeEvent::TYPE_FILE_REMOVE;

            for
            (
              vector<Path>::iterator known_directory_path_i
              = known_directory_paths.begin();
              known_directory_path_i != known_directory_paths.end();
              ++known_directory_path_i
            ) {
              if (*known_directory_path_i == path) {
                known_directory_paths.erase(known_directory_path_i);
                type = VolumeChangeEvent::TYPE_DIRECTORY_REMOVE;
                break;
              }
            }
          }
          break;

          default: {
            type = VolumeChangeEvent::TYPE_DIRECTORY_ADD;
            DebugBreak();
          }
          break;
          }

          if ((get_events() & type) == type) {
            volume_change_event.set_path(path);
            volume_change_event.set_type(type);
            read_volume_change_event = true;
          }
        }
      }

      if (file_notify_info->NextEntryOffset > 0) {
        dwReadUntilBufferOffset += file_notify_info->NextEntryOffset;

        if (read_volume_change_event)
          break;
        else
          continue;
      } else {
        dwReadUntilBufferOffset = static_cast<DWORD>(~0);
        break;
      }
    }

    return read_volume_change_event;
  }

  // AIOCB
  void abort(uint32_t) {
    DebugBreak();
  }

  fd_t get_fd() const {
    return *directory;
  }

  bool initiate() {
    dwReadUntilBufferOffset = 0;

    return ReadDirectoryChangesW
           (
             get_fd(),
             buffer,
             sizeof(buffer),
             (get_flags() & FLAG_RECURSIVE) ? TRUE : FALSE,
             dwNotifyFilter,
             NULL,
             this,
             NULL
           ) == TRUE;
  }

private:
  Watch
  (
    YO_NEW_REF Directory& directory,
    VolumeChangeEvent::Type events,
    Flag flags,
    const Path& path
  )
    : VolumeChangeEventQueue::Watch(events, flags, path),
      directory(&directory) {
    dwNotifyFilter = 0;

    if
    (
      get_events() & VolumeChangeEvent::TYPE_DIRECTORY_ADD
      ||
      get_events() & VolumeChangeEvent::TYPE_DIRECTORY_REMOVE
      ||
      get_events() & VolumeChangeEvent::TYPE_DIRECTORY_RENAME
    )
      dwNotifyFilter |= FILE_NOTIFY_CHANGE_DIR_NAME;

    if
    (
      get_events() & VolumeChangeEvent::TYPE_DIRECTORY_MODIFY
      ||
      get_events() & VolumeChangeEvent::TYPE_FILE_MODIFY
    ) {
      dwNotifyFilter |= FILE_NOTIFY_CHANGE_ATTRIBUTES |
                        FILE_NOTIFY_CHANGE_CREATION |
                        FILE_NOTIFY_CHANGE_LAST_ACCESS |
                        FILE_NOTIFY_CHANGE_LAST_WRITE |
                        FILE_NOTIFY_CHANGE_SIZE;
    }

    if
    (
      get_events() & VolumeChangeEvent::TYPE_FILE_ADD
      ||
      get_events() & VolumeChangeEvent::TYPE_FILE_REMOVE
      ||
      get_events() & VolumeChangeEvent::TYPE_FILE_RENAME
    )
      dwNotifyFilter |= FILE_NOTIFY_CHANGE_FILE_NAME;

    dwReadUntilBufferOffset = 0;

    gather_known_directory_paths(directory, path);
  }

  void
  gather_known_directory_paths
  (
    yield::fs::Directory& directory,
    const Path& directory_path
  ) {
    yield::fs::Directory::Entry* dirent
    = directory.read(yield::fs::Directory::Entry::TYPE_DIR);
    if (dirent != NULL) {
      do {
        if (!dirent->is_hidden() && !dirent->is_special()) {
          Path subdirectory_path = directory_path / dirent->get_name();
          known_directory_paths.push_back(subdirectory_path);
          yield::fs::Directory* subdirectory
          = Volume().opendir(subdirectory_path);
          if (subdirectory != NULL) {
            gather_known_directory_paths(*subdirectory, subdirectory_path);
            Directory::dec_ref(*subdirectory);
          }
        }
      } while
      (
        directory.read
        (
          *dirent,
          yield::fs::Directory::Entry::TYPE_DIR
        )
      );

      yield::fs::Directory::Entry::dec_ref(*dirent);
      directory.rewind();
    }
  }

private:
  Directory* directory;
  DWORD dwNotifyFilter, dwReadUntilBufferOffset;
  char buffer[
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


DirectoryChangeEventQueue::DirectoryChangeEventQueue
(
  IOCompletionPort& io_completion_port
)
  : io_completion_port(io_completion_port)
{ }

DirectoryChangeEventQueue::~DirectoryChangeEventQueue() {
  IOCompletionPort::dec_ref(io_completion_port);

  for
  (
    WatchMap<Watch>::iterator watch_i = watches.begin();
    watch_i != watches.end();
    ++watch_i
  ) {
    watch_i->second->close();
    delete watch_i->second;
  }
}

bool
DirectoryChangeEventQueue::associate
(
  const Path& path,
  VolumeChangeEvent::Type events,
  Flag flags
) {
  Watch* watch = watches.find(path);
  if (watch != NULL)
    DebugBreak(); // Modify the existing watch
  else {
    watch = Watch::create(events, flags, path);
    if (watch != NULL) {
      if (io_completion_port.associate(watch->get_fd())) {
        watches.insert(path, *watch);
        watch->initiate();
        return true;
      } else
        delete watch;
    }
  }

  return false;
}

DirectoryChangeEventQueue& DirectoryChangeEventQueue::create() {
  IOCompletionPort* io_completion_port
  = IOCompletionPort::create();
  if (io_completion_port != NULL)
    return *new DirectoryChangeEventQueue(*io_completion_port);
  else
    throw Exception();
}

void DirectoryChangeEventQueue::dissociate(const Path& path) {
  Watch* watch = watches.erase(path);
  if (watch != NULL)
    watch->close(); // Don't delete until it comes back to poll.
}

int
DirectoryChangeEventQueue::poll
(
  VolumeChangeEvent* volume_change_events,
  int volume_change_events_len,
  const Time& _timeout
) {
  Time timeout(_timeout);

  int volume_change_event_i
  = DirectoryChangeEventQueue::get_leftover_volume_change_events
    (
      volume_change_events,
      volume_change_events_len
    );

  IOCompletionStatus io_completion_status;
  while
  (
    volume_change_event_i < volume_change_events_len
    &&
    io_completion_port.poll(io_completion_status, timeout)
  ) {
#ifdef _DEBUG
    if (io_completion_status.get_error_code() != 0) DebugBreak();
#endif
    Watch* watch = Watch::cast(io_completion_status.get_overlapped());

    if (!watch->is_closed()) {
      volume_change_event_i +=
        read
        (
          volume_change_events + volume_change_event_i,
          volume_change_events_len - volume_change_event_i,
          *watch
        );

      if ((watch->get_flags() & FLAG_ONESHOT) != FLAG_ONESHOT) {
        watch->reset();
        watch->initiate();
      } else
        dissociate(watch->get_path());
    } else
      delete watch;

    timeout = static_cast<uint64_t>(0);
  }

  return volume_change_event_i;
}
