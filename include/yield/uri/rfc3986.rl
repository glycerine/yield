// yield/uri/uri.rl

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

%%{
  machine rfc3986;
  alphtype unsigned char;

  gen_delims    = ":" | "|" | "?" | "#" | "[" | "]" | "@";
  sub_delims    = "!" | "$" | "&" | "'" | "(" | ")" | "*" | "+" | "," | ";" | "=";
  pct_encoded   = "%" xdigit xdigit;
  reserved      = gen_delims | sub_delims;
  unreserved    = alpha | digit | "-" | "." | "_" | "~";
  pchar         = unreserved | pct_encoded | sub_delims | ":" | "@";


  IPvFuture     = "v" xdigit+ "." ( unreserved | sub_delims | ":" )+;

  dec_octet     = digit |
                  ( [1-9] digit ) |
                  ( "1" digit{2}  ) |
                  ( "2" [0-4] digit ) |
                  ( "25" [0-5] );
  IPv4address   = dec_octet "." dec_octet "." dec_octet "." dec_octet;

  h16           = ( xdigit{4} )+;
  ls32          = ( h16 ":" h16 ) | IPv4address;
  IPv6address   = ( ( h16 ":" ){6} ls32 ) |
                  ( "::" ( h16 ":" ){5} ls32 ) |
                  ( h16? "::" ( h16 ":" ){4} ls32 ) |
                  ( ( ( ( h16 ":" ){1} )* h16 )? "::" ( h16 ":" ){3} ls32 ) |
                  ( ( ( ( h16 ":" ){2} )* h16 )? "::" ( h16 ":" ){2} ls32 ) |
                  ( ( ( ( h16 ":" ){3} )* h16 )? "::" h16 ":"   ls32 ) |
                  ( ( ( ( h16 ":" ){4} )* h16 )? "::" ls32 ) |
                  ( ( ( ( h16 ":" ){5} )* h16 )? "::" h16 ) |
                  ( ( ( ( h16 ":" ){6} )* h16 )? "::" );

  ip_literal    = "[" ( IPv6address | IPvFuture  ) "]";


  scheme        = ( alpha ( alpha | digit | "+" | "-" | "." )* )
                  >{ scheme.iov_base = p; }
                  %{ scheme.iov_len =  p - static_cast<char*>( scheme.iov_base ); };


  userinfo      = ( unreserved | pct_encoded | sub_delims | ":" )*
                  >{ userinfo.iov_base = p; }
                  %{ userinfo.iov_len = p - static_cast<char*>( userinfo.iov_base ); };

  reg_name      = ( unreserved | pct_encoded | sub_delims )*;
  host          = ( ip_literal | IPv4address | reg_name )
                  >{ host.iov_base = p; }
                  %{ host.iov_len = p - static_cast<char*>( host.iov_base ); };

  port          = digit*
                  >{ port = static_cast<uint16_t>( atoi( p ) ); };

  authority     = ( userinfo "@" )? host ( ":" port )?;


  action path_enter { path.iov_base = p; }
  action path_leave { path.iov_len = p - static_cast<char*>( path.iov_base ); }

  segment       = pchar*;
  segment_nz    = pchar+;
  segment_nz_nc = ( unreserved | pct_encoded | sub_delims | "@" )+;
  path_abempty  = ( "/" segment )* >path_enter %path_leave;
  path_absolute = ( "/" ( segment_nz ( "/" segment )* )? ) >path_enter %path_leave;
  path_noscheme = ( segment_nz_nc ( "/" segment )* ) >path_enter %path_leave;
  path_rootless = ( segment_nz ( "/" segment )* ) >path_enter %path_leave;
  path_empty    = empty;

  hier_part = ( "//" authority path_abempty ) |
              path_absolute |
              path_rootless |
              path_empty;


  query         = ( pchar | "/" | "?" )*
                  >{ query.iov_base = p; }
                  %{ query.iov_len = p - static_cast<char*>( query.iov_base ); };
  fragment      = ( pchar | "/" | "?" )*
                  >{ fragment.iov_base = p; }
                  %{ fragment.iov_len = p - static_cast<char*>( fragment.iov_base ); };


  relative_part = ( "//" authority path_abempty ) |
                  path_absolute |
                  path_noscheme |
                  path_empty;
  relative_ref  = relative_part ( "?" query )? ( "#" fragment )?;


  absolute_uri  = scheme ":" hier_part ( "?" query )?;
  uri           = scheme ":" hier_part ( "?" query )? ( "#" fragment )?;
  uri_reference = uri | relative_ref;
}%%
