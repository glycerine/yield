// yield/i18n/posix/iconv.cpp

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

#include "yield/config.hpp"
#include <iconv.h>
#ifdef __sun
#undef iconv
#endif

#include "yield/assert.hpp"
#include "yield/exception.hpp"
#include "yield/i18n/posix/iconv.hpp"

#include <errno.h>


namespace yield {
namespace i18n {
namespace posix {
iconv::iconv(Code tocode, Code fromcode) {
  cd = ::iconv_open(tocode, fromcode);
  if (cd == reinterpret_cast<iconv_t>(-1))
    throw Exception();
}

iconv::~iconv() {
  iconv_close(cd);
}

size_t
iconv::iconv_to_char
(
  const char** inbuf,
  size_t* inbytesleft,
  char** outbuf,
  size_t* outbytesleft
) {
#ifdef __sun
  return ::libiconv
#else
  return ::iconv
#endif
         (
           cd,
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__sun)
           inbuf,
#else
           const_cast<char**>(inbuf),
#endif
           inbytesleft,
           outbuf,
           outbytesleft
         );
}

template <class outbufStringType>
bool
iconv::iconv_to_string
(
  const char* inbuf,
  size_t inbytesleft,
  outbufStringType& outbuf
) {
  size_t outbuf_c_len = inbytesleft;

  for (;;) {
    typename outbufStringType::value_type* outbuf_c
    = new typename outbufStringType::value_type[outbuf_c_len];
    void* outbuf_c_dummy = outbuf_c;
    size_t outbytesleft = outbuf_c_len;

    size_t iconv_ret
    = iconv_to_char
      (
        &inbuf,
        &inbytesleft,
        reinterpret_cast<char**>(&outbuf_c_dummy),
        &outbytesleft
      );

    if (iconv_ret != static_cast<size_t>(-1)) {
      outbuf.append(outbuf_c, outbuf_c_len - outbytesleft);
      delete [] outbuf_c;
      return true;
    } else if (errno == E2BIG) {
      outbuf.append(outbuf_c, outbuf_c_len - outbytesleft);
      delete [] outbuf_c;
      outbuf_c_len *= 2;
      continue;
    } else {
      delete [] outbuf_c;
      return false;
    }
  }
}

size_t
iconv::operator()
(
  const char** inbuf,
  size_t* inbytesleft,
  char** outbuf,
  size_t* outbytesleft
) {
  if (reset())
    return iconv_to_char(inbuf, inbytesleft, outbuf, outbytesleft);
  else
    return static_cast<size_t>(-1);
}

bool iconv::operator()(const string& inbuf, string& outbuf) {
  if (reset())
    return iconv_to_string(inbuf.data(), inbuf.size(), outbuf);
  else
    return false;
}

bool iconv::reset() {
  return iconv_to_char(NULL, 0, NULL, 0) != static_cast<size_t>(-1);
}
}
}
}
