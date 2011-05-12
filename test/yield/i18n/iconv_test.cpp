// iconv_test.cpp

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
#include "yield/auto_object.hpp"
#include "yield/exception.hpp"
#include "yield/i18n/iconv.hpp"
#include "yunit.hpp"


#define TEST_STRING_ISO88591 "\304rger mit b\366sen B\374bchen ohne Augenma\337"
#define TEST_STRING_UTF8\
  "\303\204rger mit b\303\266sen B\303\274bchen ohne Augenma\303\237"
#ifdef _WIN32
#define TEST_STRING_WIDE L"test string"
#endif


TEST_SUITE(iconv);

namespace yield {
namespace i18n {
TEST(iconv, c_strings) {
  const char* inbuf = TEST_STRING_ISO88591;
  size_t inbytesleft = strlen(TEST_STRING_ISO88591);
  char outbuf[256] = { 0 };
  size_t outbytesleft = 256;
  char* outbuf_p = outbuf;

  size_t iconv_ret
  = iconv(Code::UTF8, Code::ISO88591)
    (
      &inbuf,
      &inbytesleft,
      &outbuf_p,
      &outbytesleft
    );

  if (iconv_ret == static_cast<size_t>(-1))
    throw Exception();

  // throw_assert_ne(inbuf, TEST_STRING_ISO88591);
  throw_assert_eq(inbytesleft, 0);
  throw_assert_ne(outbuf_p, outbuf);
  throw_assert_gt(strlen(outbuf), 0);
  throw_assert_lt(outbytesleft, 256);
}

TEST(iconv, iso88591_to_utf8) {
  string outbuf;
  if (!iconv(Code::UTF8, Code::ISO88591)(TEST_STRING_ISO88591, outbuf))
    throw Exception();
  throw_assert_false(outbuf.empty());
  throw_assert_eq(outbuf, TEST_STRING_UTF8);
}

TEST(iconv, utf8_to_iso88591) {
  string outbuf;
  if (!iconv(Code::ISO88591, Code::UTF8)(TEST_STRING_UTF8, outbuf))
    throw Exception();
  throw_assert_false(outbuf.empty());
  throw_assert_eq(outbuf, TEST_STRING_ISO88591);
}

#ifdef _WIN32
TEST(iconv, utf8_to_wide) {
  std::wstring outbuf;
  if (!iconv(Code::WCHAR_T, Code::UTF8)(TEST_STRING_UTF8, outbuf))
    throw Exception();
  throw_assert_false(outbuf.empty());
}

TEST(iconv, wide_to_utf8) {
  string outbuf;
  if (!iconv(Code::UTF8, Code::WCHAR_T)(TEST_STRING_WIDE, outbuf))
    throw Exception();
  throw_assert_eq(outbuf.size(), wcslen(TEST_STRING_WIDE));
}
#endif
}
}
