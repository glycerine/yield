# http_request_parser_test.py

# Copyright (c) 2011 Minor Gordon
# All rights reserved

# This source file is part of the Yield project.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# * Neither the name of the Yield project nor the
# names of its contributors may be used to endorse or promote products
# derived from this software without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from http_message_parser_test import *


# Constants
URI = "/"


class HTTPRequestParserTest(HTTPMessageParserTest):
    def ASSERT_1_METHOD(self, method="GET"):
        self.append("""throw_assert_eq(http_request->get_method(), HTTPRequest::METHOD_%(method)s);""" % locals())

    def ASSERT_1_URI(self):
        URI = globals()["URI"]
        self.append("""throw_assert_eq(strcmp(http_request->get_uri(), "%(URI)s"), 0);""" % locals())

    def ASSERT_N_METHOD(self, n, method="GET"):
        self.append("""for (size_t i = 0; i < %(n)u; i++) { throw_assert_eq(http_requests[i]->get_method(), HTTPRequest::METHOD_%(method)s); }""" % locals())

    def ASSERT_N_URI(n):
        URI = globals()["URI"]
        self.append("""for (size_t i = 0; i < %(n)u; i++) { throw_assert_eq(strcmp(http_requests[i]->get_uri(), "%(URI)s"), 0); }""" % locals())


class MalformedHTTPVersionMissingHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_1("GET", ' ', URI, CRLF, HOST_FIELD, CRLF)
        self.ASSERT_1_NULL()


class MalformedHTTPVersionMissingHTTPHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_1("GET", ' ', URI, ' ', "/1.0", CRLF, HOST_FIELD, CRLF)
        self.ASSERT_1_NULL()


class MalformedHTTPVersionMissingMinorVersionHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_1("GET", ' ', URI, " HTTP/1.", CRLF, HOST_FIELD, CRLF)
        self.ASSERT_1_NULL()


class MalformedHTTPVersionMissingTrailingCRLFHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_1("GET", ' ', URI, ' ', HTTP_VERSION, HOST_FIELD, CRLF)
        self.ASSERT_1_NULL()


class MalformedMethodMissingHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_1(URI, " HTTP/1.0", CRLF, HOST_FIELD, CRLF)
        self.ASSERT_1_NULL()


class MalformedURIEmbeddedLFHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_1("GET", " /\\r ", HTTP_VERSION, CRLF, HOST_FIELD, CRLF)
        self.ASSERT_1_NULL()


class MalformedURIMissingHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_1("GET", ' ', HTTP_VERSION, CRLF, HOST_FIELD, CRLF)
        self.ASSERT_1_NULL()


class WellFormedChunk1BodyHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_N(
            2,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF,
            HOST_FIELD,
            TE_CHUNKED_FIELD,
            CRLF,
            "1" + CRLF + "x" + CRLF + "0" + CRLF * 2
        )
        self.ASSERT_N(2)
        self.ASSERT_N_METHOD(2)
        # ASSERT_N_URI(2)
        self.ASSERT_N_HTTP_VERSION(2)
        self.ASSERT_N_BODY_NONNULL(2)
        self.append("throw_assert_eq(http_requests[0]->get_content_length(), 1);")
        self.append("throw_assert_eq(http_requests[1]->get_content_length(), 0);")
        self.DEC_REF_N(2)


class WellFormedChunk2BodyHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_N(
            3,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF,
            HOST_FIELD,
            TE_CHUNKED_FIELD,
            CRLF,
            "1" + CRLF + "x" + CRLF + "1" + CRLF + "y" + CRLF + "0" + CRLF * 2
        )
        self.ASSERT_N(3)
        self.ASSERT_N_METHOD(3)
        # ASSERT_N_URI(3)
        self.ASSERT_N_HTTP_VERSION(3)
        self.ASSERT_N_HOST_FIELD(3)
        self.ASSERT_N_BODY_NONNULL(3)
        self.append("throw_assert_eq(http_requests[0]->get_content_length(), 1);")
        self.append("throw_assert_eq(http_requests[1]->get_content_length(), 1);")
        self.append("throw_assert_eq(http_requests[2]->get_content_length(), 0);")
        self.DEC_REF_N(3)


class WellFormedNoBodyHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_1("GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CRLF)
        self.ASSERT_1_NONNULL()
        self.ASSERT_1_METHOD()
        self.ASSERT_1_HTTP_VERSION()
        self.ASSERT_1_HOST_FIELD()
        self.ASSERT_1_BODY_NULL()
        self.DEC_REF_1()


class WellFormedNoFieldsHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_1("GET", ' ', URI, ' ', HTTP_VERSION, CRLF, CRLF)
        self.ASSERT_1_NONNULL()
        self.ASSERT_1_HTTP_VERSION()
        self.ASSERT_1_BODY_NULL()
        self.DEC_REF_1()


class WellFormedNormalBodyHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_1("GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CL2_FIELD, CRLF, BODY2)
        self.ASSERT_1_NONNULL()
        self.ASSERT_1_BODY_NONNULL()
        self.ASSERT_1_BODY2()
        self.DEC_REF_1()


class WellFormedPipelinedNoBodyHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_N(
            2,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CRLF,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CRLF,
        )
        self.ASSERT_N(2)
        self.ASSERT_N_METHOD(2)
        # ASSERT_N_URI(2)
        self.ASSERT_N_HTTP_VERSION(2)
        self.ASSERT_N_HOST_FIELD(2)
        self.ASSERT_N_BODY_NULL(2)
        self.DEC_REF_N(2)


class WellFormedPipelinedNormalBodyHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSE_N(
            2,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CL2_FIELD, CRLF, BODY2,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CL2_FIELD, CRLF, BODY2,
        )
        self.ASSERT_N(2)
        self.ASSERT_N_METHOD(2)
        # ASSERT_N_URI(2)
        self.ASSERT_N_HTTP_VERSION(2)
        self.ASSERT_N_HOST_FIELD(2)
        self.ASSERT_N_BODY_NONNULL(2)
        self.ASSERT_N_BODY2(2)
        self.DEC_REF_N(2)


class HTTPRequestParserTestSuite(HTTPMessageParserTestSuite):
    def __init__(self):
        HTTPMessageParserTestSuite.__init__(self)
        self.append(MalformedHTTPVersionMissingHTTPRequestParserTest())
        self.append(MalformedHTTPVersionMissingHTTPHTTPRequestParserTest())
        self.append(MalformedHTTPVersionMissingMinorVersionHTTPRequestParserTest())
        self.append(MalformedHTTPVersionMissingTrailingCRLFHTTPRequestParserTest())
        self.append(MalformedMethodMissingHTTPRequestParserTest())
        self.append(MalformedURIEmbeddedLFHTTPRequestParserTest())
        self.append(MalformedURIMissingHTTPRequestParserTest())
        self.append(WellFormedChunk1BodyHTTPRequestParserTest())
        self.append(WellFormedChunk2BodyHTTPRequestParserTest())
        self.append(WellFormedNoBodyHTTPRequestParserTest())
        self.append(WellFormedNoFieldsHTTPRequestParserTest())
        self.append(WellFormedNormalBodyHTTPRequestParserTest())
        self.append(WellFormedPipelinedNoBodyHTTPRequestParserTest())
        self.append(WellFormedPipelinedNormalBodyHTTPRequestParserTest())


if __name__ == "__main__":
    print HTTPRequestParserTestSuite()
