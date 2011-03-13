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

/* #line 1 "src\\yield\\http\\http_message_parser.rl" */
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
#include "yield/page.hpp"
#include "yield/http/http_body_chunk.hpp"
#include "yield/http/http_message_parser.hpp"
#include "yield/http/http_request.hpp"

#include <stdlib.h> // For strtol


#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4702)
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
  : buffer(*new Page(buffer)) {
  debug_assert_false(buffer.empty());

  ps = p = this->buffer;
  eof = ps + this->buffer.size();
}

HTTPMessageParser::~HTTPMessageParser() {
  Buffer::dec_ref(buffer);
}

bool HTTPMessageParser::parse_body(size_t content_length, void*& body) {
  if
  (
    content_length == 0
    ||
    content_length == HTTPRequest::CONTENT_LENGTH_CHUNKED
  ) {
    body = NULL;
    return true;
  } else if (static_cast<size_t>(eof - p) >= content_length) {
    body = p;
    p += content_length;
    return true;
  } else
    return false;
}

Object* HTTPMessageParser::parse_body_chunk() {
  const char* chunk_data_p = NULL;
  size_t chunk_size = 0;
  const char* chunk_size_p = NULL;
  int cs;
  iovec field_name = { 0 }, field_value = { 0 };
  size_t seen_chunk_size = 0;

  ps = p;


  /* #line 2 "src\\yield\\http\\http_message_parser.cpp" */
  static const char _chunk_parser_actions[] = {
    0, 1, 0, 1, 1, 1, 2, 1,
    3, 1, 4, 1, 5, 1, 6, 1,
    8, 1, 9, 2, 0, 6, 2, 7,
    5
  };

  static const char _chunk_parser_cond_offsets[] = {
    0, 0, 0, 0, 0, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 2, 3,
    4, 5, 6, 7, 8, 8, 8, 9,
    9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 10
  };

  static const char _chunk_parser_cond_lengths[] = {
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1
  };

  static const short _chunk_parser_cond_keys[] = {
    0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u,
    0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u,
    0u, 255u, 0u, 255u, 0u, 255u, 0
  };

  static const char _chunk_parser_cond_spaces[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0
  };

  static const short _chunk_parser_key_offsets[] = {
    0, 0, 7, 15, 16, 54, 55, 70,
    74, 79, 80, 81, 82, 86, 90, 96,
    136, 149, 162, 168, 179, 187, 188, 192,
    206, 223, 238, 254, 275, 280, 282, 284,
    291, 310, 332, 350, 364, 381, 396, 412,
    433, 438, 440, 442, 449, 468, 490, 508,
    508, 509, 513
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
    48u, 57u, 65u, 90u, 94u, 122u, 32u, 127u,
    0u, 31u, 13u, 32u, 127u, 0u, 31u, 10u,
    13u, 10u, 13u, 127u, 0u, 31u, 269u, 525u,
    512u, 767u, 266u, 269u, 522u, 525u, 512u, 767u,
    269u, 314u, 380u, 382u, 525u, 556u, 559u, 570u,
    635u, 637u, 289u, 294u, 298u, 299u, 301u, 302u,
    304u, 313u, 321u, 346u, 350u, 378u, 512u, 544u,
    545u, 550u, 551u, 553u, 554u, 569u, 571u, 576u,
    577u, 602u, 603u, 605u, 606u, 638u, 639u, 767u,
    269u, 288u, 525u, 544u, 639u, 289u, 382u, 384u,
    511u, 512u, 543u, 545u, 767u, 269u, 288u, 525u,
    544u, 639u, 289u, 382u, 384u, 511u, 512u, 543u,
    545u, 767u, 266u, 269u, 522u, 525u, 512u, 767u,
    269u, 525u, 639u, 288u, 382u, 384u, 511u, 512u,
    543u, 544u, 767u, 13u, 59u, 48u, 57u, 65u,
    70u, 97u, 102u, 10u, 269u, 525u, 512u, 767u,
    124u, 126u, 33u, 38u, 42u, 43u, 45u, 46u,
    48u, 57u, 65u, 90u, 94u, 122u, 13u, 59u,
    61u, 124u, 126u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 34u,
    124u, 126u, 33u, 38u, 42u, 43u, 45u, 46u,
    48u, 57u, 65u, 90u, 94u, 122u, 13u, 59u,
    124u, 126u, 33u, 38u, 42u, 43u, 45u, 46u,
    48u, 57u, 65u, 90u, 94u, 122u, 13u, 34u,
    59u, 92u, 124u, 126u, 127u, 0u, 31u, 33u,
    38u, 42u, 43u, 45u, 46u, 48u, 57u, 65u,
    90u, 94u, 122u, 34u, 92u, 127u, 0u, 31u,
    13u, 59u, 34u, 92u, 13u, 34u, 59u, 92u,
    127u, 0u, 31u, 34u, 92u, 124u, 126u, 127u,
    0u, 31u, 33u, 38u, 42u, 43u, 45u, 46u,
    48u, 57u, 65u, 90u, 94u, 122u, 13u, 34u,
    59u, 61u, 92u, 124u, 126u, 127u, 0u, 31u,
    33u, 38u, 42u, 43u, 45u, 46u, 48u, 57u,
    65u, 90u, 94u, 122u, 92u, 124u, 126u, 127u,
    0u, 31u, 33u, 38u, 42u, 43u, 45u, 46u,
    48u, 57u, 65u, 90u, 94u, 122u, 124u, 126u,
    33u, 38u, 42u, 43u, 45u, 46u, 48u, 57u,
    65u, 90u, 94u, 122u, 13u, 59u, 61u, 124u,
    126u, 33u, 38u, 42u, 43u, 45u, 46u, 48u,
    57u, 65u, 90u, 94u, 122u, 34u, 124u, 126u,
    33u, 38u, 42u, 43u, 45u, 46u, 48u, 57u,
    65u, 90u, 94u, 122u, 13u, 59u, 124u, 126u,
    33u, 38u, 42u, 43u, 45u, 46u, 48u, 57u,
    65u, 90u, 94u, 122u, 13u, 34u, 59u, 92u,
    124u, 126u, 127u, 0u, 31u, 33u, 38u, 42u,
    43u, 45u, 46u, 48u, 57u, 65u, 90u, 94u,
    122u, 34u, 92u, 127u, 0u, 31u, 13u, 59u,
    34u, 92u, 13u, 34u, 59u, 92u, 127u, 0u,
    31u, 34u, 92u, 124u, 126u, 127u, 0u, 31u,
    33u, 38u, 42u, 43u, 45u, 46u, 48u, 57u,
    65u, 90u, 94u, 122u, 13u, 34u, 59u, 61u,
    92u, 124u, 126u, 127u, 0u, 31u, 33u, 38u,
    42u, 43u, 45u, 46u, 48u, 57u, 65u, 90u,
    94u, 122u, 92u, 124u, 126u, 127u, 0u, 31u,
    33u, 38u, 42u, 43u, 45u, 46u, 48u, 57u,
    65u, 90u, 94u, 122u, 13u, 269u, 525u, 512u,
    767u, 269u, 525u, 512u, 767u, 0
  };

  static const char _chunk_parser_single_lengths[] = {
    0, 1, 2, 1, 8, 1, 3, 2,
    3, 1, 1, 1, 2, 2, 4, 10,
    5, 5, 4, 3, 2, 1, 2, 2,
    5, 3, 4, 7, 3, 2, 2, 5,
    5, 8, 4, 2, 5, 3, 4, 7,
    3, 2, 2, 5, 5, 8, 4, 0,
    1, 2, 2
  };

  static const char _chunk_parser_range_lengths[] = {
    0, 3, 3, 0, 15, 0, 6, 1,
    1, 0, 0, 0, 1, 1, 1, 15,
    4, 4, 1, 4, 3, 0, 1, 6,
    6, 6, 6, 7, 1, 0, 0, 1,
    7, 7, 7, 6, 6, 6, 6, 7,
    1, 0, 0, 1, 7, 7, 7, 0,
    0, 1, 1
  };

  static const short _chunk_parser_index_offsets[] = {
    0, 0, 5, 11, 13, 37, 39, 49,
    53, 58, 60, 62, 64, 68, 72, 78,
    104, 114, 124, 130, 138, 144, 146, 150,
    159, 171, 181, 192, 207, 212, 215, 218,
    225, 238, 254, 266, 275, 287, 297, 308,
    323, 328, 331, 334, 341, 354, 370, 382,
    383, 385, 389
  };

  static const char _chunk_parser_indicies[] = {
    1, 2, 2, 2, 0, 3, 5, 4,
    4, 4, 0, 6, 0, 7, 8, 8,
    10, 9, 9, 9, 9, 8, 8, 8,
    8, 8, 8, 9, 11, 9, 11, 9,
    11, 9, 11, 9, 0, 12, 0, 14,
    13, 13, 13, 13, 13, 13, 13, 13,
    0, 15, 0, 0, 16, 17, 15, 0,
    0, 16, 18, 0, 19, 0, 20, 0,
    17, 0, 0, 21, 22, 24, 23, 0,
    12, 22, 25, 24, 23, 0, 22, 14,
    13, 13, 24, 23, 23, 27, 23, 23,
    13, 13, 13, 13, 13, 13, 23, 26,
    23, 26, 23, 26, 23, 26, 23, 0,
    22, 15, 24, 28, 23, 16, 16, 23,
    29, 0, 30, 15, 31, 28, 23, 16,
    16, 23, 29, 0, 20, 22, 32, 24,
    23, 0, 30, 31, 23, 21, 21, 23,
    33, 0, 34, 35, 4, 4, 4, 0,
    36, 0, 7, 10, 9, 0, 37, 37,
    37, 37, 37, 37, 37, 37, 0, 38,
    39, 40, 37, 37, 37, 37, 37, 37,
    37, 37, 0, 42, 41, 41, 41, 41,
    41, 41, 41, 41, 0, 38, 39, 41,
    41, 41, 41, 41, 41, 41, 41, 0,
    38, 41, 44, 45, 42, 42, 0, 0,
    42, 42, 42, 42, 42, 42, 43, 46,
    45, 0, 0, 43, 38, 39, 0, 47,
    45, 43, 38, 46, 44, 45, 0, 0,
    43, 37, 45, 48, 48, 0, 0, 48,
    48, 48, 48, 48, 48, 43, 38, 37,
    44, 49, 45, 48, 48, 0, 0, 48,
    48, 48, 48, 48, 48, 43, 45, 42,
    42, 0, 0, 42, 42, 42, 42, 42,
    42, 43, 50, 50, 50, 50, 50, 50,
    50, 50, 0, 51, 52, 53, 50, 50,
    50, 50, 50, 50, 50, 50, 0, 55,
    54, 54, 54, 54, 54, 54, 54, 54,
    0, 51, 52, 54, 54, 54, 54, 54,
    54, 54, 54, 0, 51, 54, 57, 58,
    55, 55, 0, 0, 55, 55, 55, 55,
    55, 55, 56, 59, 58, 0, 0, 56,
    51, 52, 0, 60, 58, 56, 51, 59,
    57, 58, 0, 0, 56, 50, 58, 61,
    61, 0, 0, 61, 61, 61, 61, 61,
    61, 56, 51, 50, 57, 62, 58, 61,
    61, 0, 0, 61, 61, 61, 61, 61,
    61, 56, 58, 55, 55, 0, 0, 55,
    55, 55, 55, 55, 55, 56, 0, 19,
    0, 22, 24, 23, 0, 19, 63, 23,
    0, 0
  };

  static const char _chunk_parser_trans_targs[] = {
    0, 2, 20, 3, 20, 35, 4, 5,
    6, 13, 14, 15, 47, 6, 7, 8,
    12, 9, 10, 11, 48, 12, 5, 13,
    14, 49, 15, 16, 17, 19, 11, 18,
    50, 19, 21, 23, 22, 24, 21, 23,
    25, 26, 27, 28, 32, 30, 29, 31,
    33, 34, 36, 3, 35, 37, 38, 39,
    40, 44, 42, 41, 43, 45, 46, 18
  };

  static const char _chunk_parser_trans_actions[] = {
    17, 9, 9, 22, 0, 22, 0, 13,
    1, 13, 13, 19, 15, 0, 3, 5,
    5, 7, 0, 0, 15, 0, 0, 0,
    0, 15, 0, 3, 5, 5, 7, 7,
    15, 0, 11, 11, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  };

  static const char _chunk_parser_eof_actions[] = {
    0, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 17, 17, 17, 0,
    0, 0, 0
  };

  static const int chunk_parser_start = 1;
  static const int chunk_parser_first_final = 47;
  static const int chunk_parser_error = 0;

  static const int chunk_parser_en_main = 1;


  /* #line 239 "src\\yield\\http\\http_message_parser.cpp" */
  {
    cs = chunk_parser_start;
  }

  /* #line 242 "src\\yield\\http\\http_message_parser.cpp" */
  {
    int _klen;
    unsigned int _trans;
    short _widec;
    const char* _acts;
    unsigned int _nacts;
    const short* _keys;

    if (cs == 0)
      goto _out;
_resume:
    _widec = (*p);
    _klen = _chunk_parser_cond_lengths[cs];
    _keys = _chunk_parser_cond_keys + (_chunk_parser_cond_offsets[cs] * 2);
    if (_klen > 0) {
      const short* _lower = _keys;
      const short* _mid;
      const short* _upper = _keys + (_klen << 1) - 2;
      while (1) {
        if (_upper < _lower)
          break;

        _mid = _lower + (((_upper - _lower) >> 1) & ~1);
        if (_widec < _mid[0])
          _upper = _mid - 2;
        else if (_widec > _mid[1])
          _lower = _mid + 2;
        else {
          switch (_chunk_parser_cond_spaces[_chunk_parser_cond_offsets[cs] + ((_mid - _keys) >> 1)]) {
          case 0: {
            _widec = (short)(256u + ((*p) - 0u));
            if (
              /* #line 56 "src\\yield\\http\\chunk.rl" */
              seen_chunk_size++ < chunk_size) _widec += 256;
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
        if (_upper < _lower)
          break;

        _mid = _lower + ((_upper - _lower) >> 1);
        if (_widec < *_mid)
          _upper = _mid - 1;
        else if (_widec > *_mid)
          _lower = _mid + 1;
        else {
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
        if (_upper < _lower)
          break;

        _mid = _lower + (((_upper - _lower) >> 1) & ~1);
        if (_widec < _mid[0])
          _upper = _mid - 2;
        else if (_widec > _mid[1])
          _lower = _mid + 2;
        else {
          _trans += ((_mid - _keys) >> 1);
          goto _match;
        }
      }
      _trans += _klen;
    }

_match:
    _trans = _chunk_parser_indicies[_trans];
    cs = _chunk_parser_trans_targs[_trans];

    if (_chunk_parser_trans_actions[_trans] == 0)
      goto _again;

    _acts = _chunk_parser_actions + _chunk_parser_trans_actions[_trans];
    _nacts = (unsigned int) * _acts++;
    while (_nacts-- > 0) {
      switch (*_acts++) {
      case 0:
        /* #line 31 "src\\yield\\http\\field.rl" */
      {
        field_name.iov_base = p;
      }
      break;
      case 1:
        /* #line 31 "src\\yield\\http\\field.rl" */
      {
        field_name.iov_len = p - static_cast<char*>(field_name.iov_base);
      }
      break;
      case 2:
        /* #line 31 "src\\yield\\http\\field.rl" */
      {
        field_value.iov_base = p;
      }
      break;
      case 3:
        /* #line 31 "src\\yield\\http\\field.rl" */
      {
        field_value.iov_len = p - static_cast<char*>(field_value.iov_base);
      }
      break;
      case 4:
        /* #line 38 "src\\yield\\http\\chunk.rl" */
      {
        chunk_size_p = p;
      }
      break;
      case 5:
        /* #line 40 "src\\yield\\http\\chunk.rl" */
      {
        char* chunk_size_pe = p;
        chunk_size
        = static_cast<size_t>
          (
            strtol(chunk_size_p, &chunk_size_pe, 16)
          );
      }
      break;
      case 6:
        /* #line 57 "src\\yield\\http\\chunk.rl" */
      {
        chunk_data_p = p;
      }
      break;
      case 7:
        /* #line 61 "src\\yield\\http\\chunk.rl" */
      {
        chunk_size = 0;
      }
      break;
      case 8:
        /* #line 101 "src\\yield\\http\\http_message_parser.rl" */
      { {
          p++;
          goto _out;
        }
      }
      break;
      case 9:
        /* #line 102 "src\\yield\\http\\http_message_parser.rl" */
      {
        return NULL;
      }
      break;
      /* #line 381 "src\\yield\\http\\http_message_parser.cpp" */
      }
    }

_again:
    if (cs == 0)
      goto _out;
    p += 1;
    goto _resume;
    if (p == eof) {
      const char* __acts = _chunk_parser_actions + _chunk_parser_eof_actions[cs];
      unsigned int __nacts = (unsigned int) * __acts++;
      while (__nacts-- > 0) {
        switch (*__acts++) {
        case 9:
          /* #line 102 "src\\yield\\http\\http_message_parser.rl" */
        {
          return NULL;
        }
        break;
        /* #line 398 "src\\yield\\http\\http_message_parser.cpp" */
        }
      }
    }

_out:
    {}
  }

  /* #line 107 "src\\yield\\http\\http_message_parser.rl" */


  if (cs != chunk_parser_error) {
    if (chunk_size > 0) {
      // Cut off the chunk size + extension + CRLF before
      // the chunk data and the CRLF after
      return new HTTPBodyChunk(chunk_data_p, p - chunk_data_p - 2);
    } else // Last chunk
      return new HTTPBodyChunk;
  } else if (p == eof && chunk_size != 0)
    return new Page(chunk_size + 2);   // Assumes no trailers..
  else
    return NULL;
}

bool
HTTPMessageParser::parse_fields
(
  OUT uint16_t& fields_offset,
  OUT size_t& content_length
) {
  fields_offset = static_cast<uint16_t>(p - ps);

  content_length = 0;

  int cs;
  iovec field_name = { 0 }, field_value = { 0 };


  /* #line 404 "src\\yield\\http\\http_message_parser.cpp" */
  static const char _fields_parser_actions[] = {
    0, 1, 0, 1, 1, 1, 2, 1,
    3, 1, 4, 1, 5, 1, 6, 2,
    4, 0
  };

  static const char _fields_parser_key_offsets[] = {
    0, 0, 15, 16, 31, 35, 40, 41,
    56, 60
  };

  static const unsigned char _fields_parser_trans_keys[] = {
    13u, 124u, 126u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 10u,
    58u, 124u, 126u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 32u,
    127u, 0u, 31u, 13u, 32u, 127u, 0u, 31u,
    10u, 13u, 124u, 126u, 33u, 38u, 42u, 43u,
    45u, 46u, 48u, 57u, 65u, 90u, 94u, 122u,
    13u, 127u, 0u, 31u, 0
  };

  static const char _fields_parser_single_lengths[] = {
    0, 3, 1, 3, 2, 3, 1, 3,
    2, 0
  };

  static const char _fields_parser_range_lengths[] = {
    0, 6, 0, 6, 1, 1, 0, 6,
    1, 0
  };

  static const char _fields_parser_index_offsets[] = {
    0, 0, 10, 12, 22, 26, 31, 33,
    43, 47
  };

  static const char _fields_parser_indicies[] = {
    1, 2, 2, 2, 2, 2, 2, 2,
    2, 0, 3, 0, 6, 4, 4, 4,
    4, 4, 4, 4, 4, 5, 7, 5,
    5, 8, 9, 7, 5, 5, 8, 10,
    5, 11, 12, 12, 12, 12, 12, 12,
    12, 12, 0, 9, 5, 5, 13, 0,
    0
  };

  static const char _fields_parser_trans_targs[] = {
    0, 2, 3, 9, 3, 0, 4, 5,
    8, 6, 7, 2, 3, 8
  };

  static const char _fields_parser_trans_actions[] = {
    13, 0, 1, 11, 0, 0, 3, 5,
    5, 7, 0, 9, 15, 0
  };

  static const char _fields_parser_eof_actions[] = {
    0, 13, 13, 0, 0, 0, 0, 13,
    0, 0
  };

  static const int fields_parser_start = 1;
  static const int fields_parser_first_final = 9;
  static const int fields_parser_error = 0;

  static const int fields_parser_en_main = 1;


  /* #line 472 "src\\yield\\http\\http_message_parser.cpp" */
  {
    cs = fields_parser_start;
  }

  /* #line 475 "src\\yield\\http\\http_message_parser.cpp" */
  {
    int _klen;
    unsigned int _trans;
    const char* _acts;
    unsigned int _nacts;
    const unsigned char* _keys;

    if (cs == 0)
      goto _out;
_resume:
    _keys = _fields_parser_trans_keys + _fields_parser_key_offsets[cs];
    _trans = _fields_parser_index_offsets[cs];

    _klen = _fields_parser_single_lengths[cs];
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

    _klen = _fields_parser_range_lengths[cs];
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
    _trans = _fields_parser_indicies[_trans];
    cs = _fields_parser_trans_targs[_trans];

    if (_fields_parser_trans_actions[_trans] == 0)
      goto _again;

    _acts = _fields_parser_actions + _fields_parser_trans_actions[_trans];
    _nacts = (unsigned int) * _acts++;
    while (_nacts-- > 0) {
      switch (*_acts++) {
      case 0:
        /* #line 31 "src\\yield\\http\\field.rl" */
      {
        field_name.iov_base = p;
      }
      break;
      case 1:
        /* #line 31 "src\\yield\\http\\field.rl" */
      {
        field_name.iov_len = p - static_cast<char*>(field_name.iov_base);
      }
      break;
      case 2:
        /* #line 31 "src\\yield\\http\\field.rl" */
      {
        field_value.iov_base = p;
      }
      break;
      case 3:
        /* #line 31 "src\\yield\\http\\field.rl" */
      {
        field_value.iov_len = p - static_cast<char*>(field_value.iov_base);
      }
      break;
      case 4:
        /* #line 145 "src\\yield\\http\\http_message_parser.rl" */
      {
        if
        (
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
        } else if
        (
          field_name.iov_len == 17
          &&
          (
            memcmp(field_name.iov_base, "Transfer-Encoding", 17) == 0
            ||
            memcmp(field_name.iov_base, "Transfer-encoding", 17) == 0
          )
          &&
          memcmp(field_value.iov_base, "chunked", 7) == 0
        )
          content_length = HTTPRequest::CONTENT_LENGTH_CHUNKED;
      }
      break;
      case 5:
        /* #line 175 "src\\yield\\http\\http_message_parser.rl" */
      { {
          p++;
          goto _out;
        }
      }
      break;
      case 6:
        /* #line 176 "src\\yield\\http\\http_message_parser.rl" */
      {
        return false;
      }
      break;
      /* #line 595 "src\\yield\\http\\http_message_parser.cpp" */
      }
    }

_again:
    if (cs == 0)
      goto _out;
    p += 1;
    goto _resume;
    if (p == eof) {
      const char* __acts = _fields_parser_actions + _fields_parser_eof_actions[cs];
      unsigned int __nacts = (unsigned int) * __acts++;
      while (__nacts-- > 0) {
        switch (*__acts++) {
        case 6:
          /* #line 176 "src\\yield\\http\\http_message_parser.rl" */
        {
          return false;
        }
        break;
        /* #line 612 "src\\yield\\http\\http_message_parser.cpp" */
        }
      }
    }

_out:
    {}
  }

  /* #line 181 "src\\yield\\http\\http_message_parser.rl" */


  return cs != fields_parser_error;
}
}
}

#ifdef _WIN32
#pragma warning(pop)
#endif
//
