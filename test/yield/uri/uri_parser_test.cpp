// uri_parser_test.cpp

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

#include "yield/assert.hpp"
#include "yield/uri/uri.hpp"
#include "yunit.hpp"

TEST_SUITE(URIParser);

namespace yield {
namespace uri {
TEST(URIParser, path1) {
  URI uri("/mydir/");
  throw_assert_eq(uri.get_path(), "/mydir/");
}

TEST(URIParser, path2) {
  URI uri("/mypath");
  throw_assert_eq(uri.get_path(), "/mypath");
}

TEST(URIParser, path3) {
  URI uri("/mydir/mypath");
  throw_assert_eq(uri.get_path(), "/mydir/mypath");
}

TEST(URIParser, path_query1) {
  URI uri("/mypath?key=value");
  throw_assert_eq(uri.get_path(), "/mypath");
  //throw_assert_eq( uri.get_query_value( "key" ), "value" );
  //throw_assert_eq( uri.get_query_value( "otherkey" ), "" );
  //throw_assert_ne( uri.get_query_values( "key" ), uri.get_query().end() );
}

TEST(URIParser, path_query2) {
  URI uri("/mypath?key1=value1&key2=value2");
  throw_assert_eq(uri.get_path(), "/mypath");
  //throw_assert_eq( uri.get_query_value( "key1" ), "value1" );
  //throw_assert_eq( uri.get_query_value( "key2" ), "value2" );
  //throw_assert_eq( uri.get_query_value( "otherkey" ), "" );
}

TEST(URIParser, path_query3) { // A Django URI 20110427
  URI uri("/accounts/login/?next=/");
  throw_assert_eq(uri.get_path(), "/accounts/login/");
  throw_assert_eq(uri.get_query(), "next=/");
}

TEST(URIParser, scheme_host1) {
  URI uri("http://localhost");
  throw_assert_eq(uri.get_scheme(), "http");
  throw_assert_eq(uri.get_host(), "localhost");
  //throw_assert( uri.get_user().empty() );
  //throw_assert( uri.get_password().empty() );
  throw_assert_eq(uri.get_port(), 0);
  throw_assert(uri.get_path().empty());
}

TEST(URIParser, scheme_host2) {
  URI uri("http://localhost/");
  throw_assert_eq(uri.get_scheme(), "http");
  throw_assert_eq(uri.get_host(), "localhost");
  throw_assert_eq(uri.get_path(), "/");
}

TEST(URIParser, scheme_host_port1) {
  URI uri("http://*:80/");
  throw_assert_eq(uri.get_scheme(), "http");
  throw_assert_eq(uri.get_host(), "*");
  throw_assert_eq(uri.get_path(), "/");
}

TEST(URIParser, scheme_host_port2) {
  URI uri("http://localhost:1");
  throw_assert_eq(uri.get_scheme(), "http");
  throw_assert_eq(uri.get_host(), "localhost");
  throw_assert_eq(uri.get_port(), 1);
  throw_assert(uri.get_path().empty());
}

TEST(URIParser, scheme_host_port3) {
  URI uri("http://localhost:1/");
  throw_assert_eq(uri.get_scheme(), "http");
  throw_assert_eq(uri.get_host(), "localhost");
  throw_assert_eq(uri.get_port(), 1);
  throw_assert_eq(uri.get_path(), "/");
}

TEST(URIParser, scheme_user_host) {
  URI uri("http://minorg@localhost");
  throw_assert_eq(uri.get_scheme(), "http");
  throw_assert_eq(uri.get_host(), "localhost");
  //throw_assert_eq( uri.get_user(), "minorg" );
  //throw_assert( uri.get_password().empty() );
}

TEST(URIParser, scheme_user_password_host) {
  URI uri("http://minorg:minorg@localhost");
  throw_assert_eq(uri.get_scheme(), "http");
  throw_assert_eq(uri.get_host(), "localhost");
  //throw_assert_eq( uri.get_user(), "minorg" );
  //throw_assert_eq( uri.get_password(), "minorg" );
}
}
}
