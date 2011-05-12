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
from yutil import decamel, indent


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

    def ASSERT_BODY2(self, http_message_instance=None):
        if http_message_instance is None:
            http_message_instance = decamel(self.http_message_type)

        self.append("throw_assert_eq(static_cast<Buffer*>(%(http_message_instance)s->get_body())->size(), 2);" % locals())

    def ASSERT_BODY_NONNULL(self, http_message_instance=None):
        if http_message_instance is None:
            http_message_instance = decamel(self.http_message_type)

        self.append("throw_assert_ne(%(http_message_instance)s->get_body(), NULL);" % locals())

    def ASSERT_BODY_NULL(self, http_message_instance=None):
        if http_message_instance is None:
            http_message_instance = decamel(self.http_message_type)

        self.append("throw_assert_eq(%(http_message_instance)s->get_body(), NULL);" % locals())

    def ASSERT_HOST_FIELD(self, http_message_instance=None):
        if http_message_instance is None:
            http_message_instance = decamel(self.http_message_type)

        HOST = globals()["HOST"]
        self.append("""throw_assert_eq((*%(http_message_instance)s)["Host"], "%(HOST)s");""" % locals())

    def ASSERT_HTTP_VERSION(self, http_message_instance=None, http_version="1"):
        if http_message_instance is None:
            http_message_instance = decamel(self.http_message_type)

        self.append("throw_assert_eq(%(http_message_instance)s->get_http_version(), %(http_version)s);" % locals())

    def ASSERT_NONNULL(self, object_instance=None):
        if object_instance is None:
            object_instance = decamel(self.http_message_type)

        self.append("throw_assert_ne(%(object_instance)s, NULL);" % locals())

    def ASSERT_NULL(self, object_instance=None):
        if object_instance is None:
            object_instance = decamel(self.http_message_type)

        self.append("throw_assert_eq(%(object_instance)s, NULL);" % locals())

    def DEC_REF(self, object_type=None, object_instance=None):
        if object_type is None:
            object_type = self.http_message_type

        if object_instance is None:
            object_instance = decamel(object_type)

        self.append("%(object_type)s::dec_ref(%(object_instance)s);" % locals())

    @property
    def http_message_type(self):
        return self.__http_message_type

    def PARSE(self, object_type=None, object_instance=None):
        if object_type is None:
            object_type = self.http_message_type

        if object_instance is None:
            object_instance = decamel(object_type)

        http_message_type_lower = decamel(self.http_message_type)

        self.append("""%(object_type)s* %(object_instance)s = object_cast<%(object_type)s>(%(http_message_type_lower)s_parser.parse());""" % locals())

    def PARSER(self, *args):
        args = "".join(args)
        http_message_type = self.http_message_type
        http_message_type_lower_case = decamel(self.http_message_type)
        self.append("""%(http_message_type)sParser %(http_message_type_lower_case)s_parser(\"%(args)s\");""" % locals())

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


class MalformedFieldMissingColonHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSER(
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF,
            "Host", CRLF,
            CRLF
        )
        self.PARSE()
        self.ASSERT_NULL()


class MalformedFieldMissingNameHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSER(
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF,
            ": localhost", CRLF,
            CRLF
        )
        self.PARSE()
        self.ASSERT_NULL()


class WellFormed1ChunkBodyHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSER(
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF,
            HOST_FIELD,
            TE_CHUNKED_FIELD,
            CRLF,
            "1" + CRLF + "x" + CRLF,
            "0" + CRLF,
            CRLF
        )
        self.PARSE()
        self.ASSERT_NONNULL()
        self.ASSERT_HTTP_VERSION()
        self.ASSERT_HOST_FIELD()
        for http_message_body_chunk_size in (1, 0):
            self.append('{')
            self.PARSE("HTTPMessageBodyChunk", "http_message_body_chunk")
            self.ASSERT_NONNULL("http_message_body_chunk")
            self.append("throw_assert_eq(http_message_body_chunk->size(), %(http_message_body_chunk_size)u);" % locals())
            self.DEC_REF("HTTPMessageBodyChunk", "http_message_body_chunk")
            self.append('}')
        self.DEC_REF()


class WellFormed2ChunkBodyHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSER(
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF,
            HOST_FIELD,
            TE_CHUNKED_FIELD,
            CRLF,
            "1" + CRLF + "x" + CRLF,
            "1" + CRLF + "y" + CRLF,
            "0" + CRLF,
            CRLF
        )
        self.PARSE()
        self.ASSERT_NONNULL()
        self.ASSERT_HTTP_VERSION()
        self.ASSERT_HOST_FIELD()
        for http_message_body_chunk_size in (1, 1, 0):
            self.append('{')
            self.PARSE("HTTPMessageBodyChunk", "http_message_body_chunk")
            self.ASSERT_NONNULL("http_message_body_chunk")
            self.append("throw_assert_eq(http_message_body_chunk->size(), %(http_message_body_chunk_size)u);" % locals())
            self.DEC_REF("HTTPMessageBodyChunk", "http_message_body_chunk")
            self.append('}')
        self.DEC_REF()


class WellFormedChunkedBodyWithChunkExtensionHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSER(
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF,
            TE_CHUNKED_FIELD,
            CRLF,
            "1",
            ";chunk_ext1",
            ";chunk_ext2=\\\"ChunkExtension\\\"",
            CRLF,
            "x",
            CRLF,
            "0" + CRLF,
            CRLF
        )
        self.PARSE()
        self.ASSERT_NONNULL()
        self.ASSERT_HTTP_VERSION()
        for http_message_body_chunk_size in (1, 0):
            self.append('{')
            self.PARSE("HTTPMessageBodyChunk", "http_message_body_chunk")
            self.ASSERT_NONNULL("http_message_body_chunk")
            self.append("throw_assert_eq(http_message_body_chunk->size(), %(http_message_body_chunk_size)u);" % locals())
            self.DEC_REF("HTTPMessageBodyChunk", "http_message_body_chunk")
            self.append('}')
        self.DEC_REF()


class WellFormedChunkedBodyWithTrailerHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSER(
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF,
            TE_CHUNKED_FIELD,
            CRLF,
            "1" + CRLF + "x" + CRLF,
            "1" + CRLF + "y" + CRLF,
            "0" + CRLF,
            HOST_FIELD,
            "X-Host: x-localhost" + CRLF,
            CRLF
        )
        self.PARSE()
        self.ASSERT_NONNULL()
        self.ASSERT_HTTP_VERSION()
        for http_message_body_chunk_size in (1, 1, 0):
            self.append('{')
            self.PARSE("HTTPMessageBodyChunk", "http_message_body_chunk")
            self.ASSERT_NONNULL("http_message_body_chunk")
            self.append("throw_assert_eq(http_message_body_chunk->size(), %(http_message_body_chunk_size)u);" % locals())
            self.DEC_REF("HTTPMessageBodyChunk", "http_message_body_chunk")
            self.append('}')
        self.DEC_REF()



class WellFormedFieldMissingValueHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSER(
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF,
            "Host:", CRLF,
            CRLF
        )
        self.PARSE()
        self.ASSERT_NONNULL()
        self.ASSERT_HTTP_VERSION()
        http_message_instance = decamel(self.http_message_type)
        self.append("""throw_assert_eq((*%(http_message_instance)s)["Host"], "");""" % locals())
        self.DEC_REF()


class WellFormedNoBodyHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSER("GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CRLF)
        self.PARSE()
        self.ASSERT_NONNULL()
        self.ASSERT_BODY_NULL()
        self.DEC_REF()


class WellFormedNormalBodyHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSER("GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CL2_FIELD, CRLF, BODY2)
        self.PARSE()
        self.ASSERT_NONNULL()
        self.ASSERT_BODY_NONNULL()
        self.ASSERT_BODY2()
        self.DEC_REF()


class WellFormedPipelinedNoBodyHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSER(
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CRLF,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CRLF,
        )
        for i in xrange(2):
            self.append('{')
            self.PARSE()
            self.ASSERT_NONNULL()
            self.ASSERT_HTTP_VERSION()
            self.ASSERT_HOST_FIELD()
            self.ASSERT_BODY_NULL()
            self.DEC_REF()
            self.append('}')


class WellFormedPipelinedNormalBodyHTTPMessageParserTest(HTTPMessageParserTest):
    def __init__(self):
        HTTPMessageParserTest.__init__(self)
        self.PARSER(
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CL2_FIELD, CRLF, BODY2,
            "GET", ' ', URI, ' ', HTTP_VERSION, CRLF, HOST_FIELD, CL2_FIELD, CRLF, BODY2,
        )
        for i in xrange(2):
            self.append('{')
            self.PARSE()
            self.ASSERT_NONNULL()
            self.ASSERT_HTTP_VERSION()
            self.ASSERT_HOST_FIELD()
            self.ASSERT_BODY_NONNULL()
            self.ASSERT_BODY2()
            self.DEC_REF()
            self.append('}')


class HTTPMessageParserTestSuite(list):
    def __init__(self, *args):
        list.__init__(self, *args)
        if self.__class__ == HTTPMessageParserTestSuite:
            self.append(MalformedFieldMissingColonHTTPMessageParserTest())
            self.append(MalformedFieldMissingNameHTTPMessageParserTest())
            self.append(WellFormedChunkedBodyWithChunkExtensionHTTPMessageParserTest())
            self.append(WellFormedChunkedBodyWithTrailerHTTPMessageParserTest())
            self.append(WellFormed1ChunkBodyHTTPMessageParserTest())
            self.append(WellFormed2ChunkBodyHTTPMessageParserTest())
            self.append(WellFormedFieldMissingValueHTTPMessageParserTest())
            self.append(WellFormedNoBodyHTTPMessageParserTest())
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
#include "yield/http/http_message_body_chunk.hpp"
#include "yield/http/%(http_message_type_lower_case)s.hpp"
#include "yield/http/%(http_message_type_lower_case)s_parser.hpp"
#include "yunit.hpp"

TEST_SUITE(%(http_message_parser_type)s);

namespace yield {
namespace http {
%(tests)s}
}
""" % locals()


if __name__ == "__main__":
    print HTTPMessageParserTestSuite()
