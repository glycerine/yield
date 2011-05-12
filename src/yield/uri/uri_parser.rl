// yield/uri/uri_parser.rl

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

#include "yield/uri/uri_parser.hpp"

#include <stdlib.h> // For atoi

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4702)
#endif

namespace yield {
namespace uri {
bool
URIParser::parse(
  iovec& fragment,
  iovec& host,
  iovec& path,
  uint16_t& port,
  iovec& query,
  iovec& scheme,
  iovec& userinfo
) {
  memset(&fragment, 0, sizeof(fragment));
  memset(&host, 0, sizeof(host));
  memset(&path, 0, sizeof(path));
  port = 0;
  memset(&query, 0, sizeof(query));
  memset(&scheme, 0, sizeof(scheme));
  memset(&userinfo, 0, sizeof(userinfo));

  int cs;
  const char* eof = pe;

  %%{
    machine parse_uri_reference;
    alphtype unsigned char;

    include rfc3986 "../../../include/yield/uri/rfc3986.rl";

    main := uri_reference;

    write data;
    write init;
    write exec;
  }%%

  return cs != parse_uri_reference_error;
}
}
}

#ifdef _WIN32
#pragma warning(pop)
#endif
//
