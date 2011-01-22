// yield/i18n/posix/iconv.hpp

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


#ifndef _YIELD_I18N_POSIX_ICONV_HPP_
#define _YIELD_I18N_POSIX_ICONV_HPP_


#include "yield/types.hpp"
#include "yield/i18n/code.hpp"


namespace yield {
namespace i18n {
namespace posix {
class iconv {
public:

public:
  iconv(Code tocode, Code fromcode);
  ~iconv();

  // iconv.3
  size_t
  operator()
  (
    const char** inbuf,
    size_t* inbytesleft,
    char** outbuf,
    size_t* outbytesleft
  );

  // Other operator()'s return false on failure
  bool operator()(const string& inbuf, string& outbuf);

private:
  size_t
  iconv_to_char
  (
    const char** inbuf,
    size_t* inbytesleft,
    char** outbuf,
    size_t* outbytesleft
  );

  template <class outbufStringType>
  bool iconv_to_string
  (
    const char* inbuf,
    size_t inbytesleft,
    outbufStringType& outbuf
  );

  bool reset();

private:
  void* cd;
};
}
}
}


#endif
