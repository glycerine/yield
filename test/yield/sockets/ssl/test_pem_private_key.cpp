// test_pem_private_key.cpp

// Copyright (c) 2012 Minor Gordon
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

namespace yield {
namespace sockets {
#ifdef YIELD_HAVE_OPENSSL
namespace ssl {
const char* TEST_PEM_PRIVATE_KEY = \
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

const char* TEST_PEM_PRIVATE_KEY_PASSPHRASE = "passphrase";
}
#endif
}
}
