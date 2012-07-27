// yield/i18n/code.hpp

// Copyright (c) 2012 Minor Gordon
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

#ifndef _YIELD_I18N_CODE_HPP_
#define _YIELD_I18N_CODE_HPP_

#include "yield/types.hpp"

namespace yield {
namespace i18n {
/**
  iconv code constants. Similar to Win32 code pages. See iconv(3) for details.
*/
class Code {
public:
  /**
    The default platform multibyte character code.
  */
  const static Code CHAR;

  /**
    ISO-8859-1 code.
  */
  const static Code ISO88591;

  /**
    UTF-8 code.
  */
  const static Code UTF8;

#ifdef _WIN32
  /**
    Wide character (wchar_t) code.
  */
  const static Code WCHAR_T;
#endif

public:
#ifdef _WIN32
  /**
    Construct a Code from a Win32 code page constant.
    @param code_page Win32 code page constant
  */
  Code(uint32_t code_page)
    : code_page(code_page)
  { }
#else
  /**
    Construct a Code from an iconv(3) code name.
    @param iconv_code iconv(3) code name
  */
  Code(const char* iconv_code)
    : iconv_code(iconv_code)
  { }
#endif

public:
#ifdef _WIN32
  /**
    Get this Code as a Win32 code page constant.
    @return the Win32 code page constant equivalent of this Code
  */
  operator uint32_t() const {
    return code_page;
  }
#else
  /**
    Get this Code as an iconv(3) code name
    @return the iconv(3) code name equivalent of this Code
  */
  operator const char* () const {
    return iconv_code;
  }
#endif

private:
#ifdef _WIN32
  uint32_t code_page;
#else
  const char* iconv_code;
#endif
};
}
}

#endif
