// yield/i18n/tstring.hpp

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

#ifndef _YIELD_I18N_TSTRING_HPP_
#define _YIELD_I18N_TSTRING_HPP_

#include "yield/types.hpp"
#include "yield/i18n/code.hpp"

namespace yield {
namespace i18n {
/**
  Platform-specific internationalized string: a wide string on Win32,
    a multibyte string with the default character set on POSIX systems.
*/
#ifdef _WIN32
class tstring : public std::wstring {
#else
class tstring : public std::string {
#endif
public:
  /**
    Construct an empty tstring.
  */
  tstring() { }

  /**
    Construct a tstring from a single character.
    @param s new contents of the tstring
    @param code iconv code of the tstring
  */
  tstring(char s, Code code = Code::CHAR);

  /**
    Construct a tstring from a C string.
    @param s new contents of the tstring, copied
    @param code iconv code of the tstring
  */
  tstring(const char* s, Code code = Code::CHAR);

  /**
    Construct a tstring from a C++ string.
    @param s new contents of the tstring, copied
    @param code iconv code of the tstring
  */
  tstring(const string& s, Code code = Code::CHAR);

  /**
    Construct a tstring from a string and length.
    @param s new contents of the tstring, copied
    @param len length of s
    @param code iconv code of the tstring
  */
  tstring(const char* s, size_t len, Code code = Code::CHAR);

#ifdef _WIN32
  /**
    Construct a tstring from a single wide character.
    @param s new contents of the tstring
  */
  tstring(wchar_t s);

  /**
    Construct a tstring from a wide C string.
    @param s new contents of the tstring, copied
  */
  tstring(const wchar_t* s);

  /**
    Construct a tstring from a wide string and length.
    @param s new contents of the tstring, copied
    @param len length of s
  */
  tstring(const wchar_t* s, size_t len);

  /**
    Construct a tstring from a C++ wstring.
    @param s new contents of the tstring, copied
  */
  tstring(const std::wstring& s);
#endif

  /**
    Encode a tstring as a multibyte C++ string.
    @param tocode the iconv code with which to encode the tstring
    @return the encoded tstring as a multibyte C++ string
  */
  string encode(Code tocode) const;

private:
  void init(const char*, size_t, Code);
};
}
}

#endif
