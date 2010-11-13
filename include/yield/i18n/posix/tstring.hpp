// yield/i18n/posix/tstring.hpp

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


#ifndef _YIELD_I18N_POSIX_TSTRING_HPP_
#define _YIELD_I18N_POSIX_TSTRING_HPP_


#include "yield/types.hpp"
#include "yield/i18n/code.hpp"


namespace yield
{
  namespace i18n
  {
    namespace posix
    {
      class tstring : public string
      {
      public:
        tstring() { }

        tstring( char s, Code code = Code::CHAR );
        tstring( const char* s, Code code = Code::CHAR );
        tstring( const string& s, Code code = Code::CHAR );
        tstring( const char* s, size_t len, Code code = Code::CHAR );

        tstring( wchar_t s );
        tstring( const wchar_t* s );
        tstring( const wchar_t* s, size_t len );
        tstring( const wstring& s );

        string encode( Code tocode ) const;

      private:
        void init( const char*, size_t, Code );
        void init( const wchar_t*, size_t );
      };
    }
  }
}


#endif
