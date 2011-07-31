// yield/uri/uri_parser.hpp

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

#ifndef _YIELD_URI_URI_PARSER_HPP_
#define _YIELD_URI_URI_PARSER_HPP_

#include "yield/buffer.hpp"

namespace yield {
namespace uri {
/**
  An RFC 3986-conformant URI parser.
*/
class URIParser {
public:
  /**
    Construct a URI parser on the specified string.
    @param ps pointer to the beginning of the URI string
    @param pe pointer to the end of the URI string
  */
  URIParser(const char* ps, const char* pe)
    : p(const_cast<char*>(ps)), pe(pe), ps(ps) {
  }

  /**
    Construct a URI parser on the specified buffer.
    @param buffer buffer with the URI
  */
  URIParser(Buffer& buffer) {
    p = buffer;
    ps = buffer;
    pe = static_cast<const char*>(buffer) + buffer.size();
  }

  /**
    Parse a URI from the string or buffer specified in the constructor.
    @param[out] fragment component of the URI
    @param[out] host component of the URI
    @param[out] path component of the URI
    @param[out] port component of the URI
    @param[out] query component of the URI
    @param[out] scheme component of the URI
    @param[out] userinfo component of the URI
    @return true if a URI was successfully parsed, false on failure
  */
  bool parse(
    iovec& fragment,
    iovec& host,
    iovec& path,
    uint16_t& port,
    iovec& query,
    iovec& scheme,
    iovec& userinfo
  );

private:
  char* p;
  const char* pe, *ps;
};
}
}

#endif
