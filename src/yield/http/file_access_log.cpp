// yield/http/file_access_log.cpp

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

#include "file_access_log.hpp"
#include "yield/auto_object.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/file_system.hpp"

#include <fcntl.h>


namespace yield {
namespace http {
using yield::fs::File;
using yield::fs::Path;
using yield::fs::FileSystem;


FileAccessLog::FileAccessLog(const Path& file_path, Format& format)
  : AccessLog(format),
    file_path(file_path)
{ }

FileAccessLog::~FileAccessLog() {
  File::dec_ref(file);
}

void
FileAccessLog::write
(
  const HTTPRequest& http_request,
  const HTTPResponse& http_response
) {
  if (file == NULL) {
    auto_Object<FileSystem> file_system = FileSystem::create();
    file = file_system->open(file_path, O_CREAT | O_WRONLY | O_APPEND);
    if (file == NULL)
      return;
  }

  string entry = get_format()(http_request, http_response);

  file->write(entry.c_str(), entry.size());
}
}
}
