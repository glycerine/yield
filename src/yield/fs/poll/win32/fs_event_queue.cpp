// yield/fs/poll/win32/fs_event_queue.cpp

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

#include "fs_event_queue.hpp"
#include "yield/assert.hpp"
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

class FSEventQueue::Watch : public AIOCB {
public:
  const static uint32_t TYPE_ID = 497230685;

public:
  Watch(
    YO_NEW_REF Directory& directory,
    FSEvent::Type events,
    const Path& path,
    bool recursive
  ) : directory(&directory),
      path(path),
      events(events) {
    bWatchSubtree = recursive ? TRUE : FALSE;

    dwNotifyFilter = 0;

    if (
      events & FSEvent::TYPE_DIRECTORY_ADD
      ||
      events & FSEvent::TYPE_DIRECTORY_REMOVE
      ||
      events & FSEvent::TYPE_DIRECTORY_RENAME
    )
      dwNotifyFilter |= FILE_NOTIFY_CHANGE_DIR_NAME;

    if (
      events & FSEvent::TYPE_DIRECTORY_MODIFY
      ||
      events & FSEvent::TYPE_FILE_MODIFY
    ) {
      dwNotifyFilter |= FILE_NOTIFY_CHANGE_ATTRIBUTES |
                        FILE_NOTIFY_CHANGE_CREATION |
                        FILE_NOTIFY_CHANGE_LAST_ACCESS |
                        FILE_NOTIFY_CHANGE_LAST_WRITE |
                        FILE_NOTIFY_CHANGE_SIZE;
    }

    if (
      events & FSEvent::TYPE_FILE_ADD
      ||
      events & FSEvent::TYPE_FILE_REMOVE
      ||
      events & FSEvent::TYPE_FILE_RENAME
    )
      dwNotifyFilter |= FILE_NOTIFY_CHANGE_FILE_NAME;

    gather_known_directory_paths(directory, path);
  }

  ~Watch() {
    close();
  }

public:
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

public:
  const Path& get_path() const {
    return path;
  }

public:
  bool read(EventHandler& fs_event_handler) {
    if (get_return() > 0) {
      DWORD dwReadUntilBufferOffset = 0;

      for (;;) {
        const FILE_NOTIFY_INFORMATION* file_notify_info
          = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(
              &buffer[dwReadUntilBufferOffset]
            );

        const wchar_t* name = file_notify_info->FileName;
        size_t name_len = file_notify_info->FileNameLength;
        name_len /= sizeof(wchar_t);
        Path path = this->path / Path(name, name_len);

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
            if (old_paths.empty())
              DebugBreak();

            FSEvent::Type type;
            if (FileSystem().isdir(path)) {
              type = FSEvent::TYPE_DIRECTORY_RENAME;
              known_directory_paths.push_back(path);
            } else
              type = FSEvent::TYPE_FILE_RENAME;

            if ((events & type) == type)
              fs_event_handler.handle(*new FSEvent(old_paths.top(), path, type));

            old_paths.pop();
          } else {
            FSEvent::Type type;

            switch (file_notify_info->Action) {
            case FILE_ACTION_ADDED: {
              if (FileSystem().isdir(path)) {
                type = FSEvent::TYPE_DIRECTORY_ADD;
                known_directory_paths.push_back(path);
              } else
                type = FSEvent::TYPE_FILE_ADD;
            }
            break;

            case FILE_ACTION_MODIFIED: {
              if (FileSystem().isdir(path))
                type = FSEvent::TYPE_DIRECTORY_MODIFY;
              else
                type = FSEvent::TYPE_FILE_MODIFY;
            }
            break;

            case FILE_ACTION_REMOVED: {
              type = FSEvent::TYPE_FILE_REMOVE;

              for
              (
                vector<Path>::iterator known_directory_path_i
                = known_directory_paths.begin();
                known_directory_path_i != known_directory_paths.end();
                ++known_directory_path_i
              ) {
                if (*known_directory_path_i == path) {
                  known_directory_paths.erase(known_directory_path_i);
                  type = FSEvent::TYPE_DIRECTORY_REMOVE;
                  break;
                }
              }
            }
            break;

            default: {
              type = FSEvent::TYPE_DIRECTORY_ADD;
              DebugBreak();
            }
            break;
            }

            if ((events & type) == type)
              fs_event_handler.handle(*new FSEvent(path, type));
          }
        }

        if (file_notify_info->NextEntryOffset > 0)
          dwReadUntilBufferOffset += file_notify_info->NextEntryOffset;
        else
          break;
      }
    }

    DWORD dwBytesRead = 0;
    BOOL bRet =
      ReadDirectoryChangesW(
        *directory,
        buffer,
        sizeof(buffer),
        bWatchSubtree,
        dwNotifyFilter,
        &dwBytesRead,
        *this,
        NULL
      );

    if (bRet == TRUE) {
      if (dwBytesRead > 0) {
        set_return(dwBytesRead);
        return read(fs_event_handler);
      }

      return true;
    } else
      return false;
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
        if (dirent->ISDIR() && !dirent->is_hidden() && !dirent->is_special()) {
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

public:
  // yield::Object
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::fs::poll::win32::FSEventQueue::Watch";
  }

private:
  BOOL bWatchSubtree;
  Directory* directory;
  DWORD dwNotifyFilter;
  FSEvent::Type events;
  vector<Path> known_directory_paths;
  stack<Path> old_paths;
  Path path;

  char buffer[
    (
      sizeof(FILE_NOTIFY_INFORMATION)
      +
      (MAX_PATH - 1) * sizeof(wchar_t)
    )
    *
    16
  ];
};


FSEventQueue::~FSEventQueue() {
  for (
    std::map<Path, Watch*>::iterator watch_i = watches.begin();
    watch_i != watches.end();
    ++watch_i
  ) {
    watch_i->second->close();
    delete watch_i->second;
  }
}

bool
FSEventQueue::associate(
  const Path& path,
  FSEvent::Type events,
  bool recursive
) {
  std::map<Path, Watch*>::iterator watch_i = watches.find(path);
  if (watch_i == watches.end()) {
    Directory* directory = FileSystem().opendir(path);
    if (directory != NULL) {
      if (aio_queue.associate(*directory)) {
        Watch* watch = new Watch(*directory, events, path, recursive);
        if (watch->read(*this)) {
          watches[path] = watch;
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
}

YO_NEW_REF Event* FSEventQueue::dequeue(const Time& timeout) {
  Event* event = aio_queue.dequeue(timeout);
  if (event != NULL) {
    if (event->get_type_id() == Watch::TYPE_ID) {
      Watch* watch = static_cast<Watch*>(event);
      if (!watch->is_closed()) {
        debug_assert_eq(watch->get_error(), 0);

        if (watch->read(aio_queue))
          return aio_queue.trydequeue();
        else {
          dissociate(watch->get_path());
          delete watch;
          return aio_queue.trydequeue();
        }
      } else
        delete watch;
    } else
      return event;
  }

  return NULL;
}

bool FSEventQueue::dissociate(const Path& path) {
  std::map<Path, Watch*>::iterator watch_i = watches.find(path);
  if (watch_i != watches.end()) {
    Watch* watch = watch_i->second;
    watch->close(); // Don't delete until it comes back to the completion port
    watches.erase(watch_i);
    return true;
  } else
    return false;
}

bool FSEventQueue::enqueue(YO_NEW_REF Event& event) {
  return aio_queue.enqueue(event);
}
}
}
}
}
