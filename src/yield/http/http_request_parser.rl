// yield/http/http_request_parser.rl

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


#include "yield/assert.hpp"
#include "yield/page.hpp"
#include "yield/http/http_request_parser.hpp"

#include <stdlib.h> // For atof

#ifdef _WIN32
  #pragma warning( push )
  #pragma warning( disable: 4702 )
#endif


namespace yield
{
  namespace http
  {
    using yield::net::URI;


    Object& HTTPRequestParser::parse()
    {
      if ( p < eof )
      {
        ps = p;

        int cs;
        HTTPRequest::Method method = HTTPRequest::METHOD_GET;
        float http_version = 1.1F;
        // URI variables
        iovec fragment = { 0 };
        iovec host;
          host.iov_base = const_cast<char*>( "localhost" );
          host.iov_len = 9;
        iovec path = { 0 };
        uint16_t port = 80;
        iovec query = { 0 };
        iovec scheme;
          scheme.iov_base = const_cast<char*>( "http" );
          scheme.iov_len = 4;
        iovec userinfo = { 0 };

        %%{
          machine request_line_parser;
          alphtype unsigned char;

          include basic_rules "basic_rules.rl";
          include uri "../net/uri.rl";

          method
            = ( "CONNECT" % { method = HTTPRequest::METHOD_CONNECT; } ) |
              ( "COPY" % { method = HTTPRequest::METHOD_COPY; } ) |
              ( "DELETE" % { method = HTTPRequest::METHOD_DELETE; } ) |
              ( "GET" % { method = HTTPRequest::METHOD_GET; } ) |
              ( "HEAD" % { method = HTTPRequest::METHOD_HEAD; } ) |
              ( "LOCK" % { method = HTTPRequest::METHOD_LOCK; } ) |
              ( "MKCOL" % { method = HTTPRequest::METHOD_MKCOL; } ) |
              ( "MOVE" % { method = HTTPRequest::METHOD_MOVE; } ) |
              ( "OPTIONS" % { method = HTTPRequest::METHOD_OPTIONS; } ) |
              ( "PATCH" % { method = HTTPRequest::METHOD_PATCH; } ) |
              ( "POST" % { method = HTTPRequest::METHOD_POST; } ) |
              ( "PROPFIND" % { method = HTTPRequest::METHOD_PROPFIND; } ) |
              ( "PROPPATCH" % { method = HTTPRequest::METHOD_PROPPATCH; } ) |
              ( "PUT" % { method = HTTPRequest::METHOD_PUT; } ) |
              ( "TRACE" % { method = HTTPRequest::METHOD_TRACE; } ) |
              ( "UNLOCK" % { method = HTTPRequest::METHOD_UNLOCK; } );

          request_uri =
            (
              (
                '*'
                >{ path.iov_base = p; }
                %{ path.iov_len = p - static_cast<char*>( path.iov_base ); }
              ) |
              absolute_uri |
              path_absolute |
              authority
            );

          request_line = method ' ' request_uri ' ' http_version crlf;

          main := request_line
                  @{ fbreak; };

          write data;
          write init;
          write exec noend;
        }%%

        if ( cs != request_line_parser_error )
        {
          URI uri
              (
                get_buffer(),
                fragment,
                host,
                path,
                port,
                query,
                scheme,
                userinfo
              );

          uint16_t fields_offset;
          size_t content_length;

          if ( parse_fields( fields_offset, content_length ) )
          {
            void* body;

            if ( parse_body( content_length, body ) )
            {
              return *new HTTPRequest
                          (
                            body,
                            get_buffer(),
                            content_length,
                            fields_offset,
                            http_version,
                            method,
                            uri
                          );
            }
            else
            {
              Buffer* next_buffer
                = new Page( p - ps + content_length, ps, eof - ps );
              ps = p;
              return *next_buffer;
            }
          }
        }
        else // cs == request_line_parser_error
        {
          Object* object = parse_body_chunk();
          if ( object != NULL )
            return *object;
        }

        if ( p == eof ) // EOF parsing
        {
          Buffer* next_buffer
            = new Page( eof - ps + Page::getpagesize(), ps, eof - ps );
          p = ps;
          return *next_buffer;
        }
        else // Error parsing
          return *new HTTPResponse( NULL, http_version, 400 );
      }
      else // p == eof
        return *new Page;
    }
  }
}


#ifdef _WIN32
  #pragma warning( pop )
#endif
//
