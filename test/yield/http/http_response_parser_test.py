# http_response_parser_test.py

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

from yutil.constant import INDENT_SPACES
from yutil.file import write_file
from yutil.string import indent


# Constants
CRLF = "\\r\\n"
INDENT_SPACES = INDENT_SPACES["cpp"]
OK = "200 OK"
URI = "/"
VERSION = "HTTP/1.1"

# Bodies
BODY2 = "12"

# Fields
CL0_FIELD = "Content-Length: 0" + CRLF
CL2_FIELD = "Content-Length: 2" + CRLF
DATE = "Fri, 31 Dec 1999 23:59:59 GMT"
DATE_FIELD = "Date: %(DATE)s" % locals() + CRLF

# Assertions
ASSERT_1_BODY_NONNULL = "throw_assert_ne( http_response->get_body(), NULL );"
ASSERT_1_BODY_NULL = "throw_assert_eq( http_response->get_body(), NULL );"
ASSERT_1_BODY2 = "throw_assert_eq( http_response->get_content_length(), 2 );"
ASSERT_1_DATE_FIELD = """throw_assert_eq( ( *http_response )["Date"], "%(DATE)s" );""" % locals()
ASSERT_1_NONNULL = "throw_assert_ne( http_response, NULL );"
ASSERT_1_NULL = "throw_assert_eq( http_response, NULL );"
def ASSERT_1_STATUS_CODE( status_code ):
    return "throw_assert_eq( http_response->get_status_code(), %(status_code)u );" % locals()

def ASSERT_N( n ):
    return "throw_assert_eq( http_responses.size(), %(n)u );" % locals()
def ASSERT_N_BODY_NONNULL( n ):
    return """for ( size_t i = 0; i < %(n)u; i++ ) { throw_assert_ne( http_responses[i]->get_body(), NULL ); }""" % locals()
def ASSERT_N_BODY_NULL( n ):
    return """for ( size_t i = 0; i < %(n)u; i++ ) { throw_assert_eq( http_responses[i]->get_body(), NULL ); }""" % locals()
def ASSERT_N_BODY2( n ):
    return "for ( size_t i = 0; i < %(n)u; i++ ) { throw_assert_eq( http_responses[i]->get_content_length(), 2 ); }" % locals()
def ASSERT_N_DATE_FIELD( n ):
    DATE = globals()["DATE"]
    return """for ( size_t i = 0; i < %(n)u; i++ ) { throw_assert_eq( ( *http_responses[i] )["Date"], "%(DATE)s" ); }""" % locals()
def ASSERT_N_STATUS_CODE( n, status_code ):
    return "for ( size_t i = 0; i < %(n)u; i++ ) { throw_assert_eq( http_responses[i]->get_status_code(), %(status_code)u ); }" % locals()

# Read calls
def PARSE_1( *args ):
    args = "".join( args )
    return """HTTPResponse* http_response = object_cast<HTTPResponse>( HTTPResponseParser( \"%(args)s\" ).parse() );""" % locals()

def PARSE_N( n, *args ):
    args = "".join( args )
    return """\
HTTPResponseParser http_response_parser( \"%(args)s\" );
vector<HTTPResponse*> http_responses;

for ( uint32_t i = 0; i < %(n)u; i++ )
{
  HTTPResponse* http_response = object_cast<HTTPResponse>( http_response_parser.parse() );

  if ( http_response != NULL )
    http_responses.push_back( http_response );
  else
  {
    while ( !http_responses.empty() )
    {
      HTTPResponse::dec_ref( *http_responses.back() );
      http_responses.pop_back();
    }

    throw_assert( false );
  }
}
""" % locals()


# dec_ref calls
DEC_REF_1 = "HTTPResponse::dec_ref( *http_response );"
def DEC_REF_N( n ):
    return "for ( size_t i = 0; i < %(n)u; i++ ) HTTPResponse::dec_ref( *http_responses[i] );" % locals()




# Test cases
test_cases = {}

test_cases["malformed_reason_phrase_missing"] = \
(
    PARSE_1( VERSION, " 200", CRLF, DATE_FIELD, CRLF ),
    ASSERT_1_NULL
)

test_cases["malformed_status_code_alpha"] = \
(
    PARSE_1( VERSION, " XX OK", CRLF, DATE_FIELD, CRLF ),
    ASSERT_1_NULL
)

test_cases["malformed_status_code_missing"] = \
(
    PARSE_1( VERSION, " OK", CRLF, DATE_FIELD, CRLF ),
    ASSERT_1_NULL
)

test_cases["malformed_status_line_missing"] = \
(
    PARSE_1( DATE_FIELD, CRLF ),
    ASSERT_1_NULL
)

test_cases["well_formed_no_body"] = \
(
    PARSE_1( VERSION, ' ', OK, CRLF, DATE_FIELD, CRLF ),
    ASSERT_1_NONNULL,
    ASSERT_1_STATUS_CODE( 200 ),
    ASSERT_1_DATE_FIELD,
    ASSERT_1_BODY_NULL,
    DEC_REF_1
)

test_cases["well_formed_normal_body"] = \
(
    PARSE_1( VERSION, ' ', OK, CRLF, DATE_FIELD, CL2_FIELD, CRLF, BODY2 ),
    ASSERT_1_NONNULL,
    ASSERT_1_STATUS_CODE( 200 ),
    ASSERT_1_DATE_FIELD,
    ASSERT_1_BODY_NONNULL,
    ASSERT_1_BODY2,
    DEC_REF_1
)

test_cases["well_formed_pipelined_no_body"] = \
(
    PARSE_N
    (
        2,
        VERSION, ' ', OK, CRLF, DATE_FIELD, CRLF,
        VERSION, ' ', OK, CRLF, DATE_FIELD, CRLF,
    ),
    ASSERT_N( 2 ),
    ASSERT_N_STATUS_CODE( 2, 200 ),
    ASSERT_N_DATE_FIELD( 2 ),
    ASSERT_N_BODY_NULL( 2 ),
    DEC_REF_N( 2 )
)

test_cases["well_formed_pipelined_normal_body"] = \
(
    PARSE_N
    (
        2,
        VERSION, ' ', OK, CRLF, DATE_FIELD, CL2_FIELD, CRLF, BODY2,
        VERSION, ' ', OK, CRLF, DATE_FIELD, CL2_FIELD, CRLF, BODY2,
    ),
    ASSERT_N( 2 ),
    ASSERT_N_STATUS_CODE( 2, 200 ),
    ASSERT_N_DATE_FIELD( 2 ),
    ASSERT_N_BODY_NONNULL( 2 ),
    ASSERT_N_BODY2( 2 ),
    DEC_REF_N( 2 )
)

test_cases["well_formed_reason_phrase_split"] = \
(
    PARSE_1( VERSION, ' ', "415 Unsupported Media Type", CRLF, DATE_FIELD, CRLF ),
    ASSERT_1_NONNULL,
    ASSERT_1_STATUS_CODE( 415 ),
    ASSERT_1_DATE_FIELD,
    ASSERT_1_BODY_NULL,
    DEC_REF_1
)


test_case_names = test_cases.keys()
test_case_names.sort()
test_case_names.reverse()
for test_case_name in test_case_names:
    test_case = '\n'.join( test_cases[test_case_name] )
    test_case = indent( INDENT_SPACES, test_case )
    test_cases[test_case_name] = """\
TEST( HTTPResponseParser, %(test_case_name)s )
{
%(test_case)s
}
""" % locals()
test_cases =\
    '\n'.join(
        [test_cases[test_case_name]
        for test_case_name in test_case_names]
    )
test_cases = indent( INDENT_SPACES * 2, test_cases )


write_file(
    "http_response_parser_test.cpp",
    """\
#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/buffer.hpp"
#include "yield/http/http_response.hpp"
#include "yield/http/http_response_parser.hpp"
#include "yunit.hpp"


TEST_SUITE( HTTPResponseParser );

namespace yield
{
  namespace http
  {
%(test_cases)s
  }
}
""" % locals() )
