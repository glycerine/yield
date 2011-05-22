// yield/i18n/win32/iconv.cpp

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

#include "yield/i18n/iconv.hpp"

#include <Windows.h>

namespace yield {
namespace i18n {
iconv::iconv(Code tocode, Code fromcode)
  : fromcode(fromcode), tocode(tocode)
{ }

iconv::~iconv() {
}

size_t
iconv::operator()(
  const char** inbuf,
  size_t* inbytesleft,
  char** outbuf,
  size_t* outbytesleft
) {
  int inbuf_w_len
  = MultiByteToWideChar(
      fromcode,
      0,
      *inbuf,
      static_cast<int>(*inbytesleft),
      NULL,
      0
    );

  if (inbuf_w_len > 0) {
    wchar_t* inbuf_w = new wchar_t[inbuf_w_len];

    inbuf_w_len
    = MultiByteToWideChar(
        fromcode,
        0,
        *inbuf,
        static_cast<int>(*inbytesleft),
        inbuf_w,
        inbuf_w_len
      );

    if (inbuf_w_len > 0) {
      int outbyteswritten
      = WideCharToMultiByte(
          tocode,
          0,
          inbuf_w,
          inbuf_w_len,
          *outbuf,
          *outbytesleft,
          0,
          0
        );

      delete [] inbuf_w;

      if (outbyteswritten > 0) {
        *inbuf += *inbytesleft;
        *inbytesleft = 0;
        *outbuf += outbyteswritten;
        *outbytesleft -= outbyteswritten;
        return outbyteswritten;
      }
    } else
      delete [] inbuf_w;
  }

  return static_cast<size_t>(-1);
}

bool iconv::operator()(const string& inbuf, string& outbuf) {
  int inbuf_w_len
  = MultiByteToWideChar(
      fromcode,
      0,
      inbuf.c_str(),
      inbuf.size(),
      NULL,
      0
    );

  if (inbuf_w_len > 0) {
    wchar_t* inbuf_w = new wchar_t[inbuf_w_len];

    inbuf_w_len
    = MultiByteToWideChar(
        fromcode,
        0,
        inbuf.c_str(),
        inbuf.size(),
        inbuf_w,
        inbuf_w_len
      );

    if (inbuf_w_len > 0) {
      int outbuf_c_len
      = WideCharToMultiByte(
          tocode,
          0,
          inbuf_w,
          inbuf_w_len,
          NULL,
          0,
          0,
          0
        );

      if (outbuf_c_len > 0) {
        char* outbuf_c = new char[outbuf_c_len];

        outbuf_c_len
        = WideCharToMultiByte(
            tocode,
            0,
            inbuf_w,
            inbuf_w_len,
            outbuf_c,
            outbuf_c_len,
            0,
            0
          );

        if (outbuf_c_len > 0) {
          outbuf.append(outbuf_c, outbuf_c_len);
          delete [] outbuf_c;
          return true;
        } else
          delete [] outbuf_c;
      } else
        delete [] inbuf_w;
    } else
      delete [] inbuf_w;
  }

  return false;
}

bool iconv::operator()(const string& inbuf, std::wstring& outbuf) {
  int outbuf_w_len
  = MultiByteToWideChar(
      fromcode,
      0,
      inbuf.c_str(),
      inbuf.size(),
      NULL,
      0
    );

  if (outbuf_w_len > 0) {
    wchar_t* outbuf_w = new wchar_t[outbuf_w_len];

    outbuf_w_len
    = MultiByteToWideChar(
        fromcode,
        0,
        inbuf.c_str(),
        inbuf.size(),
        outbuf_w,
        outbuf_w_len
      );

    if (outbuf_w_len > 0) {
      outbuf.append(outbuf_w, outbuf_w_len);
      delete [] outbuf_w;
      return true;
    } else
      delete [] outbuf_w;
  }

  return false;
}

bool iconv::operator()(const std::wstring& inbuf, string& outbuf) {
  int outbuf_c_len
  = WideCharToMultiByte(
      tocode,
      0,
      inbuf.c_str(),
      inbuf.size(),
      NULL,
      0,
      0,
      0
    );

  if (outbuf_c_len > 0) {
    char* outbuf_c = new char[outbuf_c_len];

    outbuf_c_len
    = WideCharToMultiByte(
        tocode,
        0,
        inbuf.c_str(),
        inbuf.size(),
        outbuf_c,
        outbuf_c_len,
        0,
        0
      );

    if (outbuf_c_len > 0) {
      outbuf.append(outbuf_c, outbuf_c_len);
      delete [] outbuf_c;
      return true;
    } else
      delete [] outbuf_c;
  }

  return false;
}
}
}
