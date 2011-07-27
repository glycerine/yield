// yield/http/server/file/http_file_request_handler.cpp

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
#include "yield/fs/file_system.hpp"
#include "yield/http/server/http_request.hpp"
#include "yield/http/server/file/http_file_request_handler.hpp"

namespace yield {
namespace http {
namespace server {
namespace file {
using yield::fs::File;
using yield::fs::FileSystem;
using yield::fs::Path;
using yield::uri::URI;

HTTPFileRequestHandler::HTTPFileRequestHandler(
  const Path& root_directory_path,
  const URI& root_uri
) : root_directory_path(root_directory_path),
  root_uri(root_uri) {
  this->root_uri.get_path(root_uri_path);
}

void HTTPFileRequestHandler::handle(YO_NEW_REF Event& event) {
  debug_assert_eq(event.get_type_id(), HTTPRequest::TYPE_ID);
  HTTPRequest& http_request = static_cast<HTTPRequest&>(event);

  iovec http_request_uri_path;
  http_request.get_uri().get_path(http_request_uri_path);

  if (http_request_uri_path.iov_len > root_uri_path.iov_len) {
    if (
      memcmp(
        http_request_uri_path.iov_base,
        root_uri_path.iov_base,
        root_uri_path.iov_len
      ) == 0
    ) {
      Path file_relpath(
        static_cast<char*>(http_request_uri_path.iov_base) +
        root_uri_path.iov_len,
        http_request_uri_path.iov_len - root_uri_path.iov_len
      );

      Path file_abspath = root_directory_path / file_relpath;

      File* file = FileSystem().open(file_abspath);
      if (file != NULL) {
        File::Map* map = file->mmap(SIZE_MAX, 0, true);
        if (map != NULL) {
          http_request.respond(200, *map);
        } else {
          http_request.respond(404, Exception());
        }

        File::dec_ref(*file);
      } else {
        http_request.respond(404, Exception());
      }

      HTTPRequest::dec_ref(http_request);
    }
  }
}
}
}
}
}
