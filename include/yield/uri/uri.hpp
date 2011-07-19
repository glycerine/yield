// yield/uri/uri.hpp

// Copyright (c) 2011 Minor Gordon
// All rights reserved

// This source file is component of the Yield project.

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

#ifndef _YIELD_URI_URI_HPP_
#define _YIELD_URI_URI_HPP_

#include "yield/exception.hpp"
#include "yield/object.hpp"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4521) // Multiple copy constructors
#endif

namespace yield {
class Buffer;

namespace uri {
/**
  A parsed Uniform Resource Identifier (URI).
*/
class URI : public Object {
public:
  /**
    Construct a URI from its constituent components:
      scheme://userinfo\@host:port/path\#fragment?query
    @param buffer the underlying buffer that the various iovecs point into
    @param fragment fragment component of the URI
    @param host host component of the URI
    @param path path component of the URI
    @param port port component of the URI
    @param query query component of the URI
    @param scheme scheme component of the URI
    @param userinfo userinfo component of the URI
  */
  URI(
    Buffer& buffer,
    const iovec& fragment,
    const iovec& host,
    const iovec& path,
    uint16_t port,
    const iovec& query,
    const iovec& scheme,
    const iovec& userinfo
  );

  /**
    Construct a URI by parsing a string.
    @param uri the URI string
  */
  URI(const char* uri) throw(Exception);

  /**
    Construct a URI by parsing a string.
    @param uri the URI string
  */
  URI(const string& uri) throw(Exception);

  /**
    Construct a URI by parsing a string.
    @param uri the URI string
    @param uri_len length of uri
  */
  URI(const char* uri, size_t uri_len) throw(Exception);

  /**
    Copy constructor that minimizes allocations by taking a reference
      to other's buffer instead of copying it.
    @param other URI to copy
  */
  URI(URI& other);

  /**
    Copy constructor that preserves const-correctness by copying other's
      buffer instead of taking a reference to it.
  */
  URI(const URI& other);

  ~URI();

public:
  /**
    Get the fragment component of the URI, copying into a temporary string.
    @return the fragment component of the URI
  */
  string get_fragment() const {
    return iovec_to_string(fragment);
  }

  /**
    Get the fragment component of the URI, returning a pointer into URI's buffer
      instead of copying.
    @param[out] fragment fragment component of the URI
  */
  void get_fragment(iovec& fragment) const {
    fragment = this->fragment;
  }

  /**
    Get the host component of the URI, copying into a temporary string.
    @return the host component of the URI
  */
  string get_host() const {
    return iovec_to_string(host);
  }

  /**
    Get the host component of the URI, returning a pointer into URI's buffer
      instead of copying.
    @param[out] host host component of the URI
  */
  void get_host(iovec& host) const {
    host = this->host;
  }

  /**
    Get the path component of the URI, copying into a temporary string.
    @return the path component of the URI
  */
  string get_path() const {
    return iovec_to_string(path);
  }

  /**
    Get the path component of the URI, returning a pointer into URI's buffer
      instead of copying.
    @param[out] path path component of the URI
  */
  void get_path(iovec& path) const {
    path = this->path;
  }

  /**
    Get the port component of the URI.
    @return the port component of the URI
  */
  uint16_t get_port() const {
    return port;
  }

  /**
    Get the scheme component of the URI, copying into a temporary string.
    @return the scheme component of the URI
  */
  string get_scheme() const {
    return iovec_to_string(scheme);
  }

  /**
    Get the scheme component of the URI, returning a pointer into URI's buffer
      instead of copying.
    @param[out] scheme scheme component of the URI
  */
  void get_scheme(iovec& scheme) const {
    scheme = this->scheme;
  }

  /**
    Get the query component of the URI, copying into a temporary string.
    @return the query component of the URI
  */
  string get_query() const {
    return iovec_to_string(query);
  }

  /**
    Get the query component of the URI, returning a pointer into URI's buffer
      instead of copying.
    @param[out] query query component of the URI
  */
  void get_query(iovec& query) const {
    query = this->query;
  }

  /**
    Get the userinfo component of the URI, copying into a temporary string.
    @return the userinfo component of the URI
  */
  string get_userinfo() const {
    return iovec_to_string(userinfo);
  }

  /**
    Get the userinfo component of the URI, returning a pointer into URI's buffer
      instead of copying.
    @param[out] userinfo userinfo component of the URI
  */
  void get_userinfo(iovec& userinfo) const {
    userinfo = this->userinfo;
  }

public:
  /**
    Check if the URI has a fragment component.
    @return true if the URI has a fragment component
  */
  bool has_fragment() const {
    return fragment.iov_len > 0;
  }

  /**
    Check if the URI has a host component.
    @return true if the URI has a host component
  */
  bool has_host() const {
    return host.iov_len > 0;
  }

  /**
    Check if the URI has a path component.
    @return true if the URI has a path component
  */
  bool has_path() const {
    return path.iov_len > 0;
  }

  /**
    Check if the URI has a port component.
    @return true if the URI has a port component
  */
  bool has_port() const {
    return port != 0;
  }

  /**
    Check if the URI has a query component.
    @return true if the URI has a query component
  */
  bool has_query() const {
    return query.iov_len > 0;
  }


  /**
    Check if the URI has a scheme component.
    @return true if the URI has a scheme component
  */
  bool has_scheme() const {
    return scheme.iov_len > 0;
  }

  /**
    Check if the URI has a userinfo component.
    @return true if the URI has a userinfo component
  */
  bool has_userinfo() const {
    return userinfo.iov_len > 0;
  }

public:
  /**
    Return a string representation of the URI.
    @return a string representation of the URI
  */
  operator string() const;

public:
  /**
    Compare two URIs for equality.
    @param other URI to compare this one to
    @return true if the two URIs are equal
  */
  bool operator==(const URI& other) const;

public:
  /**
    Concatenate a string to the URI.
    @param s string to concatenate to the URI
    @return a new URI with the added string
  */
  URI operator+(const char* s) const;

  /**
    Concatenate a string to the URI.
    @param s string to concatenate to the URI
    @return a new URI with the added string
  */
  URI operator+(const string& s) const;

public:
  /**
    Set the port component of the URI.
    @param port new port component of the URI
  */
  void set_port(uint16_t port) {
    this->port = port;
  }

public:
  // yield::Object
  URI& inc_ref() {
    return Object::inc_ref(*this);
  }

private:
  friend std::ostream& operator<<(std::ostream&, const URI&);

private:
  void init(const char* uri, size_t uri_len);
  static string iovec_to_string(const iovec&);

  void
  rebase(
    char* old_base,
    const iovec& old_iov,
    char* new_base,
    iovec& new_iov
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
#pragma warning(pop)
#endif
//

#endif
