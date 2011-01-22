// yield/net/sockets/ssl/ssl_context.hpp

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


#ifndef _YIELD_NET_SOCKETS_SSL_SSL_CONTEXT_HPP_
#define _YIELD_NET_SOCKETS_SSL_SSL_CONTEXT_HPP_


#ifdef YIELD_HAVE_OPENSSL
struct ssl_ctx_st;
typedef ssl_ctx_st SSL_CTX;
struct ssl_method_st;
typedef ssl_method_st SSL_METHOD;
#endif


namespace yield {
namespace net {
namespace sockets {
#ifdef YIELD_HAVE_OPENSSL
namespace ssl {
class SSLContext : public Object {
public:
  static const char* TEST_PEM_CERTIFICATE;
  static const char* TEST_PEM_PRIVATE_KEY;
  static const char* TEST_PEM_PRIVATE_KEY_PASSPHRASE;

public:
  ~SSLContext();

  static YO_NEW_REF SSLContext& create( const SSL_METHOD* );

  static YO_NEW_REF SSLContext&
  create
  (
    const SSL_METHOD* method,
    const Path& pem_certificate_file_path,
    const Path& pem_private_key_file_path,
    const string& pem_private_key_passphrase
  );

  static YO_NEW_REF SSLContext&
  create
  (
    const SSL_METHOD* method,
    const char* pem_certificate,
    const char* pem_private_key,
    const char* pem_private_key_passphrase
  );

  static YO_NEW_REF SSLContext&
  create
  (
    const SSL_METHOD* method,
    const string& pem_certificate,
    const string& pem_private_key,
    const string& pem_private_key_passphrase
  );

  static YO_NEW_REF SSLContext&
  create
  (
    const SSL_METHOD* method,
    const Path& pkcs12_file_path,
    const string& pkcs12_passphrase
  );

  operator SSL_CTX*() const {
    return &ctx;
  }

  // Object
  SSLContext& inc_ref() {
    return Object::inc_ref( *this );
  }

private:
  SSLContext( SSL_CTX& ctx );

  static SSL_CTX& createSSL_CTX( const SSL_METHOD* );
  static int pem_password_callback( char*, int, int, void* );
  static void use_pem_certificate( SSL_CTX&, const string& certificate );
  static void use_pem_certificate( SSL_CTX&, const Path& file_path );
  static void use_pem_private_key( SSL_CTX&, const string&, const string& );
  static void use_pem_private_key( SSL_CTX&, const Path&, const string& );
  static void use_pkcs12( SSL_CTX&, const Path& file_path, const string& );

private:
  SSL_CTX& ctx;
};
}
#endif
}
}
}


#endif
