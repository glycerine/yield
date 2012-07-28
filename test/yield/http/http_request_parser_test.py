# http_request_parser_test.py

# Copyright (c) 2012 Minor Gordon
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


class HTTPRequestParserTest(HTTPMessageParserTest):
    def ASSERT_METHOD(self, method="GET"):
        self.append("""ASSERT_EQ(http_request->get_method(), HTTPRequest::Method::%(method)s);""" % locals())

    def ASSERT_URI(self):
        self.append("""ASSERT_EQ(strcmp(http_request->get_uri(), "%(URI)s"), 0);""" % globals())


class MalformedHTTPVersionMissingHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSER("GET", ' ', URI, CRLF, HOST_FIELD, CRLF)
        self.PARSE()
        self.ASSERT_NULL()


class MalformedHTTPVersionMissingHTTPHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSER("GET", ' ', URI, ' ', "/1.0", CRLF, HOST_FIELD, CRLF)
        self.PARSE()
        self.ASSERT_NULL()


class MalformedHTTPVersionMissingMinorVersionHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSER("GET", ' ', URI, " HTTP/1.", CRLF, HOST_FIELD, CRLF)
        self.PARSE()
        self.ASSERT_NULL()


class MalformedHTTPVersionMissingTrailingCRLFHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSER("GET", ' ', URI, ' ', HTTP_VERSION, HOST_FIELD, CRLF)
        self.PARSE()
        self.ASSERT_NULL()


class MalformedMethodMissingHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSER(URI, " HTTP/1.0", CRLF, HOST_FIELD, CRLF)
        self.PARSE()
        self.ASSERT_NULL()


class MalformedURIEmbeddedLFHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSER("GET", " /\\r ", HTTP_VERSION, CRLF, HOST_FIELD, CRLF)
        self.PARSE()
        self.ASSERT_NULL()


class MalformedURIMissingHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSER("GET", ' ', HTTP_VERSION, CRLF, HOST_FIELD, CRLF)
        self.PARSE()
        self.ASSERT_NULL()


class WellFormedRequestLineOnlyHTTPRequestParserTest(HTTPRequestParserTest):
    def __init__(self):
        HTTPRequestParserTest.__init__(self)
        self.PARSER("GET", ' ', URI, ' ', HTTP_VERSION, CRLF, CRLF)
        self.PARSE()
        self.ASSERT_NONNULL()
        self.ASSERT_METHOD()
        self.ASSERT_HTTP_VERSION()
        self.ASSERT_BODY_NULL()
        self.DEC_REF()


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
        self.append(WellFormedRequestLineOnlyHTTPRequestParserTest())


if __name__ == "__main__":
    print HTTPRequestParserTestSuite()
