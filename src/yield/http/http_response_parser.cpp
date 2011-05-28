// yield/http/http_response_parser.cpp

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

/* #line 1 "./src/yield/http/http_response_parser.rl" */
// yield/http/http_response_parser.rl

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

#include "yield/assert.hpp"
#include "yield/buffer.hpp"
#include "yield/http/http_response_parser.hpp"

#include <stdlib.h> // For atof and atoi

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4702)
#else
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

namespace yield {
namespace http {
Object& HTTPResponseParser::parse() {
  if (p < eof) {
    ps = p;

    uint8_t http_version;
    uint16_t status_code;
    if (parse_status_line(http_version, status_code)) {
      uint16_t fields_offset;
      size_t content_length;
      if (parse_fields(fields_offset, content_length)) {
        Object* body;
        if (parse_body(content_length, body)) {
          return create_http_response(
                   body,
                   fields_offset,
                   buffer,
                   http_version,
                   status_code
                 );
        } else {
          Buffer& next_buffer
          = Buffer::copy(
              Buffer::getpagesize(),
              p - ps + content_length,
              ps,
              eof - ps
            );
          ps = p;
          return next_buffer;
        }
      }
    } else {
      Object* object = parse_body_chunk();
      if (object != NULL)
        return *object;
    }

    if (p == eof) { // EOF parsing
      Buffer& next_buffer
      = Buffer::copy(
          Buffer::getpagesize(),
          eof - ps + Buffer::getpagesize(),
          ps,
          eof - ps
        );
      p = ps;
      return next_buffer;
    } else { // Error parsing
      HTTPResponse& http_response
      = create_http_response(400, NULL, http_version);
      http_response.set_field("Content-Length", 14, "0", 1);
      return http_response;
    }
  } else // p == eof
    return *new Buffer(Buffer::getpagesize(), Buffer::getpagesize());
}

bool
HTTPResponseParser::parse_status_line(
  OUT uint8_t& http_version,
  OUT uint16_t& status_code
) {
  int cs;


  /* #line 114 "./src/yield/http/http_response_parser.cpp" */
  static const char _status_line_parser_actions[] = {
    0, 1, 0, 1, 1, 1, 2, 1,
    3, 1, 4
  };

  static const char _status_line_parser_key_offsets[] = {
    0, 0, 1, 2, 3, 4, 5, 6,
    7, 9, 10, 13, 16, 21, 27, 28
  };

  static const unsigned char _status_line_parser_trans_keys[] = {
    72u, 84u, 84u, 80u, 47u, 49u, 46u, 48u,
    49u, 32u, 32u, 48u, 57u, 32u, 48u, 57u,
    32u, 65u, 90u, 97u, 122u, 13u, 32u, 65u,
    90u, 97u, 122u, 10u, 0
  };

  static const char _status_line_parser_single_lengths[] = {
    0, 1, 1, 1, 1, 1, 1, 1,
    2, 1, 1, 1, 1, 2, 1, 0
  };

  static const char _status_line_parser_range_lengths[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 2, 2, 0, 0
  };

  static const char _status_line_parser_index_offsets[] = {
    0, 0, 2, 4, 6, 8, 10, 12,
    14, 17, 19, 22, 25, 29, 34, 36
  };

  static const char _status_line_parser_indicies[] = {
    1, 0, 2, 0, 3, 0, 4, 0,
    5, 0, 6, 0, 7, 0, 8, 9,
    0, 10, 0, 10, 11, 0, 12, 13,
    0, 14, 14, 14, 0, 15, 14, 14,
    14, 0, 16, 0, 0, 0
  };

  static const char _status_line_parser_trans_targs[] = {
    0, 2, 3, 4, 5, 6, 7, 8,
    9, 9, 10, 11, 12, 11, 13, 14,
    15
  };

  static const char _status_line_parser_trans_actions[] = {
    9, 0, 0, 0, 0, 0, 0, 0,
    1, 3, 0, 5, 0, 0, 0, 0,
    7
  };

  static const char _status_line_parser_eof_actions[] = {
    0, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 0
  };

  static const int status_line_parser_start = 1;
  static const int status_line_parser_first_final = 15;
  static const int status_line_parser_error = 0;

  static const int status_line_parser_en_main = 1;


  /* #line 179 "./src/yield/http/http_response_parser.cpp" */
  {
    cs = status_line_parser_start;
  }

  /* #line 184 "./src/yield/http/http_response_parser.cpp" */
  {
    int _klen;
    unsigned int _trans;
    const char* _acts;
    unsigned int _nacts;
    const unsigned char* _keys;

    if (cs == 0)
      goto _out;
_resume:
    _keys = _status_line_parser_trans_keys + _status_line_parser_key_offsets[cs];
    _trans = _status_line_parser_index_offsets[cs];

    _klen = _status_line_parser_single_lengths[cs];
    if (_klen > 0) {
      const unsigned char* _lower = _keys;
      const unsigned char* _mid;
      const unsigned char* _upper = _keys + _klen - 1;
      while (1) {
        if (_upper < _lower)
          break;

        _mid = _lower + ((_upper - _lower) >> 1);
        if ((*p) < *_mid)
          _upper = _mid - 1;
        else if ((*p) > *_mid)
          _lower = _mid + 1;
        else {
          _trans += (_mid - _keys);
          goto _match;
        }
      }
      _keys += _klen;
      _trans += _klen;
    }

    _klen = _status_line_parser_range_lengths[cs];
    if (_klen > 0) {
      const unsigned char* _lower = _keys;
      const unsigned char* _mid;
      const unsigned char* _upper = _keys + (_klen << 1) - 2;
      while (1) {
        if (_upper < _lower)
          break;

        _mid = _lower + (((_upper - _lower) >> 1) & ~1);
        if ((*p) < _mid[0])
          _upper = _mid - 2;
        else if ((*p) > _mid[1])
          _lower = _mid + 2;
        else {
          _trans += ((_mid - _keys) >> 1);
          goto _match;
        }
      }
      _trans += _klen;
    }

_match:
    _trans = _status_line_parser_indicies[_trans];
    cs = _status_line_parser_trans_targs[_trans];

    if (_status_line_parser_trans_actions[_trans] == 0)
      goto _again;

    _acts = _status_line_parser_actions + _status_line_parser_trans_actions[_trans];
    _nacts = (unsigned int) * _acts++;
    while (_nacts-- > 0) {
      switch (*_acts++) {
      case 0:
        /* #line 32 "./src/yield/http/rfc2616.rl" */
      {
        http_version = 0;
      }
      break;
      case 1:
        /* #line 33 "./src/yield/http/rfc2616.rl" */
      {
        http_version = 1;
      }
      break;
      case 2:
        /* #line 118 "./src/yield/http/http_response_parser.rl" */
      {
        status_code = static_cast<uint16_t>(atoi(p));
      }
      break;
      case 3:
        /* #line 126 "./src/yield/http/http_response_parser.rl" */
      { {
          p++;
          goto _out;
        }
      }
      break;
      case 4:
        /* #line 127 "./src/yield/http/http_response_parser.rl" */
      {
        return false;
      }
      break;
      /* #line 276 "./src/yield/http/http_response_parser.cpp" */
      }
    }

_again:
    if (cs == 0)
      goto _out;
    p += 1;
    goto _resume;
    if (p == eof) {
      const char* __acts = _status_line_parser_actions + _status_line_parser_eof_actions[cs];
      unsigned int __nacts = (unsigned int) * __acts++;
      while (__nacts-- > 0) {
        switch (*__acts++) {
        case 4:
          /* #line 127 "./src/yield/http/http_response_parser.rl" */
        {
          return false;
        }
        break;
        /* #line 295 "./src/yield/http/http_response_parser.cpp" */
        }
      }
    }

_out:
    {}
  }

  /* #line 132 "./src/yield/http/http_response_parser.rl" */


  return cs != status_line_parser_error;
}
}
}

#ifdef _WIN32
#pragma warning(pop)
#else
#pragma GCC diagnostic warning "-Wold-style-cast"
#pragma GCC diagnostic warning "-Wunused-variable"
#endif
//
