// test_pem_certificate.cpp

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
const char* TEST_PEM_CERTIFICATE = \
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
}
#endif
}
}
