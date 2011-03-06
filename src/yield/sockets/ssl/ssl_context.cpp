// yield/net/sockets/ssl/ssl_context.cpp

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

#ifdef YIELD_HAVE_OPENSSL

#include "yield/fs/path.hpp"
#include "yield/net/sockets/ssl/ssl_context.hpp"

#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#ifdef _WIN32
#pragma comment( lib, "libeay32.lib" )
#pragma comment( lib, "ssleay32.lib" )
#endif


namespace yield {
namespace net {
namespace sockets {
namespace ssl {
const char* SSLContext::TEST_PEM_CERTIFICATE = \
    "-----BEGIN CERTIFICATE-----\n"
    "MIICmTCCAgICAQYwDQYJKoZIhvcNAQEFBQAwgZAxCzAJBgNVBAYTAkVVMREwDwYD\n"
    "VQQIEwhYdHJlZW1GUzERMA8GA1UEBxMIWHRyZWVtRlMxETAPBgNVBAoTCFh0cmVl\n"
    "bUZTMQ0wCwYDVQQLEwR0ZXN0MRkwFwYDVQQDExBYdHJlZW1GUy10ZXN0LUNBMR4w\n"
    "HAYJKoZIhvcNAQkBFg9DQUB4dHJlZW1mcy5vcmcwHhcNMDkwNDA1MTEzOTMyWhcN\n"
    "MTAwNDA1MTEzOTMyWjCBmDELMAkGA1UEBhMCRVUxETAPBgNVBAgTCFh0cmVlbUZT\n"
    "MREwDwYDVQQHEwhYdHJlZW1GUzERMA8GA1UEChMIWHRyZWVtRlMxDTALBgNVBAsT\n"
    "BHRlc3QxHTAbBgNVBAMTFFh0cmVlbUZTLXRlc3QtQ2xpZW50MSIwIAYJKoZIhvcN\n"
    "AQkBFhNDbGllbnRAeHRyZWVtZnMub3JnMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCB\n"
    "iQKBgQDCjEmDQzzp4+FtNw31mJM6D85EDW8mhrX1xWBRKd4Tr3fibYjYVgkoXX03\n"
    "QmIqmtA4Rbbu7cqkq8grOdX7a2LcZ3tNaByS9C1wFc5QVqXd2ZyKj7O7bvTEawwD\n"
    "bU2gdgFgFsrt7AGT3fPeT3bqK7W0bS/Rgw0hn8OAm6QL9NqiiQIDAQABMA0GCSqG\n"
    "SIb3DQEBBQUAA4GBAJzafyIsGcyK/ZRFozgCwteu4DchMuvHmhkEa8vO5hVJwiK/\n"
    "mDFmtgCUEf3gtyqQVmYyp99OohFNVTmBndX4j/GCUVqRxfCaojzN89kpsJnHuR/B\n"
    "FVHbqso+oavB+jOgu8fWMQ6jNsT4Ql0BgIzf5Lc08m/kIodnES92qNHOdixg\n"
    "-----END CERTIFICATE-----\n";

const char* SSLContext::TEST_PEM_PRIVATE_KEY = \
    "-----BEGIN RSA PRIVATE KEY-----\n"
    "MIICXAIBAAKBgQDCjEmDQzzp4+FtNw31mJM6D85EDW8mhrX1xWBRKd4Tr3fibYjY\n"
    "VgkoXX03QmIqmtA4Rbbu7cqkq8grOdX7a2LcZ3tNaByS9C1wFc5QVqXd2ZyKj7O7\n"
    "bvTEawwDbU2gdgFgFsrt7AGT3fPeT3bqK7W0bS/Rgw0hn8OAm6QL9NqiiQIDAQAB\n"
    "AoGAeYyQuXQe8Lqxszy8ef9zrMkHRDlogpeTxBQnU05nM8Fjk5J/9zn5ysBuZUIS\n"
    "dy/2e9ICs1DaVPwGugsOiGDViGqVwzNJUo5Odst0POEEFCc04EdPlLutOdd8vaFC\n"
    "fgj7toeosynhL6nqvAzQRe1qCp0oDkxa5zwymijYp7hlbEkCQQD4tqWSrizdpHVI\n"
    "wJS/NbUD67ggoaQ37Pjdp+TkueGFgZaBMrE1XNFX39Go3bl7MK4lhU8ENc+A7s8U\n"
    "tA4aau8bAkEAyD9mHgC2V8BBE1VuIdq9FwGwu9Khl4a0OjWCRIDLNZyjSaYGNCWU\n"
    "7R+g6rHU3C9LiJvrsPz8eMZsgcfWO6/7KwJAFtDKh5Yqz3pSuAXSn2fR3zY0genS\n"
    "Ir8+ZTcNrF3Q7mpg4CFCWF7XZXRGy3LrnP1uPErqYLx++JjvrjqGQK4mmQJAJxUw\n"
    "ohRzgkb0rIhU+WiO1UmwK8hdX4yb6mx9I0+b2bfbqskrlsvq+CjfKE9TAzJ75IrC\n"
    "AoYt5IidrurI5ldfhwJBAN0odw6+SW/AiUl4maE38y5iCGwg6vOUNAiW0HinZjCE\n"
    "rarUI6jSyCc4DeiLp7DTF5n9OLdkXq8qUx8OaX5qcRU=\n"
    "-----END RSA PRIVATE KEY-----\n";

const char* SSLContext::TEST_PEM_PRIVATE_KEY_PASSPHRASE = "passphrase";


SSLContext::SSLContext(SSL_CTX& ctx)
  : ctx(ctx)
{ }

SSLContext& SSLContext::create(const SSL_METHOD* method) {
  return *new SSLContext(createSSL_CTX(method));
}

SSLContext&
SSLContext::create
(
  const SSL_METHOD* method,
  const Path& pem_certificate_file_path,
  const Path& pem_private_key_file_path,
  const string& pem_private_key_passphrase
) {
  SSL_CTX& ctx = createSSL_CTX(method);

  try {
    use_pem_certificate(ctx, pem_certificate_file_path);
    use_pem_private_key(ctx, pem_private_key_file_path, pem_private_key_passphrase);
  } catch (...) {
    SSL_CTX_free(&ctx);
    throw;
  }

  return *new SSLContext(ctx);
}

SSLContext&
SSLContext::create
(
  const SSL_METHOD* method,
  const char* pem_certificate,
  const char* pem_private_key,
  const char* pem_private_key_passphrase
) {
  return create
         (
           method,
           string(pem_certificate),
           string(pem_private_key),
           string(pem_private_key_passphrase)
         );
}

SSLContext&
SSLContext::create
(
  const SSL_METHOD* method,
  const string& pem_certificate,
  const string& pem_private_key,
  const string& pem_private_key_passphrase
) {
  SSL_CTX& ctx = createSSL_CTX(method);

  try {
    use_pem_certificate(ctx, pem_certificate);
    use_pem_private_key(ctx, pem_private_key, pem_private_key_passphrase);
  } catch (...) {
    SSL_CTX_free(&ctx);
    throw;
  }

  return *new SSLContext(ctx);
}

SSLContext& SSLContext::create
(
  const SSL_METHOD* method,
  const Path& pkcs12_file_path,
  const string& pkcs12_passphrase
) {
  SSL_CTX& ctx = createSSL_CTX(method);

  try {
    use_pkcs12(ctx, pkcs12_file_path, pkcs12_passphrase);
  } catch (...) {
    SSL_CTX_free(&ctx);
    throw;
  }

  return *new SSLContext(ctx);
}

SSLContext::~SSLContext() {
  SSL_CTX_free(&ctx);
}

SSL_CTX& SSLContext::createSSL_CTX(const SSL_METHOD* method) {
  SSL_library_init();
  OpenSSL_add_all_algorithms();

  SSL_CTX* ctx;
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
    return *ctx;
  } else
    throw SSLException();
}

int SSLContext::pem_password_callback(char* buf, int size, int, void* userdata) {
  const string* pem_password
  = static_cast<const string*>(userdata);
  if (size > static_cast<int>(pem_password->size()))
    size = static_cast<int>(pem_password->size());
  memcpy_s(buf, size, pem_password->c_str(), size);
  return size;
}

void
SSLContext::use_pem_certificate
(
  SSL_CTX& ctx,
  const string& certificate
) {
  BIO* bio
  = BIO_new_mem_buf
    (
      const_cast<char*>(certificate.c_str()),
      static_cast<int>(certificate.size())
    );

  if (bio != NULL) {
    X509* cert = PEM_read_bio_X509(bio, NULL, NULL, NULL);
    if (cert != NULL) {
      if (SSL_CTX_use_certificate(&ctx, cert) == 1) {
        BIO_free(bio);
        return;
      } else
        X509_free(cert);
    }

    BIO_free(bio);
  }

  throw SSLException();
}

void
SSLContext::use_pem_certificate
(
  SSL_CTX& ctx,
  const Path& _file_path
) {
#ifdef _WIN32
  // Need to get a string on Windows, because SSL doesn't support wide paths
  string file_path(_file_path);
#else
  const string& file_path = _file_path;
#endif

  if
  (
    SSL_CTX_use_certificate_file
    (
      &ctx,
      file_path.c_str(),
      SSL_FILETYPE_PEM
    ) == 1
  )
    return;
  else
    throw SSLException();
}

void
SSLContext::use_pem_private_key
(
  SSL_CTX& ctx,
  const string& key,
  const string& passphrase
) {
  BIO* bio
  = BIO_new_mem_buf
    (
      const_cast<char*>(key.c_str()),
      static_cast<int>(key.size())
    );

  if (bio != NULL) {
    EVP_PKEY* pkey
    = PEM_read_bio_PrivateKey
      (
        bio,
        NULL,
        pem_password_callback,
        const_cast<string*>(&passphrase)
      );

    if (pkey != NULL) {
      if (SSL_CTX_use_PrivateKey(&ctx, pkey) == 1) {
        BIO_free(bio);
        return;
      }

    }

    BIO_free(bio);
  }

  throw SSLException();
}

void
SSLContext::use_pem_private_key
(
  SSL_CTX& ctx,
  const Path& _file_path,
  const string& passphrase
) {
#ifdef _WIN32
  string file_path(_file_path);
#else
  const string& file_path = _file_path;
#endif

  SSL_CTX_set_default_passwd_cb(&ctx, pem_password_callback);
  SSL_CTX_set_default_passwd_cb_userdata(&ctx, const_cast<string*>(&passphrase));
  if (SSL_CTX_use_PrivateKey_file(&ctx, file_path.c_str(), SSL_FILETYPE_PEM)) {
    SSL_CTX_set_default_passwd_cb(&ctx, NULL);
    SSL_CTX_set_default_passwd_cb_userdata(&ctx, NULL);
  } else
    throw SSLException();
}

void
SSLContext::use_pkcs12
(
  SSL_CTX& ctx,
  const Path& _file_path,
  const string& passphrase
) {
#ifdef _WIN32
  string file_path(_file_path);
#else
  const string& file_path = _file_path;
#endif

  BIO* bio = BIO_new_file(file_path.c_str(), "rb");
  if (bio != NULL) {
    PKCS12* p12 = d2i_PKCS12_bio(bio, NULL);
    if (p12 != NULL) {
      EVP_PKEY* pkey = NULL;
      X509* cert = NULL;
      STACK_OF(X509)* ca = NULL;
      if (PKCS12_parse(p12, passphrase.c_str(), &pkey, &cert, &ca)) {
        if (pkey != NULL && cert != NULL && ca != NULL) {
          SSL_CTX_use_certificate(&ctx, cert);
          SSL_CTX_use_PrivateKey(&ctx, pkey);

          X509_STORE* store = SSL_CTX_get_cert_store(&ctx);
          for (int i = 0; i < sk_X509_num(ca); i++) {
            X509* store_cert = sk_X509_value(ca, i);
            X509_STORE_add_cert(store, store_cert);
          }

          BIO_free(bio);

          return;
        }
      }
    }

    BIO_free(bio);
  }

  throw SSLException();
}
}
}
}
}

#endif
//
