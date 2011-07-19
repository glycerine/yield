// yield/http/http_message_parser.cpp

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

/* #line 1 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */
// yield/http/http_message_parser.rl

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
#include "yield/http/http_message_body_chunk.hpp"
#include "yield/http/http_message_parser.hpp"
#include "yield/http/http_request.hpp"

#include <stdlib.h> // For strtol

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4702)
#else
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

namespace yield {
namespace http {
HTTPMessageParser::HTTPMessageParser(Buffer& buffer)
  : buffer(buffer.inc_ref()) {
  debug_assert_false(buffer.empty());

  ps = p = buffer;
  eof = ps + buffer.size();
}

HTTPMessageParser::HTTPMessageParser(const string& buffer)
  : buffer(Buffer::copy(buffer)) {
  debug_assert_false(buffer.empty());

  ps = p = this->buffer;
  eof = ps + this->buffer.size();
}

HTTPMessageParser::~HTTPMessageParser() {
  Buffer::dec_ref(buffer);
}

bool
HTTPMessageParser::parse_body(
  size_t content_length,
  YO_NEW_REF Object*& body
) {
  if (
    content_length == 0
    ||
    content_length == HTTPRequest::CONTENT_LENGTH_CHUNKED
  ) {
    body = NULL;
    return true;
  } else if (static_cast<size_t>(eof - p) >= content_length) {
    body = &Buffer::copy(p, content_length);
    p += content_length;
    return true;
  } else {
    return false;
  }
}

Object* HTTPMessageParser::parse_body_chunk() {
  const char* chunk_data_p = NULL;
  size_t chunk_size = 0;
  const char* chunk_size_p = NULL;
  int cs;
  iovec field_name = {0, 0}, field_value = {0, 0};
  size_t seen_chunk_size = 0;

  ps = p;


  /* #line 2 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  static const char _chunk_parser_actions[] = {
    0, 1, 0, 1, 1, 1, 2, 1,
    3, 1, 4, 1, 5, 1, 6, 1,
    8, 1, 9, 2, 0, 6, 2, 2,
    3, 2, 7, 5
  };

  static const char _chunk_parser_cond_offsets[] = {
    0, 0, 0, 0, 0, 1, 1, 1,
    1, 1, 1, 1, 2, 3, 4, 5,
    5, 6, 7, 7, 7, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 9,
    9
  };

  static const char _chunk_parser_cond_lengths[] = {
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    1
  };

  static const short _chunk_parser_cond_keys[] = {
    0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u,
    0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u,
    0u, 255u, 0u, 255u, 0
  };

  static const char _chunk_parser_cond_spaces[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0
  };

  static const short _chunk_parser_key_offsets[] = {
    0, 0, 7, 15, 16, 54, 55, 70,
    75, 76, 91, 95, 99, 105, 145, 158,
    159, 165, 176, 184, 185, 189, 203, 220,
    235, 251, 272, 277, 279, 281, 288, 307,
    329, 347, 361, 378, 393, 409, 430, 435,
    437, 439, 446, 465, 487, 505, 505, 509,
    524
  };

  static const short _chunk_parser_trans_keys[] = {
    48u, 49u, 57u, 65u, 70u, 97u, 102u, 13u,
    59u, 48u, 57u, 65u, 70u, 97u, 102u, 10u,
    269u, 380u, 382u, 525u, 556u, 559u, 635u, 637u,
    289u, 294u, 298u, 299u, 301u, 302u, 304u, 313u,
    321u, 346u, 350u, 378u, 512u, 544u, 545u, 550u,
    551u, 553u, 554u, 569u, 570u, 576u, 577u, 602u,
    603u, 605u, 606u, 638u, 639u, 767u, 10u, 58u,
    124u, 126u, 33u, 38u, 42u, 43u, 45u, 46u,
    48u, 57u, 65u, 90u, 94u, 122u, 13u, 32u,
    127u, 0u, 31u, 10u, 13u, 124u, 126u, 33u,
    38u, 42u, 43u, 45u, 46u, 48u, 57u, 65u,
    90u, 94u, 122u, 13u, 127u, 0u, 31u, 269u,
    525u, 512u, 767u, 266u, 269u, 522u, 525u, 512u,
    767u, 269u, 314u, 380u, 382u, 525u, 556u, 559u,
    570u, 635u, 637u, 289u, 294u, 298u, 299u, 301u,
    302u, 304u, 313u, 321u, 346u, 350u, 378u, 512u,
    544u, 545u, 550u, 551u, 553u, 554u, 569u, 571u,
    576u, 577u, 602u, 603u, 605u, 606u, 638u, 639u,
    767u, 269u, 288u, 525u, 544u, 639u, 289u, 382u,
    384u, 511u, 512u, 543u, 545u, 767u, 10u, 266u,
    269u, 522u, 525u, 512u, 767u, 269u, 525u, 639u,
    288u, 382u, 384u, 511u, 512u, 543u, 544u, 767u,
    13u, 59u, 48u, 57u, 65u, 70u, 97u, 102u,
    10u, 269u, 525u, 512u, 767u, 124u, 126u, 33u,
    38u, 42u, 43u, 45u, 46u, 48u, 57u, 65u,
    90u, 94u, 122u, 13u, 59u, 61u, 124u, 126u,
    33u, 38u, 42u, 43u, 45u, 46u, 48u, 57u,
    65u, 90u, 94u, 122u, 34u, 124u, 126u, 33u,
    38u, 42u, 43u, 45u, 46u, 48u, 57u, 65u,
    90u, 94u, 122u, 13u, 59u, 124u, 126u, 33u,
    38u, 42u, 43u, 45u, 46u, 48u, 57u, 65u,
    90u, 94u, 122u, 13u, 34u, 59u, 92u, 124u,
    126u, 127u, 0u, 31u, 33u, 38u, 42u, 43u,
    45u, 46u, 48u, 57u, 65u, 90u, 94u, 122u,
    34u, 92u, 127u, 0u, 31u, 13u, 59u, 34u,
    92u, 13u, 34u, 59u, 92u, 127u, 0u, 31u,
    34u, 92u, 124u, 126u, 127u, 0u, 31u, 33u,
    38u, 42u, 43u, 45u, 46u, 48u, 57u, 65u,
    90u, 94u, 122u, 13u, 34u, 59u, 61u, 92u,
    124u, 126u, 127u, 0u, 31u, 33u, 38u, 42u,
    43u, 45u, 46u, 48u, 57u, 65u, 90u, 94u,
    122u, 92u, 124u, 126u, 127u, 0u, 31u, 33u,
    38u, 42u, 43u, 45u, 46u, 48u, 57u, 65u,
    90u, 94u, 122u, 124u, 126u, 33u, 38u, 42u,
    43u, 45u, 46u, 48u, 57u, 65u, 90u, 94u,
    122u, 13u, 59u, 61u, 124u, 126u, 33u, 38u,
    42u, 43u, 45u, 46u, 48u, 57u, 65u, 90u,
    94u, 122u, 34u, 124u, 126u, 33u, 38u, 42u,
    43u, 45u, 46u, 48u, 57u, 65u, 90u, 94u,
    122u, 13u, 59u, 124u, 126u, 33u, 38u, 42u,
    43u, 45u, 46u, 48u, 57u, 65u, 90u, 94u,
    122u, 13u, 34u, 59u, 92u, 124u, 126u, 127u,
    0u, 31u, 33u, 38u, 42u, 43u, 45u, 46u,
    48u, 57u, 65u, 90u, 94u, 122u, 34u, 92u,
    127u, 0u, 31u, 13u, 59u, 34u, 92u, 13u,
    34u, 59u, 92u, 127u, 0u, 31u, 34u, 92u,
    124u, 126u, 127u, 0u, 31u, 33u, 38u, 42u,
    43u, 45u, 46u, 48u, 57u, 65u, 90u, 94u,
    122u, 13u, 34u, 59u, 61u, 92u, 124u, 126u,
    127u, 0u, 31u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 92u,
    124u, 126u, 127u, 0u, 31u, 33u, 38u, 42u,
    43u, 45u, 46u, 48u, 57u, 65u, 90u, 94u,
    122u, 269u, 525u, 512u, 767u, 13u, 124u, 126u,
    33u, 38u, 42u, 43u, 45u, 46u, 48u, 57u,
    65u, 90u, 94u, 122u, 269u, 380u, 382u, 525u,
    556u, 559u, 635u, 637u, 289u, 294u, 298u, 299u,
    301u, 302u, 304u, 313u, 321u, 346u, 350u, 378u,
    512u, 544u, 545u, 550u, 551u, 553u, 554u, 569u,
    570u, 576u, 577u, 602u, 603u, 605u, 606u, 638u,
    639u, 767u, 0
  };

  static const char _chunk_parser_single_lengths[] = {
    0, 1, 2, 1, 8, 1, 3, 3,
    1, 3, 2, 2, 4, 10, 5, 1,
    4, 3, 2, 1, 2, 2, 5, 3,
    4, 7, 3, 2, 2, 5, 5, 8,
    4, 2, 5, 3, 4, 7, 3, 2,
    2, 5, 5, 8, 4, 0, 2, 3,
    8
  };

  static const char _chunk_parser_range_lengths[] = {
    0, 3, 3, 0, 15, 0, 6, 1,
    0, 6, 1, 1, 1, 15, 4, 0,
    1, 4, 3, 0, 1, 6, 6, 6,
    6, 7, 1, 0, 0, 1, 7, 7,
    7, 6, 6, 6, 6, 7, 1, 0,
    0, 1, 7, 7, 7, 0, 1, 6,
    15
  };

  static const short _chunk_parser_index_offsets[] = {
    0, 0, 5, 11, 13, 37, 39, 49,
    54, 56, 66, 70, 74, 80, 106, 116,
    118, 124, 132, 138, 140, 144, 153, 165,
    175, 186, 201, 206, 209, 212, 219, 232,
    248, 260, 269, 281, 291, 302, 317, 322,
    325, 328, 335, 348, 364, 376, 377, 381,
    391
  };

  static const char _chunk_parser_indicies[] = {
    1, 2, 2, 2, 0, 3, 5, 4,
    4, 4, 0, 6, 0, 7, 8, 8,
    10, 9, 9, 9, 9, 8, 8, 8,
    8, 8, 8, 9, 11, 9, 11, 9,
    11, 9, 11, 9, 0, 12, 0, 14,
    13, 13, 13, 13, 13, 13, 13, 13,
    0, 15, 16, 0, 0, 17, 18, 0,
    19, 8, 8, 8, 8, 8, 8, 8,
    8, 0, 20, 0, 0, 21, 19, 23,
    22, 0, 12, 19, 24, 23, 22, 0,
    19, 14, 13, 13, 23, 22, 22, 26,
    22, 22, 13, 13, 13, 13, 13, 13,
    22, 25, 22, 25, 22, 25, 22, 25,
    22, 0, 27, 16, 28, 29, 22, 17,
    17, 22, 30, 0, 31, 0, 31, 19,
    32, 23, 22, 0, 33, 34, 22, 21,
    21, 22, 35, 0, 36, 37, 4, 4,
    4, 0, 38, 0, 7, 10, 9, 0,
    39, 39, 39, 39, 39, 39, 39, 39,
    0, 40, 41, 42, 39, 39, 39, 39,
    39, 39, 39, 39, 0, 44, 43, 43,
    43, 43, 43, 43, 43, 43, 0, 40,
    41, 43, 43, 43, 43, 43, 43, 43,
    43, 0, 40, 43, 46, 47, 44, 44,
    0, 0, 44, 44, 44, 44, 44, 44,
    45, 48, 47, 0, 0, 45, 40, 41,
    0, 49, 47, 45, 40, 48, 46, 47,
    0, 0, 45, 39, 47, 50, 50, 0,
    0, 50, 50, 50, 50, 50, 50, 45,
    40, 39, 46, 51, 47, 50, 50, 0,
    0, 50, 50, 50, 50, 50, 50, 45,
    47, 44, 44, 0, 0, 44, 44, 44,
    44, 44, 44, 45, 52, 52, 52, 52,
    52, 52, 52, 52, 0, 53, 54, 55,
    52, 52, 52, 52, 52, 52, 52, 52,
    0, 57, 56, 56, 56, 56, 56, 56,
    56, 56, 0, 53, 54, 56, 56, 56,
    56, 56, 56, 56, 56, 0, 53, 56,
    59, 60, 57, 57, 0, 0, 57, 57,
    57, 57, 57, 57, 58, 61, 60, 0,
    0, 58, 53, 54, 0, 62, 60, 58,
    53, 61, 59, 60, 0, 0, 58, 52,
    60, 63, 63, 0, 0, 63, 63, 63,
    63, 63, 63, 58, 53, 52, 59, 64,
    60, 63, 63, 0, 0, 63, 63, 63,
    63, 63, 63, 58, 60, 57, 57, 0,
    0, 57, 57, 57, 57, 57, 57, 58,
    0, 19, 23, 22, 0, 19, 8, 8,
    8, 8, 8, 8, 8, 8, 0, 19,
    8, 8, 23, 22, 22, 22, 22, 8,
    8, 8, 8, 8, 8, 22, 65, 22,
    65, 22, 65, 22, 65, 22, 0, 0
  };

  static const char _chunk_parser_trans_targs[] = {
    0, 2, 18, 3, 18, 33, 4, 5,
    6, 11, 12, 13, 45, 6, 7, 8,
    7, 10, 9, 5, 8, 10, 11, 12,
    46, 13, 14, 15, 16, 14, 17, 47,
    48, 15, 16, 17, 19, 21, 20, 22,
    19, 21, 23, 24, 25, 26, 30, 28,
    27, 29, 31, 32, 34, 3, 33, 35,
    36, 37, 38, 42, 40, 39, 41, 43,
    44, 13
  };

  static const char _chunk_parser_trans_actions[] = {
    17, 9, 9, 25, 0, 25, 0, 13,
    1, 13, 13, 19, 15, 0, 3, 22,
    5, 5, 0, 0, 7, 0, 0, 0,
    15, 0, 3, 22, 22, 5, 5, 15,
    15, 7, 7, 0, 11, 11, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1
  };

  static const char _chunk_parser_eof_actions[] = {
    0, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 0, 0, 0,
    0
  };

  static const int chunk_parser_start = 1;
  static const int chunk_parser_first_final = 45;
  static const int chunk_parser_error = 0;

  static const int chunk_parser_en_main = 1;


  /* #line 249 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  {
    cs = chunk_parser_start;
  }

  /* #line 252 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  {
    int _klen;
    unsigned int _trans;
    short _widec;
    const char* _acts;
    unsigned int _nacts;
    const short* _keys;

    if (cs == 0) {
      goto _out;
    }
_resume:
    _widec = (*p);
    _klen = _chunk_parser_cond_lengths[cs];
    _keys = _chunk_parser_cond_keys + (_chunk_parser_cond_offsets[cs] * 2);
    if (_klen > 0) {
      const short* _lower = _keys;
      const short* _mid;
      const short* _upper = _keys + (_klen << 1) - 2;
      while (1) {
        if (_upper < _lower) {
          break;
        }

        _mid = _lower + (((_upper - _lower) >> 1) & ~1);
        if (_widec < _mid[0]) {
          _upper = _mid - 2;
        } else if (_widec > _mid[1]) {
          _lower = _mid + 2;
        } else {
          switch (_chunk_parser_cond_spaces[_chunk_parser_cond_offsets[cs] + ((_mid - _keys) >> 1)]) {
          case 0: {
            _widec = (short)(256u + ((*p) - 0u));
            if (
              /* #line 100 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
              seen_chunk_size++ < chunk_size) {
              _widec += 256;
            }
            break;
          }
          }
          break;
        }
      }
    }

    _keys = _chunk_parser_trans_keys + _chunk_parser_key_offsets[cs];
    _trans = _chunk_parser_index_offsets[cs];

    _klen = _chunk_parser_single_lengths[cs];
    if (_klen > 0) {
      const short* _lower = _keys;
      const short* _mid;
      const short* _upper = _keys + _klen - 1;
      while (1) {
        if (_upper < _lower) {
          break;
        }

        _mid = _lower + ((_upper - _lower) >> 1);
        if (_widec < *_mid) {
          _upper = _mid - 1;
        } else if (_widec > *_mid) {
          _lower = _mid + 1;
        } else {
          _trans += (_mid - _keys);
          goto _match;
        }
      }
      _keys += _klen;
      _trans += _klen;
    }

    _klen = _chunk_parser_range_lengths[cs];
    if (_klen > 0) {
      const short* _lower = _keys;
      const short* _mid;
      const short* _upper = _keys + (_klen << 1) - 2;
      while (1) {
        if (_upper < _lower) {
          break;
        }

        _mid = _lower + (((_upper - _lower) >> 1) & ~1);
        if (_widec < _mid[0]) {
          _upper = _mid - 2;
        } else if (_widec > _mid[1]) {
          _lower = _mid + 2;
        } else {
          _trans += ((_mid - _keys) >> 1);
          goto _match;
        }
      }
      _trans += _klen;
    }

_match:
    _trans = _chunk_parser_indicies[_trans];
    cs = _chunk_parser_trans_targs[_trans];

    if (_chunk_parser_trans_actions[_trans] == 0) {
      goto _again;
    }

    _acts = _chunk_parser_actions + _chunk_parser_trans_actions[_trans];
    _nacts = (unsigned int) * _acts++;
    while (_nacts-- > 0) {
      switch (*_acts++) {
      case 0:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_name.iov_base = p;
      }
      break;
      case 1:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_name.iov_len = p - static_cast<char*>(field_name.iov_base);
      }
      break;
      case 2:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_value.iov_base = p;
      }
      break;
      case 3:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_value.iov_len = p - static_cast<char*>(field_value.iov_base);
      }
      break;
      case 4:
        /* #line 84 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        chunk_size_p = p;
      }
      break;
      case 5:
        /* #line 86 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        char* chunk_size_pe = p;
        chunk_size
        = static_cast<size_t>(
            strtol(chunk_size_p, &chunk_size_pe, 16)
          );
      }
      break;
      case 6:
        /* #line 101 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        chunk_data_p = p;
      }
      break;
      case 7:
        /* #line 107 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        chunk_size = 0;
      }
      break;
      case 8:
        /* #line 105 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */
      { {
          p++;
          goto _out;
        }
      }
      break;
      case 9:
        /* #line 106 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */
      {
        return NULL;
      }
      break;
      /* #line 390 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
      }
    }

_again:
    if (cs == 0) {
      goto _out;
    }
    p += 1;
    goto _resume;
    if (p == eof) {
      const char* __acts = _chunk_parser_actions + _chunk_parser_eof_actions[cs];
      unsigned int __nacts = (unsigned int) * __acts++;
      while (__nacts-- > 0) {
        switch (*__acts++) {
        case 9:
          /* #line 106 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */
        {
          return NULL;
        }
        break;
        /* #line 407 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
        }
      }
    }

_out:
    {}
  }

  /* #line 111 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */


  if (cs != chunk_parser_error) {
    if (chunk_size > 0) {
      // Cut off the chunk size + extension + CRLF before
      // the chunk data and the CRLF after
      return &create_http_message_body_chunk(
               &Buffer::copy(chunk_data_p, p - chunk_data_p - 2)
             );
    } else { // Last chunk
      return &create_http_message_body_chunk(NULL);
    }
  } else if (p == eof && chunk_size != 0) {
    return new Buffer(chunk_size + 2);  // Assumes no trailers.
  } else {
    return NULL;
  }
}

bool
HTTPMessageParser::parse_content_length_field(
  const char* ps,
  const char* pe,
  size_t& content_length
) {
  int cs;
  char* p = const_cast<char*>(ps);

  iovec field_name = {0, 0}, field_value = {0, 0};

  // Don't look for the trailing CRLF before the body,
  // since it may not be present yet.

  /* #line 413 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  static const char _content_length_field_parser_actions[] = {
    0, 1, 0, 1, 1, 1, 2, 1,
    3, 1, 4, 2, 2, 3
  };

  static const char _content_length_field_parser_key_offsets[] = {
    0, 0, 15, 20, 21, 25
  };

  static const unsigned char _content_length_field_parser_trans_keys[] = {
    58u, 124u, 126u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 13u,
    32u, 127u, 0u, 31u, 10u, 13u, 127u, 0u,
    31u, 124u, 126u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 0
  };

  static const char _content_length_field_parser_single_lengths[] = {
    0, 3, 3, 1, 2, 2
  };

  static const char _content_length_field_parser_range_lengths[] = {
    0, 6, 1, 0, 1, 6
  };

  static const char _content_length_field_parser_index_offsets[] = {
    0, 0, 10, 15, 17, 21
  };

  static const char _content_length_field_parser_indicies[] = {
    2, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 3, 4, 1, 1, 5, 6,
    1, 7, 1, 1, 8, 9, 9, 9,
    9, 9, 9, 9, 9, 1, 0
  };

  static const char _content_length_field_parser_trans_targs[] = {
    1, 0, 2, 3, 2, 4, 5, 3,
    4, 1
  };

  static const char _content_length_field_parser_trans_actions[] = {
    0, 0, 3, 11, 5, 5, 9, 7,
    0, 1
  };

  static const int content_length_field_parser_start = 5;
  static const int content_length_field_parser_first_final = 5;
  static const int content_length_field_parser_error = 0;

  static const int content_length_field_parser_en_main = 5;


  /* #line 465 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  {
    cs = content_length_field_parser_start;
  }

  /* #line 468 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  {
    int _klen;
    unsigned int _trans;
    const char* _acts;
    unsigned int _nacts;
    const unsigned char* _keys;

    if (p == pe) {
      goto _test_eof;
    }
    if (cs == 0) {
      goto _out;
    }
_resume:
    _keys = _content_length_field_parser_trans_keys + _content_length_field_parser_key_offsets[cs];
    _trans = _content_length_field_parser_index_offsets[cs];

    _klen = _content_length_field_parser_single_lengths[cs];
    if (_klen > 0) {
      const unsigned char* _lower = _keys;
      const unsigned char* _mid;
      const unsigned char* _upper = _keys + _klen - 1;
      while (1) {
        if (_upper < _lower) {
          break;
        }

        _mid = _lower + ((_upper - _lower) >> 1);
        if ((*p) < *_mid) {
          _upper = _mid - 1;
        } else if ((*p) > *_mid) {
          _lower = _mid + 1;
        } else {
          _trans += (_mid - _keys);
          goto _match;
        }
      }
      _keys += _klen;
      _trans += _klen;
    }

    _klen = _content_length_field_parser_range_lengths[cs];
    if (_klen > 0) {
      const unsigned char* _lower = _keys;
      const unsigned char* _mid;
      const unsigned char* _upper = _keys + (_klen << 1) - 2;
      while (1) {
        if (_upper < _lower) {
          break;
        }

        _mid = _lower + (((_upper - _lower) >> 1) & ~1);
        if ((*p) < _mid[0]) {
          _upper = _mid - 2;
        } else if ((*p) > _mid[1]) {
          _lower = _mid + 2;
        } else {
          _trans += ((_mid - _keys) >> 1);
          goto _match;
        }
      }
      _trans += _klen;
    }

_match:
    _trans = _content_length_field_parser_indicies[_trans];
    cs = _content_length_field_parser_trans_targs[_trans];

    if (_content_length_field_parser_trans_actions[_trans] == 0) {
      goto _again;
    }

    _acts = _content_length_field_parser_actions + _content_length_field_parser_trans_actions[_trans];
    _nacts = (unsigned int) * _acts++;
    while (_nacts-- > 0) {
      switch (*_acts++) {
      case 0:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_name.iov_base = p;
      }
      break;
      case 1:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_name.iov_len = p - static_cast<char*>(field_name.iov_base);
      }
      break;
      case 2:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_value.iov_base = p;
      }
      break;
      case 3:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_value.iov_len = p - static_cast<char*>(field_value.iov_base);
      }
      break;
      case 4:
        /* #line 148 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */
      {
        if (
          parse_content_length_field(
            field_name,
            field_value,
            content_length
          )
        ) {
          return true;
        }
      }
      break;
      /* #line 565 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
      }
    }

_again:
    if (cs == 0) {
      goto _out;
    }
    if (++p != pe) {
      goto _resume;
    }
_test_eof:
    {}
_out:
    {}
  }

  /* #line 163 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */


  return false;
}

bool
HTTPMessageParser::parse_content_length_field(
  const iovec& field_name,
  const iovec& field_value,
  size_t& content_length
) {
  if (
    field_name.iov_len == 14
    &&
    (
      memcmp(field_name.iov_base, "Content-Length", 14) == 0
      ||
      memcmp(field_name.iov_base, "Content-length", 14) == 0
    )
  ) {
    char* nptr = static_cast<char*>(field_value.iov_base);
    char* endptr = nptr + field_value.iov_len;
    content_length = static_cast<size_t>(strtol(nptr, &endptr, 10));
    return true;
  } else if (
    field_name.iov_len == 17
    && (
      memcmp(field_name.iov_base, "Transfer-Encoding", 17) == 0
      ||
      memcmp(field_name.iov_base, "Transfer-encoding", 17) == 0
    )
    &&
    memcmp(field_value.iov_base, "chunked", 7) == 0
  ) {
    content_length = HTTPRequest::CONTENT_LENGTH_CHUNKED;
    return true;
  } else {
    return false;
  }
}

DateTime HTTPMessageParser::parse_date(const iovec& date) {
  return parse_date(
           static_cast<const char*>(date.iov_base),
           static_cast<const char*>(date.iov_base) + date.iov_len
         );
}

DateTime HTTPMessageParser::parse_date(const char* ps, const char* pe) {
  int cs;
  const char* eof = pe;
  char* p = const_cast<char*>(ps);

  int hour = 0, minute = 0, second = 0;
  int day = 0, month = 0, year = 0;


  /* #line 576 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  static const char _date_parser_actions[] = {
    0, 1, 0, 1, 1, 1, 2, 1,
    3, 1, 4, 1, 5, 1, 6, 1,
    7, 1, 8
  };

  static const unsigned char _date_parser_key_offsets[] = {
    0, 0, 5, 6, 7, 10, 18, 20,
    21, 22, 25, 27, 28, 30, 32, 33,
    35, 37, 38, 40, 42, 43, 45, 47,
    49, 51, 53, 54, 55, 56, 57, 58,
    59, 61, 62, 64, 65, 67, 68, 69,
    70, 71, 72, 73, 74, 76, 78, 79,
    87, 89, 90, 91, 93, 95, 97, 99,
    100, 102, 104, 105, 107, 109, 110, 112,
    114, 115, 116, 117, 118, 119, 120, 121,
    122, 123, 124, 126, 127, 129, 130, 132,
    133, 134, 135, 136, 137, 138, 139, 140,
    141, 142, 144, 146, 147, 155, 157, 158,
    159, 161, 162, 163, 164, 165, 166, 167,
    169, 170, 172, 173, 175, 176, 177, 178,
    179, 180, 181, 182, 183, 185, 186, 189,
    190, 191, 193, 194, 197, 198, 199, 202,
    203, 204, 207, 208
  };

  static const unsigned char _date_parser_trans_keys[] = {
    70u, 77u, 83u, 84u, 87u, 114u, 105u, 32u,
    44u, 100u, 65u, 68u, 70u, 74u, 77u, 78u,
    79u, 83u, 112u, 117u, 114u, 32u, 32u, 48u,
    57u, 48u, 57u, 32u, 48u, 57u, 48u, 57u,
    58u, 48u, 57u, 48u, 57u, 58u, 48u, 57u,
    48u, 57u, 32u, 48u, 57u, 48u, 57u, 48u,
    57u, 48u, 57u, 48u, 57u, 103u, 101u, 99u,
    32u, 101u, 98u, 97u, 117u, 110u, 108u, 110u,
    97u, 114u, 121u, 111u, 118u, 99u, 116u, 101u,
    112u, 32u, 48u, 57u, 48u, 57u, 32u, 65u,
    68u, 70u, 74u, 77u, 78u, 79u, 83u, 112u,
    117u, 114u, 32u, 48u, 57u, 48u, 57u, 48u,
    57u, 48u, 57u, 32u, 48u, 57u, 48u, 57u,
    58u, 48u, 57u, 48u, 57u, 58u, 48u, 57u,
    48u, 57u, 32u, 71u, 77u, 84u, 103u, 101u,
    99u, 32u, 101u, 98u, 97u, 117u, 110u, 108u,
    110u, 97u, 114u, 121u, 111u, 118u, 99u, 116u,
    101u, 112u, 97u, 121u, 44u, 32u, 48u, 57u,
    48u, 57u, 45u, 65u, 68u, 70u, 74u, 77u,
    78u, 79u, 83u, 112u, 117u, 114u, 45u, 48u,
    57u, 103u, 101u, 99u, 45u, 101u, 98u, 97u,
    117u, 110u, 108u, 110u, 97u, 114u, 121u, 111u,
    118u, 99u, 116u, 101u, 112u, 111u, 110u, 97u,
    117u, 116u, 32u, 44u, 117u, 114u, 100u, 104u,
    117u, 117u, 32u, 44u, 114u, 115u, 101u, 32u,
    44u, 115u, 101u, 100u, 32u, 44u, 110u, 101u,
    0
  };

  static const char _date_parser_single_lengths[] = {
    0, 5, 1, 1, 3, 8, 2, 1,
    1, 1, 0, 1, 0, 0, 1, 0,
    0, 1, 0, 0, 1, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 1,
    2, 1, 2, 1, 2, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 1, 8,
    2, 1, 1, 0, 0, 0, 0, 1,
    0, 0, 1, 0, 0, 1, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 2, 1, 2, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 1, 8, 2, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 2,
    1, 2, 1, 2, 1, 1, 1, 1,
    1, 1, 1, 1, 2, 1, 3, 1,
    1, 2, 1, 3, 1, 1, 3, 1,
    1, 3, 1, 0
  };

  static const char _date_parser_range_lengths[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 0, 1, 1, 0, 1,
    1, 0, 1, 1, 0, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 0,
    1, 1, 0, 1, 1, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0
  };

  static const short _date_parser_index_offsets[] = {
    0, 0, 6, 8, 10, 14, 23, 26,
    28, 30, 33, 35, 37, 39, 41, 43,
    45, 47, 49, 51, 53, 55, 57, 59,
    61, 63, 65, 67, 69, 71, 73, 75,
    77, 80, 82, 85, 87, 90, 92, 94,
    96, 98, 100, 102, 104, 106, 108, 110,
    119, 122, 124, 126, 128, 130, 132, 134,
    136, 138, 140, 142, 144, 146, 148, 150,
    152, 154, 156, 158, 160, 162, 164, 166,
    168, 170, 172, 175, 177, 180, 182, 185,
    187, 189, 191, 193, 195, 197, 199, 201,
    203, 205, 207, 209, 211, 220, 223, 225,
    227, 229, 231, 233, 235, 237, 239, 241,
    244, 246, 249, 251, 254, 256, 258, 260,
    262, 264, 266, 268, 270, 273, 275, 279,
    281, 283, 286, 288, 292, 294, 296, 300,
    302, 304, 308, 310
  };

  static const unsigned char _date_parser_indicies[] = {
    1, 2, 3, 4, 5, 0, 6, 0,
    7, 0, 8, 9, 10, 0, 11, 12,
    13, 14, 15, 16, 17, 18, 0, 19,
    20, 0, 21, 0, 22, 0, 23, 24,
    0, 25, 0, 26, 0, 27, 0, 28,
    0, 29, 0, 30, 0, 31, 0, 32,
    0, 33, 0, 34, 0, 35, 0, 36,
    0, 37, 0, 38, 0, 39, 0, 40,
    0, 21, 0, 41, 0, 42, 0, 43,
    0, 44, 0, 21, 0, 45, 46, 0,
    21, 0, 21, 21, 0, 47, 0, 21,
    21, 0, 48, 0, 21, 0, 49, 0,
    21, 0, 50, 0, 21, 0, 51, 0,
    52, 0, 53, 0, 54, 0, 55, 56,
    57, 58, 59, 60, 61, 62, 0, 63,
    64, 0, 65, 0, 66, 0, 67, 0,
    68, 0, 69, 0, 70, 0, 71, 0,
    72, 0, 73, 0, 74, 0, 75, 0,
    76, 0, 77, 0, 78, 0, 79, 0,
    80, 0, 81, 0, 82, 0, 39, 0,
    65, 0, 83, 0, 84, 0, 85, 0,
    86, 0, 65, 0, 87, 88, 0, 65,
    0, 65, 65, 0, 89, 0, 65, 65,
    0, 90, 0, 65, 0, 91, 0, 65,
    0, 92, 0, 65, 0, 93, 0, 94,
    0, 95, 0, 96, 0, 97, 0, 98,
    0, 99, 0, 100, 101, 102, 103, 104,
    105, 106, 107, 0, 108, 109, 0, 110,
    0, 111, 0, 112, 0, 110, 0, 113,
    0, 114, 0, 115, 0, 116, 0, 110,
    0, 117, 118, 0, 110, 0, 110, 110,
    0, 119, 0, 110, 110, 0, 120, 0,
    110, 0, 121, 0, 110, 0, 122, 0,
    110, 0, 123, 0, 7, 0, 124, 123,
    0, 125, 0, 8, 9, 126, 0, 127,
    0, 10, 0, 128, 129, 0, 130, 0,
    8, 9, 131, 0, 127, 0, 132, 0,
    8, 9, 127, 0, 133, 0, 134, 0,
    8, 9, 135, 0, 131, 0, 0, 0
  };

  static const unsigned char _date_parser_trans_targs[] = {
    0, 2, 114, 116, 121, 127, 3, 4,
    5, 43, 85, 6, 27, 30, 32, 35,
    37, 39, 41, 7, 26, 8, 9, 10,
    25, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, 131,
    11, 28, 29, 9, 31, 33, 34, 36,
    38, 40, 42, 44, 45, 46, 47, 48,
    69, 72, 74, 77, 79, 81, 83, 49,
    68, 50, 51, 52, 53, 54, 55, 56,
    57, 58, 59, 60, 61, 62, 63, 64,
    65, 66, 67, 70, 71, 51, 73, 75,
    76, 78, 80, 82, 84, 86, 87, 88,
    89, 90, 91, 92, 93, 98, 101, 103,
    106, 108, 110, 112, 94, 97, 95, 96,
    54, 99, 100, 96, 102, 104, 105, 107,
    109, 111, 113, 115, 117, 118, 119, 120,
    122, 125, 123, 124, 126, 128, 129, 130
  };

  static const char _date_parser_trans_actions[] = {
    17, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    9, 7, 0, 1, 0, 0, 3, 0,
    0, 5, 0, 0, 15, 0, 0, 0,
    0, 0, 0, 11, 0, 0, 0, 0,
    0, 0, 0, 0, 9, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 15, 0, 0, 0, 0,
    1, 0, 0, 3, 0, 0, 5, 0,
    0, 0, 0, 0, 0, 11, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 9, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    13, 0, 0, 11, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  };

  static const char _date_parser_eof_actions[] = {
    0, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 0
  };

  static const int date_parser_start = 1;
  static const int date_parser_first_final = 131;
  static const int date_parser_error = 0;

  static const int date_parser_en_main = 1;


  /* #line 800 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  {
    cs = date_parser_start;
  }

  /* #line 803 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  {
    int _klen;
    unsigned int _trans;
    const char* _acts;
    unsigned int _nacts;
    const unsigned char* _keys;

    if (p == pe) {
      goto _test_eof;
    }
    if (cs == 0) {
      goto _out;
    }
_resume:
    _keys = _date_parser_trans_keys + _date_parser_key_offsets[cs];
    _trans = _date_parser_index_offsets[cs];

    _klen = _date_parser_single_lengths[cs];
    if (_klen > 0) {
      const unsigned char* _lower = _keys;
      const unsigned char* _mid;
      const unsigned char* _upper = _keys + _klen - 1;
      while (1) {
        if (_upper < _lower) {
          break;
        }

        _mid = _lower + ((_upper - _lower) >> 1);
        if ((*p) < *_mid) {
          _upper = _mid - 1;
        } else if ((*p) > *_mid) {
          _lower = _mid + 1;
        } else {
          _trans += (_mid - _keys);
          goto _match;
        }
      }
      _keys += _klen;
      _trans += _klen;
    }

    _klen = _date_parser_range_lengths[cs];
    if (_klen > 0) {
      const unsigned char* _lower = _keys;
      const unsigned char* _mid;
      const unsigned char* _upper = _keys + (_klen << 1) - 2;
      while (1) {
        if (_upper < _lower) {
          break;
        }

        _mid = _lower + (((_upper - _lower) >> 1) & ~1);
        if ((*p) < _mid[0]) {
          _upper = _mid - 2;
        } else if ((*p) > _mid[1]) {
          _lower = _mid + 2;
        } else {
          _trans += ((_mid - _keys) >> 1);
          goto _match;
        }
      }
      _trans += _klen;
    }

_match:
    _trans = _date_parser_indicies[_trans];
    cs = _date_parser_trans_targs[_trans];

    if (_date_parser_trans_actions[_trans] == 0) {
      goto _again;
    }

    _acts = _date_parser_actions + _date_parser_trans_actions[_trans];
    _nacts = (unsigned int) * _acts++;
    while (_nacts-- > 0) {
      switch (*_acts++) {
      case 0:
        /* #line 35 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        hour = atoi(p);
      }
      break;
      case 1:
        /* #line 36 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        minute = atoi(p);
      }
      break;
      case 2:
        /* #line 37 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        second = atoi(p);
      }
      break;
      case 3:
        /* #line 39 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        day = atoi(p);
      }
      break;
      case 4:
        /* #line 40 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        day = atoi(p);
      }
      break;
      case 5:
        /* #line 47 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        switch (*(p - 1)) {
        case 'b':
          month = 2;
          break;
        case 'c':
          month = 12;
          break;
        case 'l':
          month = 7;
          break;
        case 'g':
          month = 8;
          break;
        case 'n': {
          switch ((*p - 2)) {
          case 'a':
            month = 1;
            break;
          case 'u':
            month = 6;
            break;
          }
        }
        break;
        case 'p':
          month = 9;
          break;
        case 'r': {
          switch (*(p - 2)) {
          case 'a':
            month = 3;
            break;
          case 'p':
            month = 4;
            break;
          }
        }
        break;
        case 't':
          month = 10;
          break;
        case 'v':
          month = 11;
          break;
        case 'y':
          month = 5;
          break;
        }
      }
      break;
      case 6:
        /* #line 78 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        year = atoi(p);
        year += (year < 50 ? 2000 : 1900);
      }
      break;
      case 7:
        /* #line 79 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        year = atoi(p);
      }
      break;
      case 8:
        /* #line 226 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */
      {
        return DateTime::INVALID_DATE_TIME;
      }
      break;
      /* #line 933 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
      }
    }

_again:
    if (cs == 0) {
      goto _out;
    }
    if (++p != pe) {
      goto _resume;
    }
_test_eof:
    {}
    if (p == eof) {
      const char* __acts = _date_parser_actions + _date_parser_eof_actions[cs];
      unsigned int __nacts = (unsigned int) * __acts++;
      while (__nacts-- > 0) {
        switch (*__acts++) {
        case 8:
          /* #line 226 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */
        {
          return DateTime::INVALID_DATE_TIME;
        }
        break;
        /* #line 951 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
        }
      }
    }

_out:
    {}
  }

  /* #line 231 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */


  if (cs != date_parser_error) {
    if (year < 100) {
      year += 2000;
    }
    year -= 1900;
    return DateTime(second, minute, hour, day, month - 1, year, false);
  } else {
    return DateTime::INVALID_DATE_TIME;
  }
}

bool
HTTPMessageParser::parse_field(
  const char* ps,
  const char* pe,
  const iovec& in_field_name,
  iovec& out_field_value
) {
  int cs;
  char* p = const_cast<char*>(ps);

  iovec field_name = {0, 0}, field_value = {0, 0};

  // Don't look for the trailing CRLF before the body,
  // since it may not be present yet.

  /* #line 957 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  static const char _field_parser_actions[] = {
    0, 1, 0, 1, 1, 1, 2, 1,
    3, 1, 4, 2, 2, 3
  };

  static const char _field_parser_key_offsets[] = {
    0, 0, 15, 20, 21, 25
  };

  static const unsigned char _field_parser_trans_keys[] = {
    58u, 124u, 126u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 13u,
    32u, 127u, 0u, 31u, 10u, 13u, 127u, 0u,
    31u, 124u, 126u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 0
  };

  static const char _field_parser_single_lengths[] = {
    0, 3, 3, 1, 2, 2
  };

  static const char _field_parser_range_lengths[] = {
    0, 6, 1, 0, 1, 6
  };

  static const char _field_parser_index_offsets[] = {
    0, 0, 10, 15, 17, 21
  };

  static const char _field_parser_indicies[] = {
    2, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 3, 4, 1, 1, 5, 6,
    1, 7, 1, 1, 8, 9, 9, 9,
    9, 9, 9, 9, 9, 1, 0
  };

  static const char _field_parser_trans_targs[] = {
    1, 0, 2, 3, 2, 4, 5, 3,
    4, 1
  };

  static const char _field_parser_trans_actions[] = {
    0, 0, 3, 11, 5, 5, 9, 7,
    0, 1
  };

  static const int field_parser_start = 5;
  static const int field_parser_first_final = 5;
  static const int field_parser_error = 0;

  static const int field_parser_en_main = 5;


  /* #line 1009 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  {
    cs = field_parser_start;
  }

  /* #line 1012 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  {
    int _klen;
    unsigned int _trans;
    const char* _acts;
    unsigned int _nacts;
    const unsigned char* _keys;

    if (p == pe) {
      goto _test_eof;
    }
    if (cs == 0) {
      goto _out;
    }
_resume:
    _keys = _field_parser_trans_keys + _field_parser_key_offsets[cs];
    _trans = _field_parser_index_offsets[cs];

    _klen = _field_parser_single_lengths[cs];
    if (_klen > 0) {
      const unsigned char* _lower = _keys;
      const unsigned char* _mid;
      const unsigned char* _upper = _keys + _klen - 1;
      while (1) {
        if (_upper < _lower) {
          break;
        }

        _mid = _lower + ((_upper - _lower) >> 1);
        if ((*p) < *_mid) {
          _upper = _mid - 1;
        } else if ((*p) > *_mid) {
          _lower = _mid + 1;
        } else {
          _trans += (_mid - _keys);
          goto _match;
        }
      }
      _keys += _klen;
      _trans += _klen;
    }

    _klen = _field_parser_range_lengths[cs];
    if (_klen > 0) {
      const unsigned char* _lower = _keys;
      const unsigned char* _mid;
      const unsigned char* _upper = _keys + (_klen << 1) - 2;
      while (1) {
        if (_upper < _lower) {
          break;
        }

        _mid = _lower + (((_upper - _lower) >> 1) & ~1);
        if ((*p) < _mid[0]) {
          _upper = _mid - 2;
        } else if ((*p) > _mid[1]) {
          _lower = _mid + 2;
        } else {
          _trans += ((_mid - _keys) >> 1);
          goto _match;
        }
      }
      _trans += _klen;
    }

_match:
    _trans = _field_parser_indicies[_trans];
    cs = _field_parser_trans_targs[_trans];

    if (_field_parser_trans_actions[_trans] == 0) {
      goto _again;
    }

    _acts = _field_parser_actions + _field_parser_trans_actions[_trans];
    _nacts = (unsigned int) * _acts++;
    while (_nacts-- > 0) {
      switch (*_acts++) {
      case 0:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_name.iov_base = p;
      }
      break;
      case 1:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_name.iov_len = p - static_cast<char*>(field_name.iov_base);
      }
      break;
      case 2:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_value.iov_base = p;
      }
      break;
      case 3:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_value.iov_len = p - static_cast<char*>(field_value.iov_base);
      }
      break;
      case 4:
        /* #line 262 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */
      {
        if (
          field_name.iov_len == in_field_name.iov_len
          &&
          memcmp(
            field_name.iov_base,
            in_field_name.iov_base,
            in_field_name.iov_len
          ) == 0
        ) {
          out_field_value = field_value;
          return true;
        }
      }
      break;
      /* #line 1113 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
      }
    }

_again:
    if (cs == 0) {
      goto _out;
    }
    if (++p != pe) {
      goto _resume;
    }
_test_eof:
    {}
_out:
    {}
  }

  /* #line 281 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */


  return false;
}

void
HTTPMessageParser::parse_fields(
  const char* ps,
  const char* pe,
  vector< std::pair<iovec, iovec> >& fields
) {
  int cs;
  char* p = const_cast<char*>(ps);

  iovec field_name = {0, 0}, field_value = {0, 0};

  // Don't look for the trailing CRLF before the body,
  // since it may not be present yet.

  /* #line 1124 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  static const char _static_fields_parser_actions[] = {
    0, 1, 0, 1, 1, 1, 2, 1,
    3, 1, 4, 2, 2, 3
  };

  static const char _static_fields_parser_key_offsets[] = {
    0, 0, 15, 20, 21, 25
  };

  static const unsigned char _static_fields_parser_trans_keys[] = {
    58u, 124u, 126u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 13u,
    32u, 127u, 0u, 31u, 10u, 13u, 127u, 0u,
    31u, 124u, 126u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 0
  };

  static const char _static_fields_parser_single_lengths[] = {
    0, 3, 3, 1, 2, 2
  };

  static const char _static_fields_parser_range_lengths[] = {
    0, 6, 1, 0, 1, 6
  };

  static const char _static_fields_parser_index_offsets[] = {
    0, 0, 10, 15, 17, 21
  };

  static const char _static_fields_parser_indicies[] = {
    2, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 3, 4, 1, 1, 5, 6,
    1, 7, 1, 1, 8, 9, 9, 9,
    9, 9, 9, 9, 9, 1, 0
  };

  static const char _static_fields_parser_trans_targs[] = {
    1, 0, 2, 3, 2, 4, 5, 3,
    4, 1
  };

  static const char _static_fields_parser_trans_actions[] = {
    0, 0, 3, 11, 5, 5, 9, 7,
    0, 1
  };

  static const int static_fields_parser_start = 5;
  static const int static_fields_parser_first_final = 5;
  static const int static_fields_parser_error = 0;

  static const int static_fields_parser_en_main = 5;


  /* #line 1176 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  {
    cs = static_fields_parser_start;
  }

  /* #line 1179 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  {
    int _klen;
    unsigned int _trans;
    const char* _acts;
    unsigned int _nacts;
    const unsigned char* _keys;

    if (p == pe) {
      goto _test_eof;
    }
    if (cs == 0) {
      goto _out;
    }
_resume:
    _keys = _static_fields_parser_trans_keys + _static_fields_parser_key_offsets[cs];
    _trans = _static_fields_parser_index_offsets[cs];

    _klen = _static_fields_parser_single_lengths[cs];
    if (_klen > 0) {
      const unsigned char* _lower = _keys;
      const unsigned char* _mid;
      const unsigned char* _upper = _keys + _klen - 1;
      while (1) {
        if (_upper < _lower) {
          break;
        }

        _mid = _lower + ((_upper - _lower) >> 1);
        if ((*p) < *_mid) {
          _upper = _mid - 1;
        } else if ((*p) > *_mid) {
          _lower = _mid + 1;
        } else {
          _trans += (_mid - _keys);
          goto _match;
        }
      }
      _keys += _klen;
      _trans += _klen;
    }

    _klen = _static_fields_parser_range_lengths[cs];
    if (_klen > 0) {
      const unsigned char* _lower = _keys;
      const unsigned char* _mid;
      const unsigned char* _upper = _keys + (_klen << 1) - 2;
      while (1) {
        if (_upper < _lower) {
          break;
        }

        _mid = _lower + (((_upper - _lower) >> 1) & ~1);
        if ((*p) < _mid[0]) {
          _upper = _mid - 2;
        } else if ((*p) > _mid[1]) {
          _lower = _mid + 2;
        } else {
          _trans += ((_mid - _keys) >> 1);
          goto _match;
        }
      }
      _trans += _klen;
    }

_match:
    _trans = _static_fields_parser_indicies[_trans];
    cs = _static_fields_parser_trans_targs[_trans];

    if (_static_fields_parser_trans_actions[_trans] == 0) {
      goto _again;
    }

    _acts = _static_fields_parser_actions + _static_fields_parser_trans_actions[_trans];
    _nacts = (unsigned int) * _acts++;
    while (_nacts-- > 0) {
      switch (*_acts++) {
      case 0:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_name.iov_base = p;
      }
      break;
      case 1:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_name.iov_len = p - static_cast<char*>(field_name.iov_base);
      }
      break;
      case 2:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_value.iov_base = p;
      }
      break;
      case 3:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_value.iov_len = p - static_cast<char*>(field_value.iov_base);
      }
      break;
      case 4:
        /* #line 306 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */
      {
        fields.push_back(std::make_pair(field_name, field_value));
      }
      break;
      /* #line 1267 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
      }
    }

_again:
    if (cs == 0) {
      goto _out;
    }
    if (++p != pe) {
      goto _resume;
    }
_test_eof:
    {}
_out:
    {}
  }

  /* #line 312 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */

}

bool
HTTPMessageParser::parse_fields(
  uint16_t& fields_offset,
  size_t& content_length
) {
  fields_offset = static_cast<uint16_t>(p - ps);

  content_length = 0;

  int cs;
  iovec field_name = {0, 0}, field_value = {0, 0};


  /* #line 1278 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  static const char _fields_parser_actions[] = {
    0, 1, 0, 1, 1, 1, 2, 1,
    3, 1, 4, 1, 5, 1, 6, 2,
    2, 3, 2, 4, 0
  };

  static const char _fields_parser_key_offsets[] = {
    0, 0, 15, 16, 31, 36, 37, 52,
    56
  };

  static const unsigned char _fields_parser_trans_keys[] = {
    13u, 124u, 126u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 10u,
    58u, 124u, 126u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 13u,
    32u, 127u, 0u, 31u, 10u, 13u, 124u, 126u,
    33u, 38u, 42u, 43u, 45u, 46u, 48u, 57u,
    65u, 90u, 94u, 122u, 13u, 127u, 0u, 31u,
    0
  };

  static const char _fields_parser_single_lengths[] = {
    0, 3, 1, 3, 3, 1, 3, 2,
    0
  };

  static const char _fields_parser_range_lengths[] = {
    0, 6, 0, 6, 1, 0, 6, 1,
    0
  };

  static const char _fields_parser_index_offsets[] = {
    0, 0, 10, 12, 22, 27, 29, 39,
    43
  };

  static const char _fields_parser_indicies[] = {
    1, 2, 2, 2, 2, 2, 2, 2,
    2, 0, 3, 0, 6, 4, 4, 4,
    4, 4, 4, 4, 4, 5, 7, 8,
    5, 5, 9, 10, 5, 11, 12, 12,
    12, 12, 12, 12, 12, 12, 0, 13,
    5, 5, 14, 0, 0
  };

  static const char _fields_parser_trans_targs[] = {
    0, 2, 3, 8, 3, 0, 4, 5,
    4, 7, 6, 2, 3, 5, 7
  };

  static const char _fields_parser_trans_actions[] = {
    13, 0, 1, 11, 0, 0, 3, 15,
    5, 5, 0, 9, 18, 7, 0
  };

  static const char _fields_parser_eof_actions[] = {
    0, 13, 13, 0, 0, 0, 13, 0,
    0
  };

  static const int fields_parser_start = 1;
  static const int fields_parser_first_final = 8;
  static const int fields_parser_error = 0;

  static const int fields_parser_en_main = 1;


  /* #line 1345 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  {
    cs = fields_parser_start;
  }

  /* #line 1348 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
  {
    int _klen;
    unsigned int _trans;
    const char* _acts;
    unsigned int _nacts;
    const unsigned char* _keys;

    if (cs == 0) {
      goto _out;
    }
_resume:
    _keys = _fields_parser_trans_keys + _fields_parser_key_offsets[cs];
    _trans = _fields_parser_index_offsets[cs];

    _klen = _fields_parser_single_lengths[cs];
    if (_klen > 0) {
      const unsigned char* _lower = _keys;
      const unsigned char* _mid;
      const unsigned char* _upper = _keys + _klen - 1;
      while (1) {
        if (_upper < _lower) {
          break;
        }

        _mid = _lower + ((_upper - _lower) >> 1);
        if ((*p) < *_mid) {
          _upper = _mid - 1;
        } else if ((*p) > *_mid) {
          _lower = _mid + 1;
        } else {
          _trans += (_mid - _keys);
          goto _match;
        }
      }
      _keys += _klen;
      _trans += _klen;
    }

    _klen = _fields_parser_range_lengths[cs];
    if (_klen > 0) {
      const unsigned char* _lower = _keys;
      const unsigned char* _mid;
      const unsigned char* _upper = _keys + (_klen << 1) - 2;
      while (1) {
        if (_upper < _lower) {
          break;
        }

        _mid = _lower + (((_upper - _lower) >> 1) & ~1);
        if ((*p) < _mid[0]) {
          _upper = _mid - 2;
        } else if ((*p) > _mid[1]) {
          _lower = _mid + 2;
        } else {
          _trans += ((_mid - _keys) >> 1);
          goto _match;
        }
      }
      _trans += _klen;
    }

_match:
    _trans = _fields_parser_indicies[_trans];
    cs = _fields_parser_trans_targs[_trans];

    if (_fields_parser_trans_actions[_trans] == 0) {
      goto _again;
    }

    _acts = _fields_parser_actions + _fields_parser_trans_actions[_trans];
    _nacts = (unsigned int) * _acts++;
    while (_nacts-- > 0) {
      switch (*_acts++) {
      case 0:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_name.iov_base = p;
      }
      break;
      case 1:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_name.iov_len = p - static_cast<char*>(field_name.iov_base);
      }
      break;
      case 2:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_value.iov_base = p;
      }
      break;
      case 3:
        /* #line 83 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\rfc2616.rl" */
      {
        field_value.iov_len = p - static_cast<char*>(field_value.iov_base);
      }
      break;
      case 4:
        /* #line 334 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */
      {
        parse_content_length_field(
          field_name,
          field_value,
          content_length
        );
      }
      break;
      case 5:
        /* #line 342 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */
      { {
          p++;
          goto _out;
        }
      }
      break;
      case 6:
        /* #line 343 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */
      {
        return false;
      }
      break;
      /* #line 1446 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
      }
    }

_again:
    if (cs == 0) {
      goto _out;
    }
    p += 1;
    goto _resume;
    if (p == eof) {
      const char* __acts = _fields_parser_actions + _fields_parser_eof_actions[cs];
      unsigned int __nacts = (unsigned int) * __acts++;
      while (__nacts-- > 0) {
        switch (*__acts++) {
        case 6:
          /* #line 343 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */
        {
          return false;
        }
        break;
        /* #line 1463 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.cpp" */
        }
      }
    }

_out:
    {}
  }

  /* #line 348 "c:\\Users\\minorg\\projects\\yield\\src\\yield\\http\\http_message_parser.rl" */


  return cs != fields_parser_error;
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
