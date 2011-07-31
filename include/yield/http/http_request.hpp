// yield/http/http_request.hpp

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

#ifndef _YIELD_HTTP_HTTP_REQUEST_HPP_
#define _YIELD_HTTP_HTTP_REQUEST_HPP_

#include "yield/date_time.hpp"
#include "yield/exception.hpp"
#include "yield/http/http_message.hpp"
#include "yield/uri/uri.hpp"

namespace yield {
namespace http {
/**
  An RFC 2616 HTTP request.
  Unlike its counterparts in yield.http.client and yield.http.server, this
    HTTPRequest is not tied to a particular connection.
*/
class HTTPRequest : public HTTPMessage<HTTPRequest> {
public:
  const static uint32_t TYPE_ID = 707981577;

public:
  /**
    A method enum class.
  */
  class Method {
  public:
    /**
      Known methods from RFC 2616.
    */
    static Method CONNECT;
    static Method COPY;
    static Method DELETE;
    static Method GET;
    static Method HEAD;
    static Method LOCK;
    static Method MKCOL;
    static Method MOVE;
    static Method OPTIONS;
    static Method PATCH;
    static Method POST;
    static Method PROPFIND;
    static Method PROPPATCH;
    static Method PUT;
    static Method TRACE;
    static Method UNLOCK;

  public:
    /**
      Construct an empty Method. Used in parsing.
    */
    Method()
      : id(0), name(NULL), name_len(0) {
    }

    /**
      Construct a method with a unique numeric identifier (for switches) and
        string representation.
      @param id unique numeric identifier of this method
      @param name string representation of this method e.g., GET
      @param name_len length of name in bytes
    */
    Method(uint8_t id, const char* name, uint8_t name_len)
      : id(id), name(name), name_len(name_len) {
    }

    /**
      Copy constructor.
    */
    Method(const Method& other)
      : id(other.id), name(other.name), name_len(other.name_len) {
    }

  public:
    /**
      Get the unique numeric identifier of this method.
      @return the unique numeric identifier of this method
    */
    uint8_t get_id() const {
      return id;
    }

    /**
      Get the string representation of this method e.g., GET.
      @return the string representation of this method
    */
    const char* get_name() const {
      return name;
    }

    /**
      Get the length in bytes of the string representation of this method.
      @return the length in bytes of the string representation of this method
    */
    uint8_t get_name_len() const {
      return name_len;
    }

  public:
    /**
      Get the string representation of this method e.g., GET.
      @return the string representation of this method
    */
    operator const char* () const {
      return name;
    }

    /**
      Get the unique numeric identifier of this method.
      @return the unique numeric identifier of this method
    */
    operator uint8_t() const {
      return id;
    }

  public:
    /**
      Assignment operator that copies the contents of another 
        Method to this one. Used in parsing.
    */
    Method& operator=(const Method& other) {
      id = other.id;
      name = other.name;
      name_len = other.name_len;
      return *this;
    }

    /**
      Compare two Method objects for equality using their unique numeric ids.
      @param other the Method to compare this one to
      @return true if the two Methods are equal
    */
    bool operator==(const Method& other) const {
      return id == other.id;
    }

    /**
      Compare two Method objects for inequality using their unique numeric ids.
      @param other the Method to compare this one to
      @return true if the two Methods are not equal
    */
    bool operator!=(const Method& other) const {
      return !operator==(other);
    }

  public:
    /**
      Parse a Method from a string representation.
      @param method string representation of a method e.g., GET
      @return the Method corresponding to the string
    */
    static Method parse(const char* method) throw(Exception);

  private:
    uint8_t id;
    const char* name;
    uint8_t name_len;
  };

public:
  /**
    Construct an HTTPRequest from its constituent parts.
    @param method the HTTP request method e.g., HTTPRequest::Method::GET
    @param uri the HTTP request URI e.g., /
    @param body an optional body, usually a Buffer
    @param http_version the HTTP version as a single byte (0 or 1 for HTTP/1.0
      and HTTP/1.1, respectively)
  */
  HTTPRequest(
    Method method,
    const yield::uri::URI& uri,
    YO_NEW_REF Object* body = NULL,
    uint8_t http_version = HTTP_VERSION_DEFAULT
  );

  /**
    Empty virtual destructor.
  */
  virtual ~HTTPRequest() { }

public:
  /**
    Get the HTTP request method e.g., HTTPRequest::Method::GET
    @return the request method
  */
  Method get_method() const {
    return method;
  }

  /**
    Get the HTTP request URI e.g., /
    @return the request URI
  */
  const yield::uri::URI& get_uri() const {
    return uri;
  }

public:
  // yield::Object
  HTTPRequest& inc_ref() {
    return Object::inc_ref(*this);
  }

public:
  // yield::Event
  uint32_t get_type_id() const {
    return TYPE_ID;
  }

  const char* get_type_name() const {
    return "yield::http::HTTPRequest";
  }

protected:
  friend class HTTPRequestParser;

  HTTPRequest(
    YO_NEW_REF Object* body,
    uint16_t fields_offset,
    Buffer& header,
    uint8_t http_version,
    Method method,
    const yield::uri::URI& uri
  );

private:
  Method method;
  yield::uri::URI uri;
};

std::ostream& operator<<(std::ostream&, const HTTPRequest&);
std::ostream& operator<<(std::ostream&, const HTTPRequest::Method&);
}
}

#endif
