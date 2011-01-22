// yield/net/uri.hpp

// Copyright (c) 2010 Minor Gordon
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


#ifndef _YIELD_NET_URI_HPP_
#define _YIELD_NET_URI_HPP_


#include "yield/exception.hpp"
#include "yield/object.hpp"


#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable: 4521 ) // Multiple copy constructors
#endif


namespace yield {
class Buffer;


namespace net {
class URI : public Object {
public:
  URI
  (
    Buffer& buffer,
    const iovec& fragment,
    const iovec& host,
    const iovec& path,
    uint16_t port,
    const iovec& query,
    const iovec& scheme,
    const iovec& userinfo
  );

  URI(const char* uri) throw(Exception);
  URI(const string& uri) throw(Exception);
  URI(const char* uri, size_t uri_len) throw(Exception);

  URI(URI& other);
  URI(const URI& other);

  ~URI();

  string get_fragment() const {
    return iovec_to_string(fragment);
  }
  void get_fragment(OUT iovec& frag) const {
    frag = this->fragment;
  }
  string get_host() const {
    return iovec_to_string(host);
  }
  void get_host(OUT iovec& host) const {
    host = this->host;
  }
  string get_path() const {
    return iovec_to_string(path);
  }
  void get_path(OUT iovec& path) const {
    path = this->path;
  }
  uint16_t get_port() const {
    return port;
  }
  string get_scheme() const {
    return iovec_to_string(scheme);
  }
  void get_scheme(OUT iovec& scheme) const {
    scheme = this->scheme;
  }
  string get_query() const {
    return iovec_to_string(query);
  }
  void get_query(OUT iovec& query) const {
    query = this->query;
  }
  string get_userinfo() const {
    return iovec_to_string(userinfo);
  }
  void get_userinfo(OUT iovec& ui) const {
    ui = this->userinfo;
  }

  bool has_fragment() const {
    return fragment.iov_len > 0;
  }
  bool has_port() const {
    return port != 0;
  }
  bool has_query() const {
    return query.iov_len > 0;
  }
  bool has_userinfo() const {
    return userinfo.iov_len > 0;
  }

  operator string() const;

  URI operator+(const char*) const;
  URI operator+(const string&) const;

  void set_port(uint16_t port) {
    this->port = port;
  }

  // Object
  URI& inc_ref() {
    return Object::inc_ref(*this);
  }

private:
  friend std::ostream& operator<<(std::ostream&, const URI&);

private:
  void init(const char* uri, size_t uri_len);
  static string iovec_to_string(const iovec&);

  void
  rebase
  (
    char* old_base,
    const iovec& old_iov,
    char* new_base,
    OUT iovec& new_iov
  );

private:
  Buffer* buffer;
  iovec fragment;
  iovec host;
  iovec path;
  uint16_t port;
  iovec query;
  iovec scheme;
  iovec userinfo;
};

std::ostream& operator<<(std::ostream&, const URI&);
}
}


#ifdef _WIN32
#pragma warning( pop )
#endif
//


#endif
