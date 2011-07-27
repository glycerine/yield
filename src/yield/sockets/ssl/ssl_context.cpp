// yield/sockets/ssl/ssl_context.cpp

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

#include "ssl_exception.hpp"
#include "yield/assert.hpp"
#include "yield/sockets/ssl/ssl_context.hpp"

#ifdef YIELD_HAVE_OPENSSL
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#ifdef _WIN32
//#pragma comment(lib, "advapi32.lib")
//#pragma comment(lib, "gdi32.lib")
//#pragma comment(lib, "libeay32MTd.lib")
//#pragma comment(lib, "ssleay32MTd.lib")
//#pragma comment(lib, "user32.lib")
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif
#endif

namespace yield {
namespace sockets {
#ifdef YIELD_HAVE_OPENSSL
namespace ssl {
SSLContext::SSLContext(SSLVersion ssl_version) throw(Exception) {
  init(ssl_version);
}

SSLContext::SSLContext(
  const string& pem_certificate,
  const string& pem_private_key,
  const string& pem_private_key_passphrase,
  SSLVersion ssl_version
) throw(Exception) {
  init(ssl_version);
  use_pem_certificate(pem_certificate);
  use_pem_private_key(pem_private_key, pem_private_key_passphrase);
}

SSLContext::~SSLContext() {
  SSL_CTX_free(ctx);
}

void SSLContext::init(SSLVersion ssl_version) {
  SSL_load_error_strings();
  SSL_library_init();

  const SSL_METHOD* method;
  switch (ssl_version) {
  case SSL_VERSION_2:
    method = SSLv2_method();
    break;
  case SSL_VERSION_23:
    method = SSLv23_method();
    break;
  case SSL_VERSION_3:
    method = SSLv3_method();
    break;
  case TLS_VERSION_1:
    method = TLSv1_method();
    break;
  case DTLS_VERSION_1:
    method = DTLSv1_method();
    break;
  default:
    debug_break();
    method = SSLv23_method();
    break;
  }

#if OPENSSL_VERSION_NUMBER >= 0x10000000L
  ctx = SSL_CTX_new(method);
#else
  ctx = SSL_CTX_new(const_cast<SSL_METHOD*>(method));
#endif

  if (ctx != NULL) {
#ifdef SSL_OP_NO_TICKET
    SSL_CTX_set_options(ctx, SSL_OP_ALL | SSL_OP_NO_TICKET);
#else
    SSL_CTX_set_options(ctx, SSL_OP_ALL);
#endif
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
  } else {
    throw SSLException();
  }
}

int SSLContext::pem_password_callback(char* buf, int size, int, void* userdata) {
  const string* pem_password
  = static_cast<const string*>(userdata);
  if (size > static_cast<int>(pem_password->size())) {
    size = static_cast<int>(pem_password->size());
  }
  memcpy_s(buf, size, pem_password->c_str(), size);
  return size;
}

void
SSLContext::use_pem_certificate(
  const string& pem_certificate
) throw(Exception) {
  try {
    BIO* bio
    = BIO_new_mem_buf(
        const_cast<char*>(pem_certificate.data()),
        static_cast<int>(pem_certificate.size())
      );

    if (bio != NULL) {
      try {
        X509* cert = PEM_read_bio_X509(bio, NULL, NULL, NULL);
        if (cert != NULL) {
          try {
            if (SSL_CTX_use_certificate(ctx, cert) == 1) {
              BIO_free(bio);
              return;
            } else {
              throw SSLException();
            }
          } catch (SSLException&) {
            X509_free(cert);
            throw;
          }
        } else {
          throw SSLException();
        }
      } catch (SSLException&) {
        BIO_free(bio);
        throw;
      }
    } else {
      throw SSLException();
    }
  } catch (SSLException&) {
    use_pem_certificate_file(pem_certificate);
  }
}

void
SSLContext::use_pem_certificate_file(
  const string& pem_certificate_file_path
) throw(Exception) {
  if (
    SSL_CTX_use_certificate_file(
      ctx,
      pem_certificate_file_path.c_str(),
      SSL_FILETYPE_PEM
    ) != 1
  ) {
    throw SSLException();
  }
}

void
SSLContext::use_pem_private_key(
  const string& pem_private_key,
  const string& pem_private_key_passphrase
) throw(Exception) {
  try {
    BIO* bio
    = BIO_new_mem_buf(
        const_cast<char*>(pem_private_key.data()),
        static_cast<int>(pem_private_key.size())
      );

    if (bio != NULL) {
      try {
        EVP_PKEY* pkey
        = PEM_read_bio_PrivateKey(
            bio,
            NULL,
            pem_password_callback,
            const_cast<string*>(&pem_private_key_passphrase)
          );

        if (pkey != NULL) {
          if (SSL_CTX_use_PrivateKey(ctx, pkey) == 1) {
            BIO_free(bio);
          } else {
            throw SSLException();
          }
        } else {
          throw SSLException();
        }
      } catch (SSLException&) {
        BIO_free(bio);
        throw;
      }
    } else {
      throw SSLException();
    }
  } catch (SSLException&) {
    use_pem_private_key_file(pem_private_key, pem_private_key_passphrase);
  }
}

void
SSLContext::use_pem_private_key_file(
  const string& pem_private_key_file_path,
  const string& pem_private_key_passphrase
) throw(Exception) {
  SSL_CTX_set_default_passwd_cb(ctx, pem_password_callback);
  SSL_CTX_set_default_passwd_cb_userdata(
    ctx,
    const_cast<string*>(&pem_private_key_passphrase)
  );

  if (
    SSL_CTX_use_PrivateKey_file(
      ctx,
      pem_private_key_file_path.c_str(),
      SSL_FILETYPE_PEM
    )
  ) {
    SSL_CTX_set_default_passwd_cb(ctx, NULL);
    SSL_CTX_set_default_passwd_cb_userdata(ctx, NULL);
  } else {
    throw SSLException();
  }
}

//void
//SSLContext::use_pkcs12(
//  const Path& pkcs12_file_path,
//  const string& passphrase
//) {
//#ifdef _WIN32
//  string pkcs12_file_path_ =
//    pkcs12_file_path.encode(yield::i18n::win32::Code::CHAR);
//#else
//  const string& pkcs12_file_path_ = pkcs12_file_path;
//#endif
//
//  BIO* bio = BIO_new_file(pkcs12_file_path_.c_str(), "rb");
//  if (bio != NULL) {
//    try {
//      PKCS12* p12 = d2i_PKCS12_bio(bio, NULL);
//      if (p12 != NULL) {
//        EVP_PKEY* pkey = NULL;
//        X509* cert = NULL;
//        STACK_OF(X509)* ca = NULL;
//        if (PKCS12_parse(p12, passphrase.c_str(), &pkey, &cert, &ca)) {
//          if (pkey != NULL && cert != NULL && ca != NULL) {
//            SSL_CTX_use_certificate(ctx, cert);
//            SSL_CTX_use_PrivateKey(ctx, pkey);
//
//            X509_STORE* store = SSL_CTX_get_cert_store(ctx);
//            for (int i = 0; i < sk_X509_num(ca); i++) {
//              X509* store_cert = sk_X509_value(ca, i);
//              X509_STORE_add_cert(store, store_cert);
//            }
//
//            BIO_free(bio);
//          } else
//            throw SSLException();
//        } else
//          throw SSLException();
//      } else
//        throw SSLException();
//    } catch (SSLException&) {
//      BIO_free(bio);
//      throw;
//    }
//  } else
//    throw SSLException();
//}
}
#endif
}
}
