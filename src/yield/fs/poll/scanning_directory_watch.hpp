// yield/fs/poll/scanning_directory_watch.hpp

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

#ifndef _YIELD_FS_POLL_SCANNING_DIRECTORY_WATCH_HPP_
#define _YIELD_FS_POLL_SCANNING_DIRECTORY_WATCH_HPP_

#include "scanning_watch.hpp"

#include <map>

namespace yield {
namespace fs {
namespace poll {
class ScanningDirectoryWatch : public ScanningWatch {
public:
  ScanningDirectoryWatch(
    FSEvent::Type fs_event_types,
    const Path& path,
    Log* log = NULL
  );

  virtual ~ScanningDirectoryWatch();

public:
  // yield::fs::poll::ScanningWatch
  void scan(EventHandler& fs_event_handler) {
    scan(fs_event_handler, get_fs_event_types());
  }

protected:
  void scan(EventHandler& fs_event_handler, FSEvent::Type fs_event_types);

private:
  YO_NEW_REF Stat* stat(Directory::Entry&);

private:
  std::map<Path, Stat*>* dentries;
};
}
}
}

#endif
