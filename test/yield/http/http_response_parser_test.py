# http_response_parser_test.py

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


# Constants
OK = "200 OK"


class HTTPResponseParserTest(HTTPMessageParserTest):
    def ASSERT_STATUS_CODE(self, status_code):
        self.append("ASSERT_EQ(http_response->get_status_code(), %(status_code)u);" % locals())


class MalformedReasonPhraseMissingHTTPResponseParserTest(HTTPResponseParserTest):
    def __init__(self):
        HTTPResponseParserTest.__init__(self)
        self.PARSER(HTTP_VERSION, " 200", CRLF, CRLF)
        self.PARSE()
        self.ASSERT_NONNULL()
        self.ASSERT_STATUS_CODE(400)
        self.DEC_REF()


class MalformedStatusCodeAlphaHTTPResponseParserTest(HTTPResponseParserTest):
    def __init__(self):
        HTTPResponseParserTest.__init__(self)
        self.PARSER(HTTP_VERSION, " XX OK", CRLF, CRLF)
        self.PARSE()
        self.ASSERT_NONNULL()
        self.ASSERT_STATUS_CODE(400)
        self.DEC_REF()


class MalformedStatusCodeMissingHTTPResponseParserTest(HTTPResponseParserTest):
    def __init__(self):
        HTTPResponseParserTest.__init__(self)
        self.PARSER(HTTP_VERSION, " OK", CRLF, CRLF)
        self.PARSE()
        self.ASSERT_NONNULL()
        self.ASSERT_STATUS_CODE(400)
        self.DEC_REF()


class MalformedStatusLineMissingHTTPResponseParserTest(HTTPResponseParserTest):
    def __init__(self):
        HTTPResponseParserTest.__init__(self)
        self.PARSER(HOST_FIELD, CRLF)
        self.PARSE()
        self.ASSERT_NONNULL()
        self.ASSERT_STATUS_CODE(400)
        self.DEC_REF()


class WellFormedStatusLineOnlyHTTPResponseParserTest(HTTPResponseParserTest):
    def __init__(self):
        HTTPResponseParserTest.__init__(self)
        self.PARSER(HTTP_VERSION, ' ', OK, CRLF, CRLF)
        self.PARSE()
        self.ASSERT_NONNULL()
        self.ASSERT_STATUS_CODE(200)
        self.ASSERT_BODY_NULL()
        self.DEC_REF()


class HTTPResponseParserTestSuite(HTTPMessageParserTestSuite):
    def __init__(self):
        HTTPMessageParserTestSuite.__init__(self)
        self.append(MalformedReasonPhraseMissingHTTPResponseParserTest())
        self.append(MalformedStatusCodeAlphaHTTPResponseParserTest())
        self.append(MalformedStatusCodeMissingHTTPResponseParserTest())
        self.append(MalformedStatusLineMissingHTTPResponseParserTest())
        self.append(WellFormedStatusLineOnlyHTTPResponseParserTest())


if __name__ == "__main__":
    print HTTPResponseParserTestSuite()
