# http_message_parser_test.py

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

from yuild.constant import INDENT_SPACES
from yutil import indent


__all__ = [
    "BODY2",
    "CRLF",
    "HOST_FIELD",
    "HTTP_VERSION",
    "HTTPMessageParserTest",
    "HTTPMessageParserTestSuite",
    "TE_CHUNKED_FIELD",
    "URI"
]


# Constants
BODY2 = "12"
CRLF = "\\r\\n"
CL0_FIELD = "Content-Length: 0" + CRLF
CL2_FIELD = "Content-Length: 2" + CRLF
HOST = "localhost"
HOST_FIELD = "Host: localhost" + CRLF
HTTP_VERSION = "HTTP/1.1"
INDENT_SPACES = INDENT_SPACES["cpp"]
TE_CHUNKED_FIELD = "Transfer-Encoding: chunked" + CRLF
URI = "/"


class HTTPMessageParserTest(list):
    def __init__(self):
        list.__init__(self)
        if self.__class__.__name__.endswith("HTTPMessageParserTest"):
            self.__http_message_type = "HTTPRequest"
        else:
            assert self.__class__.__name__.endswith("ParserTest")
            self.__http_message_type = self.__class__.__name__[self.__class__.__name__.rindex("HTTP"):self.__class__.__name__.index("ParserTest")]

    def ASSERT_1_BODY2(self):
        self.append("throw_assert_eq(%(http_message_type_lower_case)s->get_content_length(), 2);" % self)

    def ASSERT_1_BODY_NONNULL(self):
        self.append("throw_assert_ne(%(http_message_type_lower_case)s->get_body(), NULL);" % self)

    def ASSERT_1_BODY_NULL(self):
        self.append("throw_assert_eq(%(http_message_type_lower_case)s->get_body(), NULL);" % self)

    def ASSERT_1_HOST_FIELD(self):
        HOST = globals()["HOST"]
        http_message_type_lower_case = self.http_message_type_lower_case
        self.append("""throw_assert_eq((*%(http_message_type_lower_case)s)["Host"], "%(HOST)s");""" % locals())

    def ASSERT_1_HTTP_VERSION(self):
        self.append("throw_assert_eq(%(http_message_type_lower_case)s->get_http_version(), 1.1F);" % self)

    def ASSERT_1_NONNULL(self):
        self.append("throw_assert_ne(%(http_message_type_lower_case)s, NULL);" % self)

    def ASSERT_1_NULL(self):
        self.append("throw_assert_eq(%(http_message_type_lower_case)s, NULL);" % self)

    def ASSERT_N(self, n):
        http_message_type_lower_case = self.http_message_type_lower_case
        self.append("throw_assert_eq(%(http_message_type_lower_case)ss.size(), %(n)u);" % locals())

    def ASSERT_N_BODY2(self, n):
        http_message_type_lower_case = self.http_message_type_lower_case
        self.append("for (size_t i = 0; i < %(n)u; i++) { throw_assert_eq(%(http_message_type_lower_case)ss[i]->get_content_length(), 2); }" % locals())

    def ASSERT_N_BODY_NONNULL(self, n):
        http_message_type_lower_case = self.http_message_type_lower_case
        self.append("""for (size_t i = 0; i < %(n)u; i++) { throw_assert_ne(%(http_message_type_lower_case)ss[i]->get_body(), NULL); }""" % locals())

    def ASSERT_N_BODY_NULL(self, n):
        http_message_type_lower_case = self.http_message_type_lower_case
        self.append("""for (size_t i = 0; i < %(n)u; i++) { throw_assert_eq(%(http_message_type_lower_case)ss[i]->get_body(), NULL); }""" % locals())

    def ASSERT_N_HOST_FIELD(self, n):
        HOST = globals()["HOST"]
        http_message_type_lower_case = self.http_message_type_lower_case
        return """for (size_t i = 0; i < %(n)u; i++) { throw_assert_eq((*%(http_message_type_lower_case)ss[i])["Host"], "%(HOST)s"); }""" % locals()

    def ASSERT_N_HTTP_VERSION(self, n):
        http_message_type_lower_case = self.http_message_type_lower_case
        return "for (size_t i = 0; i < %(n)u; i++) { throw_assert_eq(%(http_message_type_lower_case)ss[i]->get_http_version(), 1.1F); }" % locals()

    def DEC_REF_1(self):
        self.append("%(http_message_type)s::dec_ref(*%(http_message_type_lower_case)s);" % self)

    def DEC_REF_N(self, n):
        http_message_type = self.http_message_type
        http_message_type_lower_case = self.http_message_type_lower_case
        self.append("for (size_t i = 0; i < %(n)u; i++) %(http_message_type)s::dec_ref(*%(http_message_type_lower_case)ss[i]);" % locals())

    def __getitem__(self, key):
        if key == "http_message_type":
            return self.http_message_type
        elif key == "http_message_type_lower_case":
            return self.http_message_type_lower_case
        else:
            raise KeyError, key

    @property
    def http_message_type(self):
        return self.__http_message_type

    @property
    def http_message_type_lower_case(self):
        return "http_" + self.__http_message_type[4:].lower()

    def PARSE_1(self, *args):
        args = "".join(args)
        http_message_type = self.http_message_type
        http_message_type_lower_case = self.http_message_type_lower_case
        self.append("""%(http_message_type)s* %(http_message_type_lower_case)s = object_cast<%(http_message_type)s>(%(http_message_type)sParser(\"%(args)s\").parse());""" % locals())

    def PARSE_N(self, n, *args):
        args = "".join(args)
        http_message_type = self.http_message_type
        http_message_type_lower_case = self.http_message_type_lower_case
        self.append("""\
%(http_message_type)sParser %(http_message_type_lower_case)s_parser(\"%(args)s\");
vector<%(http_message_type)s*> %(http_message_type_lower_case)ss;

for (uint32_t i = 0; i < %(n)u; i++) {
  %(http_message_type)s* %(http_message_type_lower_case)s = object_cast<%(http_message_type)s>(%(http_message_type_lower_case)s_parser.parse());

  if (%(http_message_type_lower_case)s != NULL)
    %(http_message_type_lower_case)ss.push_back(%(http_message_type_lower_case)s);
  else {
    while (!%(http_message_type_lower_case)ss.empty()) {
      %(http_message_type)s::dec_ref(*%(http_message_type_lower_case)ss.back());
      %(http_message_type_lower_case)ss.pop_back();
    }

    throw_assert(false);
  }
}
""" % locals())

    def __str__(self):
        if self.__class__.__name__.endswith("HTTPMessageParserTest"):
            http_message_parser_type = "HTTPMessageParser"
            test_name = self.__class__.__name__[:self.__class__.__name__.index("HTTPMessageParserTest")]
        else:
            http_message_parser_type = self.http_message_type + "Parser"
            test_name = self.__class__.__name__[:self.__class__.__name__.index(self.http_message_type + "ParserTest")]
        test = indent(INDENT_SPACES, '\n'.join(self))
        return """\
TEST(%(http_message_parser_type)s, %(test_name)s) {
%(test)s
}
""" % locals()


class WellFormedChunk1BodyHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSE_N(
            2,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF,
            HOST_FIELD,
            TE_CHUNKED_FIELD,
            CRLF,
            "1" + CRLF + "x" + CRLF + "0" + CRLF * 2
        )
        self.ASSERT_N(2)
        self.ASSERT_N_HTTP_VERSION(2)
        self.ASSERT_N_BODY_NONNULL(2)
        self.append("throw_assert_eq(http_requests[0]->get_content_length(), 1);")
        self.append("throw_assert_eq(http_requests[1]->get_content_length(), 0);")
        self.DEC_REF_N(2)


class WellFormedChunk2BodyHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSE_N(
            3,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF,
            HOST_FIELD,
            TE_CHUNKED_FIELD,
            CRLF,
            "1" + CRLF + "x" + CRLF + "1" + CRLF + "y" + CRLF + "0" + CRLF * 2
        )
        self.ASSERT_N(3)
        self.ASSERT_N_HTTP_VERSION(3)
        self.ASSERT_N_HOST_FIELD(3)
        self.ASSERT_N_BODY_NONNULL(3)
        self.append("throw_assert_eq(http_requests[0]->get_content_length(), 1);")
        self.append("throw_assert_eq(http_requests[1]->get_content_length(), 1);")
        self.append("throw_assert_eq(http_requests[2]->get_content_length(), 0);")
        self.DEC_REF_N(3)


class WellFormedNormalBodyHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSE_1("GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CL2_FIELD, CRLF, BODY2)
        self.ASSERT_1_NONNULL()
        self.ASSERT_1_BODY_NONNULL()
        self.ASSERT_1_BODY2()
        self.DEC_REF_1()


class WellFormedPipelinedNoBodyHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSE_N(
            2,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CRLF,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CRLF,
        )
        self.ASSERT_N(2)
        self.ASSERT_N_HTTP_VERSION(2)
        self.ASSERT_N_HOST_FIELD(2)
        self.ASSERT_N_BODY_NULL(2)
        self.DEC_REF_N(2)


class WellFormedPipelinedNormalBodyHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSE_N(
            2,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CL2_FIELD, CRLF, BODY2,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CL2_FIELD, CRLF, BODY2,
        )
        self.ASSERT_N(2)
        self.ASSERT_N_HTTP_VERSION(2)
        self.ASSERT_N_HOST_FIELD(2)
        self.ASSERT_N_BODY_NONNULL(2)
        self.ASSERT_N_BODY2(2)
        self.DEC_REF_N(2)


class HTTPMessageParserTestSuite(list):
    def __init__(self, *args):
        list.__init__(self, *args)
        if self.__class__ == HTTPMessageParserTestSuite:
            self.append(WellFormedChunk1BodyHTTPMessageParserTest())
            self.append(WellFormedChunk2BodyHTTPMessageParserTest())
            self.append(WellFormedNormalBodyHTTPMessageParserTest())
            self.append(WellFormedPipelinedNoBodyHTTPMessageParserTest())
            self.append(WellFormedPipelinedNormalBodyHTTPMessageParserTest())
        else:
            assert self.__class__.__name__.endswith("ParserTestSuite")

    def __str__(self):
        if self.__class__ == HTTPMessageParserTestSuite:
            http_message_type = "HTTPRequest"
            http_message_parser_type = "HTTPMessageParser"
        else:
            http_message_type = self.__class__.__name__[:self.__class__.__name__.index("ParserTestSuite")]
            http_message_parser_type = http_message_type + "Parser"
        http_message_type_lower_case = "http_" + http_message_type[4:].lower()
        tests = '\n'.join([str(test) for test in self])
        return """\
#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "yield/http/%(http_message_type_lower_case)s.hpp"
#include "yield/http/%(http_message_type_lower_case)s_parser.hpp"
#include "yunit.hpp"

TEST_SUITE(%(http_message_parser_type)s);

namespace yield {
namespace http {
%(tests)s
}
}
""" % locals()


if __name__ == "__main__":
    print HTTPMessageParserTestSuite()
