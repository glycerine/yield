// yield/http/rfc2616.rl

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

%%{  machine rfc2616;  alphtype unsigned char;  include rfc822 "rfc822.rl";  separators = '(' | ')' | '<' | '>' | '@' |               ',' | ';' | ':' | '\\' | "'" |               '/' | '[' | ']' | '?' | '=' |               '{' | '}' | ' ' | '\t';  text = extend -- ctl;  token = (ascii -- (ctl | separators))+;  qdtext = text -- '\"';  quoted_string = '\"' (qdtext | quoted_pair)* '\"';  http_version = "HTTP/1."
                 (
                   ('0' >{ http_version = 0; }) |
                   ('1' >{ http_version = 1; })
                 );
  hour = digit{2} >{ hour = atoi(p); };
  minute = digit{2} >{ minute = atoi(p); };
  second = digit{2} >{ second = atoi(p); };
  time = hour ':' minute ':' second;
  day1 = digit >{ day = atoi(p); };
  day2 = digit{2} >{ day = atoi(p); };
  wkday = "Mon" | "Tue" | "Wed" | "Thu" | "Fri" | "Sat" | "Sun";
  weekday = "Monday" | "Tuesday" | "Wednesday" | "Thursday" |
            "Friday" | "Saturday" | "Sunday";
  month = "Jan" | "Feb" | "Mar" | "Apr" | "May" | "Jun" | "Jul" | "Aug" |
          "Sep" | "Oct" | "Nov" | "Dec"
          %
          {
            switch (*(p - 1))
            {
              case 'b': month = 2; break;
              case 'c': month = 12; break;
              case 'l': month = 7; break;
              case 'g': month = 8; break;
              case 'n':
              {
                switch ((*p - 2))
                {
                  case 'a': month = 1; break;
                  case 'u': month = 6; break;
                }
              }
              break;
              case 'p': month = 9; break;
              case 'r':
              {
                switch (*(p - 2))
                {
                  case 'a': month = 3; break;
                  case 'p': month = 4; break;
                }
              }
              break;
              case 't': month = 10; break;
              case 'v': month = 11; break;
              case 'y': month = 5; break;
            }
          };
  year2 = digit{2} >{ year = atoi(p); year += (year < 50 ? 2000 : 1900); };
  year4 = digit{4} >{ year = atoi(p); };
  rfc1123_date = wkday ", " day2 ' ' month ' ' year4 ' ' time " GMT";
  rfc850_date  = weekday ", " day2 '-' month '-' year2 ' ' time " GMT";
  asctime = wkday ' ' month ' ' (day2 | (' ' day1)) ' ' time ' ' year4;
  date = rfc1123_date | rfc850_date | asctime;  field_name    = token      >{ field_name.iov_base = p; }      %{ field_name.iov_len = p - static_cast<char*>(field_name.iov_base); };  field_content = text*;  field_value    = field_content      >{ field_value.iov_base = p; }      %{ field_value.iov_len = p - static_cast<char*>(field_value.iov_base); };  field = field_name ':' ' '* field_value :> crlf;  chunk_size = xdigit+
               >{ chunk_size_p = p; }
               %
               {
                 char* chunk_size_pe = p;
                 chunk_size
                   = static_cast<size_t>(
                       strtol(chunk_size_p, &chunk_size_pe, 16)
                     );
               };

  chunk_ext_name = token;
  chunk_ext_val = token | quoted_string;
  chunk_extension = (';' chunk_ext_name ('=' chunk_ext_val)?)*;

  body_chunk = chunk_size chunk_extension? crlf
               (
                 (any when { seen_chunk_size++ < chunk_size })*
                 >{ chunk_data_p = p; }
               )
               crlf;

  trailer = field*;

  last_chunk = ('0' %{ chunk_size = 0; }) chunk_extension? crlf
               trailer
               :>
               crlf;

  chunk = last_chunk | body_chunk;
}%%
