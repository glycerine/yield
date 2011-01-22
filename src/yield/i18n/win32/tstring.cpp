// yield/i18n/win32/tstring.cpp

// Copyright (c) 2010 Minor Gordon
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


#include "yield/i18n/win32/iconv.hpp"
#include "yield/i18n/win32/tstring.hpp"

#include <Windows.h>


namespace yield {
namespace i18n {
namespace win32 {
tstring::tstring( char s, Code code ) {
  init( &s, 1, code );
}

tstring::tstring( const char* s, Code code ) {
  init( s, strlen( s ), code );
}

tstring::tstring( const string& s, Code code ) {
  init( s.data(), s.size(), code );
}

tstring::tstring( const char* s, size_t len, Code code ) {
  init( s, len, code );
}

tstring::tstring( wchar_t s )
  : wstring( &s, 1 )
{ }

tstring::tstring( const wchar_t* s )
  : wstring( s )
{ }

tstring::tstring( const wchar_t* s, size_t len )
  : wstring( s, len )
{ }

tstring::tstring( const wstring& s )
  : wstring( s )
{ }

string tstring::encode( Code tocode ) const {
  string outbuf;
  iconv( tocode, Code::CHAR )( *this, outbuf );
  return outbuf;
}

void tstring::init( const char* s, size_t len, Code code ) {
  iconv( Code::CHAR, code )( string( s, len ), *this );
}
}
}
}
