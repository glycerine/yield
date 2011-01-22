// yield/net/uri.rl

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
#include "yield/string_buffer.hpp"
#include "yield/exception.hpp"
#include "yield/net/uri.hpp"

#include <sstream> // or std::ostringstream
#include <stdlib.h> // For atoi

#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable: 4702 )
#endif

%%{
  machine uri;
  alphtype unsigned char;


  gen_delims    = ":" | "|" | "?" | "#" | "[" | "]" | "@";
  sub_delims    = "!" | "$" | "&" | "'" | "(" | ")" | "*" | "+" | "," | ";" | "=";
  pct_encoded   = "%" xdigit xdigit;
  reserved      = gen_delims | sub_delims;
  unreserved    = alpha | digit | "-" | "." | "_" | "~";
  pchar         = unreserved | pct_encoded | sub_delims | ":" | "@";


  IPvFuture     = "v" xdigit+ "." ( unreserved | sub_delims | ":" )+;

  dec_octet     = digit |
                  ( [1-9] digit ) |
                  ( "1" digit{2}  ) |
                  ( "2" [0-4] digit ) |
                  ( "25" [0-5] );
  IPv4address   = dec_octet "." dec_octet "." dec_octet "." dec_octet;

  h16           = ( xdigit{4} )+;
  ls32          = ( h16 ":" h16 ) | IPv4address;
  IPv6address   = ( ( h16 ":" ){6} ls32 ) |
                  ( "::" ( h16 ":" ){5} ls32 ) |
                  ( h16? "::" ( h16 ":" ){4} ls32 ) |
                  ( ( ( ( h16 ":" ){1} )* h16 )? "::" ( h16 ":" ){3} ls32 ) |
                  ( ( ( ( h16 ":" ){2} )* h16 )? "::" ( h16 ":" ){2} ls32 ) |
                  ( ( ( ( h16 ":" ){3} )* h16 )? "::" h16 ":"   ls32 ) |
                  ( ( ( ( h16 ":" ){4} )* h16 )? "::" ls32 ) |
                  ( ( ( ( h16 ":" ){5} )* h16 )? "::" h16 ) |
                  ( ( ( ( h16 ":" ){6} )* h16 )? "::" );

  ip_literal    = "[" ( IPv6address | IPvFuture  ) "]";


  scheme        = ( alpha ( alpha | digit | "+" | "-" | "." )* )
                  >{ scheme.iov_base = p; }
                  %{ scheme.iov_len =  p - static_cast<char*>( scheme.iov_base ); };


  userinfo      = ( unreserved | pct_encoded | sub_delims | ":" )*
                  >{ userinfo.iov_base = p; }
                  %{ userinfo.iov_len = p - static_cast<char*>( userinfo.iov_base ); };

  reg_name      = ( unreserved | pct_encoded | sub_delims )*;
  host          = ( ip_literal | IPv4address | reg_name )
                  >{ host.iov_base = p; }
                  %{ host.iov_len = p - static_cast<char*>( host.iov_base ); };

  port          = digit*
                  >{ port = static_cast<uint16_t>( atoi( p ) ); };

  authority     = ( userinfo "@" )? host ( ":" port )?;


  action path_enter { path.iov_base = p; }
  action path_leave { path.iov_len = p - static_cast<char*>( path.iov_base ); }

  segment       = pchar*;
  segment_nz    = pchar+;
  segment_nz_nc = ( unreserved | pct_encoded | sub_delims | "@" )+;
  path_abempty  = ( "/" segment )* >path_enter %path_leave;
  path_absolute = ( "/" ( segment_nz ( "/" segment )* )? ) >path_enter %path_leave;
  path_noscheme = ( segment_nz_nc ( "/" segment )* ) >path_enter %path_leave;
  path_rootless = ( segment_nz ( "/" segment )* ) >path_enter %path_leave;
  path_empty    = empty;

  hier_part = ( "//" authority path_abempty ) |
              path_absolute |
              path_rootless |
              path_empty;


  query         = ( pchar | "/" | "?" )*
                  >{ query.iov_base = p; }
                  %{ query.iov_len = p - static_cast<char*>( query.iov_base ); };
  fragment      = ( pchar | "/" | "?" )*
                  >{ fragment.iov_base = p; }
                  %{ fragment.iov_len = p - static_cast<char*>( fragment.iov_base ); };


  relative_part = ( "//" authority path_abempty ) |
                  path_absolute |
                  path_noscheme |
                  path_empty;
  relative_ref  = relative_part ( "?" query )? ( "#" fragment )?;


  absolute_uri  = scheme ":" hier_part ( "?" query )?;
  uri           = scheme ":" hier_part ( "?" query )? ( "#" fragment )?;
  uri_reference = uri | relative_ref;
}%%

namespace yield {
namespace net {
URI::URI
(
  Buffer& buffer,
  const iovec& fragment,
  const iovec& host,
  const iovec& path,
  uint16_t port,
  const iovec& query,
  const iovec& scheme,
  const iovec& userinfo
)
  : buffer(&buffer.inc_ref()),
    fragment(fragment),
    host(host),
    path(path),
    port(port),
    query(query),
    scheme(scheme),
    userinfo(userinfo) {
  debug_assert_ne(host.iov_base, NULL);
  debug_assert_gt(host.iov_len, 0);
  debug_assert_ne(path.iov_base, NULL);
  debug_assert_ne(scheme.iov_base, NULL);
  debug_assert_gt(scheme.iov_len, 0);
}

URI::URI(const char* uri) throw(Exception)
  : buffer(NULL) {
  init(uri, strlen(uri));
}

URI::URI(const string& uri) throw(Exception)
  : buffer(NULL) {
  init(uri.data(), uri.size());
}

URI::URI(const char* uri, size_t uri_len) throw(Exception)
  : buffer(NULL) {
  init(uri, uri_len);
}

URI::URI(URI& other)
  : buffer(&other.buffer->inc_ref()),
    fragment(other.fragment),
    host(other.host),
    path(other.path),
    port(other.port),
    query(other.query),
    scheme(other.scheme),
    userinfo(other.userinfo)
{ }

URI::URI(const URI& other)
  : port(other.port) {
  this->buffer = new StringBuffer(*other.buffer);

  char* old_base = static_cast<char*>(*other.buffer);
  char* new_base = static_cast<char*>(*this->buffer);
  rebase(old_base, other.fragment, new_base, fragment);
  rebase(old_base, other.host, new_base, host);
  rebase(old_base, other.path, new_base, path);
  rebase(old_base, other.query, new_base, query);
  rebase(old_base, other.scheme, new_base, scheme);
  rebase(old_base, other.userinfo, new_base, userinfo);
}

URI::~URI() {
  Buffer::dec_ref(*buffer);
}

void URI::init(const char* uri, size_t uri_len) {
  buffer = new StringBuffer(uri, uri_len);

  memset(&fragment, 0, sizeof(fragment));
  memset(&host, 0, sizeof(host));
  memset(&path, 0, sizeof(path));
  port = 0;
  memset(&query, 0, sizeof(query));
  memset(&scheme, 0, sizeof(scheme));
  memset(&userinfo, 0, sizeof(userinfo));

  %%{
    machine init;
    alphtype unsigned char;

    include uri;

    main := uri;

    write data;
    write init;
    write exec;
  }%%

  if (cs == init_error)
    throw Exception();
}

string URI::iovec_to_string(const iovec& iovec_) {
  return string(static_cast<char*>(iovec_.iov_base), iovec_.iov_len);
}

std::ostream& operator<<(std::ostream& os, const URI& uri) {
  os.write(static_cast<char*>(uri.scheme.iov_base), uri.scheme.iov_len);
  os.write("://", 3);

  if (uri.has_userinfo())
    os.write(static_cast<char*>(uri.userinfo.iov_base), uri.userinfo.iov_len);

  os.write(static_cast<char*>(uri.host.iov_base), uri.host.iov_len);

  if (uri.has_port())
    os << ":" << uri.port;

  os.write(static_cast<char*>(uri.path.iov_base), uri.path.iov_len);

  if (uri.has_query()) {
    os.write("?", 1);
    os.write(static_cast<char*>(uri.query.iov_base), uri.query.iov_len);
  }

  if (uri.has_fragment()) {
    os.write("#", 1);
    os.write(static_cast<char*>(uri.fragment.iov_base), uri.fragment.iov_len);
  }

  return os;
}

URI::operator string() const {
  std::ostringstream uri;
  uri << *this;
  return uri.str();
}

URI URI::operator+(const char* s) const {
  std::ostringstream uri;
  uri << *this;
  uri << s;
  return URI(uri.str());
}

URI URI::operator+(const string& s) const {
  std::ostringstream uri;
  uri << *this;
  uri << s;
  return URI(uri.str());
}

void
URI::rebase
(
  char* old_base,
  const iovec& old_iov,
  char* new_base,
  OUT iovec& new_iov
) {
  if (old_iov.iov_base != NULL) {
    if (old_iov.iov_base >= old_base) {
      new_iov.iov_base
      = new_base + (static_cast<char*>(old_iov.iov_base) - old_base);
      new_iov.iov_len = old_iov.iov_len;
    } else // assume old_iov points to a literal
      new_iov = old_iov;
  } else
    memset(&new_iov, 0, sizeof(new_iov));
}
}
}

#ifdef _WIN32
#pragma warning( pop )
#endif
//
