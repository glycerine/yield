// yield/http/chunk.rl

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

# Adapted from the ABNF in RFC 2616 section 3.6.1
%%{
  machine chunk;
  alphtype unsigned char;

  include field "field.rl";

  chunk_size = xdigit+
            >{ chunk_size_p = p; }
            %
            {
              char* chunk_size_pe = p;
              chunk_size
                = static_cast<size_t>
                  (
                    strtol(chunk_size_p, &chunk_size_pe, 16)
                 );
            };

  chunk_ext_name = token;
  chunk_ext_val = token | quoted_string;
  chunk_extension
    = (';' chunk_ext_name ('=' chunk_ext_val)?)*;

  body_chunk = chunk_size chunk_extension? crlf
               (
               (any when { seen_chunk_size++ < chunk_size })*
               >{ chunk_data_p = p; }
              )
               crlf;

  last_chunk = ('0' %{ chunk_size = 0; }) chunk_extension? crlf
               field :> crlf*
               crlf;

  chunk = last_chunk | body_chunk;
}%%
