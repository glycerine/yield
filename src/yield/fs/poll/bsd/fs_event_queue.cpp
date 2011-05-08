// yield/fs/poll/bsd/fs_event_queue.cpp

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

#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/log.hpp"
#include "yield/fs/poll/bsd/fs_event_queue.hpp"
#include "yield/fs/file_system.hpp"
#include "yield/poll/fd_event.hpp"

#include <errno.h>
#include <fcntl.h>
#include <sys/event.h>

#ifndef __MACH__
#define O_EVTONLY O_RDONLY
#endif

namespace yield {
namespace fs {
namespace poll {
namespace bsd {
class FSEventQueue::Watch {
public:
  Watch(
    int fd,
    FSEvent::Type fs_event_types,
    const Path& path,
    Log* log = NULL
  ) : fd(fd),
      fs_event_types(fs_event_types),
      log(Object::inc_ref(log)),
      path(path) {
    isdir = FileSystem().isdir(path);
  }

  ~Watch() {
    close(fd);
    Log::dec_ref(log);
  }

public:
  FSEvent::Type get_fs_event_types() const {
    return fs_event_types;
  }

  const Path& get_path() const {
    return path;
  }

public:
  void read(const struct kevent& kevent_, EventHandler& fs_event_handler) {
    if (log != NULL) {
      log->get_stream(Log::Level::DEBUG) <<
        get_type_name() << "(fd=" << fd << ", path=" << path << "): " <<
          "read kevent(fflags=" << kevent_.fflags << ")";
    }

    uint32_t fflags = kevent_.fflags;
    while (fflags != 0) {
      FSEvent::Type fs_event_type;
      if ((fflags & NOTE_ATTRIB) == NOTE_ATTRIB) {
        fflags ^= NOTE_ATTRIB;
        fs_event_type = isdir ?
                        FSEvent::TYPE_DIRECTORY_MODIFY :
                        FSEvent::TYPE_FILE_MODIFY;
      } else if ((fflags & NOTE_DELETE) == NOTE_DELETE) {
        fflags ^= NOTE_DELETE;
        fs_event_type = isdir ?
                        FSEvent::TYPE_DIRECTORY_REMOVE :
                        FSEvent::TYPE_FILE_REMOVE;
      } else if ((fflags & NOTE_EXTEND) == NOTE_EXTEND) {
        fflags ^= NOTE_EXTEND;
        fs_event_type = isdir ?
                        FSEvent::TYPE_DIRECTORY_MODIFY :
                        FSEvent::TYPE_FILE_MODIFY;
      } else if ((fflags & NOTE_LINK) == NOTE_LINK) {
        fflags ^= NOTE_LINK;
        fs_event_type = isdir ?
                        FSEvent::TYPE_DIRECTORY_MODIFY :
                        FSEvent::TYPE_FILE_MODIFY;
      } else if ((fflags & NOTE_RENAME) == NOTE_RENAME) {
        fflags ^= NOTE_RENAME;
        fs_event_type = isdir ?
                        FSEvent::TYPE_DIRECTORY_RENAME :
                        FSEvent::TYPE_FILE_RENAME;
      } else if ((fflags & NOTE_REVOKE) == NOTE_REVOKE) {
        fflags ^= NOTE_REVOKE;
        fs_event_type = isdir ?
                        FSEvent::TYPE_DIRECTORY_REMOVE :
                        FSEvent::TYPE_FILE_REMOVE;
      } else if ((fflags & NOTE_WRITE) == NOTE_WRITE) {
        fflags ^= NOTE_WRITE;
        fs_event_type = isdir ?
                        FSEvent::TYPE_DIRECTORY_MODIFY :
                        FSEvent::TYPE_FILE_MODIFY;
      }

      FSEvent* fs_event = new FSEvent(get_path(), fs_event_type);
      if (log != NULL) {
        log->get_stream(Log::Level::DEBUG) <<
          get_type_name() << "(fd=" << fd << ", path=" << path << "): " <<
            "read " << *fs_event;
      }
      fs_event_handler.handle(*fs_event);
    }
  }

public:
  const char* get_type_name() const {
    return "yield::fs::poll::bsd::FSEventQueue::Watch";
  }

private:
  int fd;
  FSEvent::Type fs_event_types;
  bool isdir;
  Log* log;
  Path path;
};

FSEventQueue::FSEventQueue(YO_NEW_REF Log* log) : log(log) {
  kq = kqueue();
  if (kq != -1) {
    if (pipe(wake_pipe) != -1) {
      struct kevent kevent_;
      EV_SET(&kevent_, wake_pipe[0], EVFILT_READ, EV_ADD, 0, 0, NULL);
      if (kevent(kq, &kevent_, 1, 0, 0, NULL) != -1)
        return;
      else {
        Exception exception;
        close(kq);
        close(wake_pipe[0]);
        close(wake_pipe[1]);
        throw exception;
      }
    } else {
      Exception exception;
      close(kq);
      throw exception;
    }
  } else
    throw Exception();
}

FSEventQueue::~FSEventQueue() {
  close(kq);
  Log::dec_ref(log);
  close(wake_pipe[0]);
  close(wake_pipe[1]);
  for (
    vector<Watch*>::iterator watch_i = watches.begin();
    watch_i != watches.end();
    ++watch_i
  )
    delete *watch_i;
}

bool
FSEventQueue::associate(
  const Path& path,
  FSEvent::Type fs_event_types
) {
  for (
    vector<Watch*>::iterator watch_i = watches.begin();
    watch_i != watches.end();
    ++watch_i
  ) {
    if ((*watch_i)->get_path() == path) {
      if ((*watch_i)->get_fs_event_types() == fs_event_types)
        return true;
      else {
        delete *watch_i;
        watches.erase(watch_i);
        break;
      }
    }
  }

  int fd = open(path.c_str(), O_EVTONLY);
  if (fd != -1) {
    Watch* watch = new Watch(fd, fs_event_types, path, log);

    struct kevent kevent_;
    int fflags = 0;
    if (fs_event_types & FSEvent::TYPE_DIRECTORY_ADD)
      fflags |= NOTE_EXTEND | NOTE_WRITE;
    if (fs_event_types & FSEvent::TYPE_DIRECTORY_MODIFY)
      fflags |= NOTE_ATTRIB | NOTE_EXTEND | NOTE_WRITE;
    if (fs_event_types & FSEvent::TYPE_DIRECTORY_REMOVE)
      fflags |= NOTE_DELETE;
    if (fs_event_types & FSEvent::TYPE_DIRECTORY_RENAME)
      fflags |= NOTE_RENAME;
    if (fs_event_types & FSEvent::TYPE_FILE_ADD)
      fflags |= NOTE_EXTEND | NOTE_WRITE;
    if (fs_event_types & FSEvent::TYPE_FILE_MODIFY)
      fflags |= NOTE_ATTRIB | NOTE_EXTEND | NOTE_WRITE;
    if (fs_event_types & FSEvent::TYPE_FILE_REMOVE)
      fflags |= NOTE_DELETE;
    if (fs_event_types & FSEvent::TYPE_FILE_RENAME)
      fflags |= NOTE_RENAME;
    EV_SET(&kevent_, fd, EVFILT_VNODE, EV_ADD, fflags, 0, watch);

    if (kevent(kq, &kevent_, 1, 0, 0, NULL) != -1) {
      watches.push_back(watch);
      return true;
    } else
      delete watch;
  }

  return false;
}

bool FSEventQueue::dissociate(const Path& path) {
  for (
    vector<Watch*>::iterator watch_i = watches.begin();
    watch_i != watches.end();
    ++watch_i
  ) {
    if ((*watch_i)->get_path() == path) {
      delete *watch_i;
      watches.erase(watch_i);
      return true;
    }
  }

  return false;
}

bool FSEventQueue::enqueue(YO_NEW_REF Event& event) {
  if (event_queue.enqueue(event)) {
    ssize_t write_ret = write(wake_pipe[1], "m", 1);
    debug_assert_eq(write_ret, 1);
    return true;
  } else
    return false;
}

YO_NEW_REF Event* FSEventQueue::timeddequeue(const Time& timeout) {
  Event* event = event_queue.trydequeue();
  if (event != NULL)
    return event;
  else {
    struct kevent kevent_;
    timespec timeout_ts = timeout;
    int ret = kevent(kq, 0, 0, &kevent_, 1, &timeout_ts);
    if (ret > 0) {
      debug_assert_eq(ret, 1);
      if (static_cast<int>(kevent_.ident) == wake_pipe[0]) {
        char m;
        read(wake_pipe[0], &m, sizeof(m));
        return event_queue.trydequeue();
      } else {
        debug_assert_eq(kevent_.filter, EVFILT_VNODE);
        Watch* watch = static_cast<Watch*>(kevent_.udata);
        watch->read(kevent_, *this);;
        return event_queue.trydequeue();
      }
    } else if (ret == 0 || errno == EINTR)
      return NULL;
    else {
      debug_break();
      return NULL;
    }
  }
}
}
}
}
}
