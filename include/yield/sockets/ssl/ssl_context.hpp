// yield/sockets/ssl/ssl_context.hpp

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

#ifndef _YIELD_SOCKETS_SSL_SSL_CONTEXT_HPP_
#define _YIELD_SOCKETS_SSL_SSL_CONTEXT_HPP_

#include "yield/object.hpp"
#include "yield/fs/path.hpp"
#ifdef YIELD_HAVE_OPENSSL
#include "yield/sockets/ssl/ssl_exception.hpp"
#endif

#ifdef YIELD_HAVE_OPENSSL
#include <openssl/ssl.h>
#endif

namespace yield {
namespace sockets {
#ifdef YIELD_HAVE_OPENSSL
namespace ssl {
class SSLContext : public Object {
public:
  SSLContext(SSL_CTX& ctx);

  SSLContext(const SSL_METHOD*) throw(SSLException);

  SSLContext(
    const SSL_METHOD* method,
    const yield::fs::Path& pem_certificate_file_path,
    const yield::fs::Path& pem_private_key_file_path,
    const string& pem_private_key_passphrase
  ) throw (SSLException);

  SSLContext(
    const SSL_METHOD* method,
    const char* pem_certificate,
    const char* pem_private_key,
    const char* pem_private_key_passphrase
  ) throw (SSLException);
  
  SSLContext(
    const SSL_METHOD* method,
    const string& pem_certificate,
    const string& pem_private_key,
    const string& pem_private_key_passphrase
  ) throw (SSLException);

  SSLContext(
    const SSL_METHOD* method,
    const yield::fs::Path& pkcs12_file_path,
    const string& pkcs12_passphrase
  ) throw (SSLException);

  ~SSLContext();

public:
  operator SSL_CTX* () const {
    return ctx;
  }

public:
  // yield::Object
  SSLContext& inc_ref() {
    return Object::inc_ref(*this);
  }

private:
  void init(const SSL_METHOD* method);

public:
  static int pem_password_callback(char*, int, int, void*);
  void use_pem_certificate(const string& certificate);
  void use_pem_certificate(const yield::fs::Path& file_path);
  void use_pem_private_key(const string&, const string&);
  void use_pem_private_key(const yield::fs::Path&, const string&);
  void use_pkcs12(const yield::fs::Path& file_path, const string&);

private:
  SSL_CTX* ctx;
};
}
#endif
}
}

#endif
