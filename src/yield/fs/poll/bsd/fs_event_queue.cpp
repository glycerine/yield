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

#include "directory_watch.hpp"
#include "file_watch.hpp"
#include "../watches.hpp"
#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/log.hpp"
#include "yield/fs/poll/bsd/fs_event_queue.hpp"
#include "yield/fs/file_system.hpp"

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
FSEventQueue::FSEventQueue(YO_NEW_REF Log* log) : log(log) {
  watches = NULL;
  kq = kqueue();
  if (kq != -1) {
    try {
      if (pipe(wake_pipe) != -1) {
        try {
          struct kevent kevent_;
          EV_SET(&kevent_, wake_pipe[0], EVFILT_READ, EV_ADD, 0, 0, NULL);
          if (kevent(kq, &kevent_, 1, 0, 0, NULL) == 0)
            watches = new Watches<Watch>;
          else
            throw Exception();
        } catch (Exception&) {
          close(wake_pipe[0]);
          close(wake_pipe[1]);
          throw;
        }
      } else
        throw Exception();
    } catch (Exception&) {
      close(kq);
      throw;
    }
  } else
    throw Exception();
}

FSEventQueue::~FSEventQueue() {
  close(kq);
  Log::dec_ref(log);
  close(wake_pipe[0]);
  close(wake_pipe[1]);
  delete watches;
}

bool
FSEventQueue::associate(
  const Path& path,
  FSEvent::Type fs_event_types
) {
  delete watches->erase(path);

  int fd = open(path.c_str(), O_EVTONLY);
  if (fd != -1) {
    struct stat stbuf;
    if (::fstat(fd, &stbuf) == 0) {
      Watch* watch;
      if (S_ISDIR(stbuf.st_mode))
        watch = new DirectoryWatch(fd, fs_event_types, path, log);
      else
        watch = new FileWatch(fd, fs_event_types, path, log);

      // Don't try to be clever with fflags, since there
      // appears to be minimal logic in how they work.
      struct kevent kevent_;
      EV_SET(
        &kevent_,
        fd,
        EVFILT_VNODE,
        EV_ADD,
        NOTE_ATTRIB | NOTE_DELETE | NOTE_EXTEND |
          NOTE_LINK | NOTE_RENAME | NOTE_REVOKE | NOTE_WRITE,
        0,
        watch
      );

      if (kevent(kq, &kevent_, 1, 0, 0, NULL) != -1) {
        watches->insert(path, *watch);
        return true;
      } else
        delete watch;
    }
  }

  return false;
}

bool FSEventQueue::dissociate(const Path& path) {
  Watch* watch = watches->erase(path);
  if (watch != NULL) {
    delete watch;
    return true;
  } else
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
        //if (log != NULL) {
        //  log->get_stream(Log::Level::DEBUG) <<
        //    get_type_name() << "(fd=" << fd << ", path=" << path << "): " <<
        //      "read kevent(fflags=" << kevent_.fflags << ")";
        //}
        Watch* watch = static_cast<Watch*>(kevent_.udata);
        watch->read(kevent_, *this);
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
