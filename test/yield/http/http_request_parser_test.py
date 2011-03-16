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

from yuild.constant import INDENT_SPACES
from yutil import indent, write_file


# Constants
CRLF = "\\r\\n"
INDENT_SPACES = INDENT_SPACES["cpp"]
URI = "/"
VERSION = "HTTP/1.1"

# Fields
CL0_FIELD = "Content-Length: 0" + CRLF
CL2_FIELD = "Content-Length: 2" + CRLF
HOST = "localhost"
HOST_FIELD = "Host: localhost" + CRLF
TE_CHUNKED_FIELD = "Transfer-Encoding: chunked" + CRLF

# Bodies
BODY2 = "12"

# Assertions
ASSERT_1_BODY_NONNULL = "throw_assert_ne(http_request->get_body(), NULL);"
ASSERT_1_BODY_NULL = "throw_assert_eq(http_request->get_body(), NULL);"
ASSERT_1_BODY2 = "throw_assert_eq(http_request->get_content_length(), 2);"
ASSERT_1_HOST_FIELD = """throw_assert_eq((*http_request)["Host"], "%(HOST)s");""" % locals()
ASSERT_1_METHOD = """throw_assert_eq(http_request->get_method(), HTTPRequest::METHOD_GET);""" % locals()
ASSERT_1_NONNULL = "throw_assert_ne(http_request, NULL);"
ASSERT_1_NULL = "throw_assert_eq(http_request, NULL);"
ASSERT_1_URI = """throw_assert_eq(strcmp(http_request->get_uri(), "%(URI)s"), 0);""" % locals()
ASSERT_1_VERSION = "throw_assert_eq(http_request->get_http_version(), 1.1F);"

def ASSERT_N(n):
    return "throw_assert_eq(http_requests.size(), %(n)u);" % locals()
def ASSERT_N_BODY_NONNULL(n):
    return """for (size_t i = 0; i < %(n)u; i++) { throw_assert_ne(http_requests[i]->get_body(), NULL); }""" % locals()
def ASSERT_N_BODY_NULL(n):
    return """for (size_t i = 0; i < %(n)u; i++) { throw_assert_eq(http_requests[i]->get_body(), NULL); }""" % locals()
def ASSERT_N_BODY2(n):
    return "for (size_t i = 0; i < %(n)u; i++) { throw_assert_eq(http_requests[i]->get_content_length(), 2); }" % locals()
def ASSERT_N_METHOD(n):
    return """for (size_t i = 0; i < %(n)u; i++) { throw_assert_eq(http_requests[i]->get_method(), HTTPRequest::METHOD_GET); }""" % locals()
def ASSERT_N_URI(n):
    URI = globals()["URI"]
    return """for (size_t i = 0; i < %(n)u; i++) { throw_assert_eq(strcmp(http_requests[i]->get_uri(), "%(URI)s"), 0); }""" % locals()
def ASSERT_N_VERSION(n):
    return "for (size_t i = 0; i < %(n)u; i++) { throw_assert_eq(http_requests[i]->get_http_version(), 1.1F); }" % locals()
def ASSERT_N_HOST_FIELD(n):
    HOST = globals()["HOST"]
    return """for (size_t i = 0; i < %(n)u; i++) { throw_assert_eq((*http_requests[i])["Host"], "%(HOST)s"); }""" % locals()

# Read calls
def PARSE_1(*args):
    args = "".join(args)
    return """HTTPRequest* http_request = object_cast<HTTPRequest>(HTTPRequestParser(\"%(args)s\").parse());""" % locals()

def PARSE_N(n, *args):
    args = "".join(args)
    return """\
HTTPRequestParser http_request_parser(\"%(args)s\");
vector<HTTPRequest*> http_requests;

for (uint32_t i = 0; i < %(n)u; i++) {
  HTTPRequest* http_request = object_cast<HTTPRequest>(http_request_parser.parse());

  if (http_request != NULL)
    http_requests.push_back(http_request);
  else {
    while (!http_requests.empty()) {
      HTTPRequest::dec_ref(*http_requests.back());
      http_requests.pop_back();
    }

    throw_assert(false);
  }
}
""" % locals()

# dec_ref calls
DEC_REF_1 = "HTTPRequest::dec_ref(*http_request);"
def DEC_REF_N(n):
    return "for (size_t i = 0; i < %(n)u; i++) HTTPRequest::dec_ref(*http_requests[i]);" % locals()




# Test cases
test_cases = {}

test_cases["malformed_method_missing"] = \
(
    PARSE_1(URI, " HTTP/1.0", CRLF, HOST_FIELD, CRLF),
    ASSERT_1_NULL
)

test_cases["malformed_uri_embedded_lf"] = \
(
    PARSE_1("GET", " /\\r ", VERSION, CRLF, HOST_FIELD, CRLF),
    ASSERT_1_NULL
)

test_cases["malformed_uri_missing"] = \
(
    PARSE_1("GET", ' ', VERSION, CRLF, HOST_FIELD, CRLF),
    ASSERT_1_NULL
)

test_cases["malformed_version_missing"] = \
(
    PARSE_1("GET", ' ', URI, CRLF, HOST_FIELD, CRLF),
    ASSERT_1_NULL
)

test_cases["malformed_version_missing_http"] = \
(
    PARSE_1("GET", ' ', URI, ' ', "/1.0", CRLF, HOST_FIELD, CRLF),
    ASSERT_1_NULL
)

test_cases["malformed_version_missing_minor_version"] = \
(
    PARSE_1("GET", ' ', URI, " HTTP/1.", CRLF, HOST_FIELD, CRLF),
    ASSERT_1_NULL
)

test_cases["malformed_version_missing_trailing_crlf"] = \
(
    PARSE_1("GET", ' ', URI, ' ', VERSION, HOST_FIELD, CRLF),
    ASSERT_1_NULL
)

test_cases["well_formed_chunk_1_body"] = \
(
    PARSE_N
    (
        2,
        "GET", ' ', URI, ' ', VERSION, CRLF,
        HOST_FIELD,
        TE_CHUNKED_FIELD,
        CRLF,
        "1" + CRLF + "x" + CRLF + "0" + CRLF * 2
   ),
    ASSERT_N(2),
    ASSERT_N_METHOD(2),
    # ASSERT_N_URI(2),
    ASSERT_N_VERSION(2),
    ASSERT_N_BODY_NONNULL(2),
    "throw_assert_eq(http_requests[0]->get_content_length(), 1);",
    "throw_assert_eq(http_requests[1]->get_content_length(), 0);",
    DEC_REF_N(2)
)

test_cases["well_formed_chunk_2_body"] = \
(
    PARSE_N
    (
        3,
        "GET", ' ', URI, ' ', VERSION, CRLF,
        HOST_FIELD,
        TE_CHUNKED_FIELD,
        CRLF,
        "1" + CRLF + "x" + CRLF + "1" + CRLF + "y" + CRLF + "0" + CRLF * 2
   ),
    ASSERT_N(3),
    ASSERT_N_METHOD(3),
    # ASSERT_N_URI(3),
    ASSERT_N_VERSION(3),
    ASSERT_N_HOST_FIELD(3),
    ASSERT_N_BODY_NONNULL(3),
    "throw_assert_eq(http_requests[0]->get_content_length(), 1);",
    "throw_assert_eq(http_requests[1]->get_content_length(), 1);",
    "throw_assert_eq(http_requests[2]->get_content_length(), 0);",
    DEC_REF_N(3)
)

test_cases["well_formed_no_body"] = \
(
    PARSE_1("GET", ' ', URI, ' ', VERSION, CRLF, HOST_FIELD, CRLF),
    ASSERT_1_NONNULL,
    ASSERT_1_METHOD,
    ASSERT_1_VERSION,
    ASSERT_1_HOST_FIELD,
    ASSERT_1_BODY_NULL,
    DEC_REF_1
)

test_cases["well_formed_no_fields"] = \
(
    PARSE_1("GET", ' ', URI, ' ', VERSION, CRLF, CRLF),
    ASSERT_1_NONNULL,
    ASSERT_1_VERSION,
    ASSERT_1_BODY_NULL,
    DEC_REF_1
)

test_cases["well_formed_normal_body"] = \
(
    PARSE_1("GET", ' ', URI, ' ', VERSION, CRLF, HOST_FIELD, CL2_FIELD, CRLF, BODY2),
    ASSERT_1_NONNULL,
    ASSERT_1_BODY_NONNULL,
    ASSERT_1_BODY2,
    DEC_REF_1
)


test_cases["well_formed_pipelined_no_body"] = \
(
    PARSE_N
    (
        2,
        "GET", ' ', URI, ' ', VERSION, CRLF, HOST_FIELD, CRLF,
        "GET", ' ', URI, ' ', VERSION, CRLF, HOST_FIELD, CRLF,
   ),
    ASSERT_N(2),
    ASSERT_N_METHOD(2),
    # ASSERT_N_URI(2),
    ASSERT_N_VERSION(2),
    ASSERT_N_HOST_FIELD(2),
    ASSERT_N_BODY_NULL(2),
    DEC_REF_N(2)
)

test_cases["well_formed_pipelined_normal_body"] = \
(
    PARSE_N
    (
        2,
        "GET", ' ', URI, ' ', VERSION, CRLF, HOST_FIELD, CL2_FIELD, CRLF, BODY2,
        "GET", ' ', URI, ' ', VERSION, CRLF, HOST_FIELD, CL2_FIELD, CRLF, BODY2,
   ),
    ASSERT_N(2),
    ASSERT_N_METHOD(2),
    # ASSERT_N_URI(2),
    ASSERT_N_VERSION(2),
    ASSERT_N_HOST_FIELD(2),
    ASSERT_N_BODY_NONNULL(2),
    ASSERT_N_BODY2(2),
    DEC_REF_N(2)
)


test_case_names = test_cases.keys()
test_case_names.sort()
test_case_names.reverse()
for test_case_name in test_case_names:
    test_case = '\n'.join(test_cases[test_case_name])
    test_case = indent(INDENT_SPACES, test_case)
    test_cases[test_case_name] = """\
TEST(HTTPRequestParser, %(test_case_name)s) {
%(test_case)s
}
""" % locals()
test_cases = \
    '\n'.join(
        [test_cases[test_case_name]
        for test_case_name in test_case_names]
   )

write_file(
    "http_request_parser_test.cpp",
    """\
#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_request_parser.hpp"
#include "yunit.hpp"

TEST_SUITE(HTTPRequestParser);

namespace yield {
namespace http {
%(test_cases)s
}
}
""" % locals())
