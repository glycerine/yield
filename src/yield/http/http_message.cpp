// yield/http/http_message.cpp

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

/* #line 1 "src\\yield\\http\\http_message.rl" */
// yield/http/http_message.rl

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

#include "yield/config.hpp"
#include "yield/assert.hpp"
#include "yield/page.hpp"
#include "yield/http/http_message.hpp"
#include "yield/http/http_request.hpp"
#include "yield/http/http_response.hpp"

#ifdef _WIN32
#include <Windows.h> // For SYSTEMTIME
#pragma warning(push)
#pragma warning(disable:4702)
#else
#include <stdio.h> // For snprintf
#include <stdlib.h> // For atoi
#endif

namespace yield {
namespace http {
template <class HTTPMessageType>
HTTPMessage<HTTPMessageType>::
HTTPMessage(
  void* body,
  Buffer& buffer,
  size_t content_length,
  uint16_t fields_offset,
  float http_version
)
  : body(body),
    buffer(buffer.inc_ref()),
    content_length(content_length),
    fields_offset(fields_offset),
    http_version(http_version)
{ }

template <class HTTPMessageType>
HTTPMessage<HTTPMessageType>::
HTTPMessage(
  YO_NEW_REF Buffer* body,
  float http_version
)
  : buffer(*new Page),
    http_version(http_version) {
  if (body != NULL) {
    this->body = *body;
    buffer.set_next_buffer(body);
    content_length = body->size();
  } else {
    this->body = NULL;
    content_length = 0;
  }

  fields_offset = 0;
}

template <class HTTPMessageType>
HTTPMessage<HTTPMessageType>::~HTTPMessage() {
  Buffer::dec_ref(buffer);
}

template <class HTTPMessageType>
void HTTPMessage<HTTPMessageType>::finalize() {
  if (!is_finalized()) {
    set_field("Content-Length", 14, content_length);
    buffer.put("\r\n", 2);
  }
}

template <class HTTPMessageType>
DateTime HTTPMessage<HTTPMessageType>::get_date_field(const char* name) const {
  iovec value;
  if (get_field(name, value)) {
    int cs;
    const char* eof = static_cast<char*>(value.iov_base) + value.iov_len;
    char* p = static_cast<char*>(value.iov_base);
    const char* pe = eof;

    int hour = 0, minute = 0, second = 0;
    int day = 0, month = 0, year = 0;


    /* #line 2 "src\\yield\\http\\http_message.cpp" */
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


    /* #line 226 "src\\yield\\http\\http_message.cpp" */
    {
      cs = date_parser_start;
    }

    /* #line 229 "src\\yield\\http\\http_message.cpp" */
    {
      int _klen;
      unsigned int _trans;
      const char* _acts;
      unsigned int _nacts;
      const unsigned char* _keys;

      if (p == pe)
        goto _test_eof;
      if (cs == 0)
        goto _out;
_resume:
      _keys = _date_parser_trans_keys + _date_parser_key_offsets[cs];
      _trans = _date_parser_index_offsets[cs];

      _klen = _date_parser_single_lengths[cs];
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

      _klen = _date_parser_range_lengths[cs];
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
      _trans = _date_parser_indicies[_trans];
      cs = _date_parser_trans_targs[_trans];

      if (_date_parser_trans_actions[_trans] == 0)
        goto _again;

      _acts = _date_parser_actions + _date_parser_trans_actions[_trans];
      _nacts = (unsigned int) * _acts++;
      while (_nacts-- > 0) {
        switch (*_acts++) {
        case 0:
          /* #line 35 "src\\yield\\http\\date.rl" */
        {
          hour = atoi(p);
        }
        break;
        case 1:
          /* #line 36 "src\\yield\\http\\date.rl" */
        {
          minute = atoi(p);
        }
        break;
        case 2:
          /* #line 37 "src\\yield\\http\\date.rl" */
        {
          second = atoi(p);
        }
        break;
        case 3:
          /* #line 39 "src\\yield\\http\\date.rl" */
        {
          day = atoi(p);
        }
        break;
        case 4:
          /* #line 40 "src\\yield\\http\\date.rl" */
        {
          day = atoi(p);
        }
        break;
        case 5:
          /* #line 47 "src\\yield\\http\\date.rl" */
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
          /* #line 78 "src\\yield\\http\\date.rl" */
        {
          year = atoi(p);
          year += (year < 50 ? 2000 : 1900);
        }
        break;
        case 7:
          /* #line 79 "src\\yield\\http\\date.rl" */
        {
          year = atoi(p);
        }
        break;
        case 8:
          /* #line 116 "src\\yield\\http\\http_message.rl" */
        {
          return DateTime::INVALID_DATE_TIME;
        }
        break;
        /* #line 359 "src\\yield\\http\\http_message.cpp" */
        }
      }

_again:
      if (cs == 0)
        goto _out;
      if (++p != pe)
        goto _resume;
_test_eof:
      {}
      if (p == eof) {
        const char* __acts = _date_parser_actions + _date_parser_eof_actions[cs];
        unsigned int __nacts = (unsigned int) * __acts++;
        while (__nacts-- > 0) {
          switch (*__acts++) {
          case 8:
            /* #line 116 "src\\yield\\http\\http_message.rl" */
          {
            return DateTime::INVALID_DATE_TIME;
          }
          break;
          /* #line 377 "src\\yield\\http\\http_message.cpp" */
          }
        }
      }

_out:
      {}
    }

    /* #line 121 "src\\yield\\http\\http_message.rl" */


    if (cs != date_parser_error) {
      if (year < 100) year += 2000;
      year -= 1900;
      return DateTime(second, minute, hour, day, month - 1, year, false);
    } else
      return DateTime::INVALID_DATE_TIME;
  } else
    return DateTime::INVALID_DATE_TIME;
}

template <class HTTPMessageType>
bool
HTTPMessage<HTTPMessageType>::
get_field(
  const char* name,
  size_t name_len,
  OUT iovec& value
) const {
  int cs;
  const char* eof = static_cast<char*>(buffer) + buffer.size();
  char* p = static_cast<char*>(buffer) + fields_offset;
  const char* pe = eof;

  iovec field_name = {0}, field_value = {0};

  // Don't look for the trailing CRLF before the body,
  // since it may not be present yet.

  /* #line 383 "src\\yield\\http\\http_message.cpp" */
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


  /* #line 435 "src\\yield\\http\\http_message.cpp" */
  {
    cs = field_parser_start;
  }

  /* #line 438 "src\\yield\\http\\http_message.cpp" */
  {
    int _klen;
    unsigned int _trans;
    const char* _acts;
    unsigned int _nacts;
    const unsigned char* _keys;

    if (p == pe)
      goto _test_eof;
    if (cs == 0)
      goto _out;
_resume:
    _keys = _field_parser_trans_keys + _field_parser_key_offsets[cs];
    _trans = _field_parser_index_offsets[cs];

    _klen = _field_parser_single_lengths[cs];
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

    _klen = _field_parser_range_lengths[cs];
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
    _trans = _field_parser_indicies[_trans];
    cs = _field_parser_trans_targs[_trans];

    if (_field_parser_trans_actions[_trans] == 0)
      goto _again;

    _acts = _field_parser_actions + _field_parser_trans_actions[_trans];
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
        /* #line 155 "src\\yield\\http\\http_message.rl" */
      {
        if (
          field_name.iov_len == name_len
          &&
          memcmp(field_name.iov_base, name, name_len) == 0
        ) {
          value = field_value;
          return true;
        }
      }
      break;
      /* #line 535 "src\\yield\\http\\http_message.cpp" */
      }
    }

_again:
    if (cs == 0)
      goto _out;
    if (++p != pe)
      goto _resume;
_test_eof:
    {}
_out:
    {}
  }

  /* #line 170 "src\\yield\\http\\http_message.rl" */


  return false;
}

template <class HTTPMessageType>
void
HTTPMessage<HTTPMessageType>::
get_fields(
  OUT vector<pair<iovec, iovec> >& fields
) const {
  int cs;
  const char* eof = static_cast<char*>(buffer) + buffer.size();
  char* p = static_cast<char*>(buffer) + fields_offset;
  const char* pe = eof;

  iovec field_name = {0}, field_value = {0};

  // Don't look for the trailing CRLF before the body,
  // since it may not be present yet.

  /* #line 546 "src\\yield\\http\\http_message.cpp" */
  static const char _fields_parser_actions[] = {
    0, 1, 0, 1, 1, 1, 2, 1,
    3, 1, 4, 2, 2, 3
  };

  static const char _fields_parser_key_offsets[] = {
    0, 0, 15, 20, 21, 25
  };

  static const unsigned char _fields_parser_trans_keys[] = {
    58u, 124u, 126u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 13u,
    32u, 127u, 0u, 31u, 10u, 13u, 127u, 0u,
    31u, 124u, 126u, 33u, 38u, 42u, 43u, 45u,
    46u, 48u, 57u, 65u, 90u, 94u, 122u, 0
  };

  static const char _fields_parser_single_lengths[] = {
    0, 3, 3, 1, 2, 2
  };

  static const char _fields_parser_range_lengths[] = {
    0, 6, 1, 0, 1, 6
  };

  static const char _fields_parser_index_offsets[] = {
    0, 0, 10, 15, 17, 21
  };

  static const char _fields_parser_indicies[] = {
    2, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 3, 4, 1, 1, 5, 6,
    1, 7, 1, 1, 8, 9, 9, 9,
    9, 9, 9, 9, 9, 1, 0
  };

  static const char _fields_parser_trans_targs[] = {
    1, 0, 2, 3, 2, 4, 5, 3,
    4, 1
  };

  static const char _fields_parser_trans_actions[] = {
    0, 0, 3, 11, 5, 5, 9, 7,
    0, 1
  };

  static const int fields_parser_start = 5;
  static const int fields_parser_first_final = 5;
  static const int fields_parser_error = 0;

  static const int fields_parser_en_main = 5;


  /* #line 598 "src\\yield\\http\\http_message.cpp" */
  {
    cs = fields_parser_start;
  }

  /* #line 601 "src\\yield\\http\\http_message.cpp" */
  {
    int _klen;
    unsigned int _trans;
    const char* _acts;
    unsigned int _nacts;
    const unsigned char* _keys;

    if (p == pe)
      goto _test_eof;
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
        /* #line 195 "src\\yield\\http\\http_message.rl" */
      {
        fields.push_back(make_pair(field_name, field_value));
      }
      break;
      /* #line 689 "src\\yield\\http\\http_message.cpp" */
      }
    }

_again:
    if (cs == 0)
      goto _out;
    if (++p != pe)
      goto _resume;
_test_eof:
    {}
_out:
    {}
  }

  /* #line 201 "src\\yield\\http\\http_message.rl" */

}

template <class HTTPMessageType>
bool HTTPMessage<HTTPMessageType>::is_finalized() const {
  debug_assert_gt(buffer.size(), 4);

  return strncmp(
           static_cast<char*>(buffer) + buffer.size() - 4,
           "\r\n\r\n",
           4
         )
         == 0;
}

template <class HTTPMessageType>
void HTTPMessage<HTTPMessageType>::mark_fields_offset() {
  fields_offset = static_cast<uint16_t>(buffer.size());
}

template <class HTTPMessageType>
HTTPMessage<HTTPMessageType>::operator Buffer& () {
  finalize();
  return buffer;
}

template <class HTTPMessageType>
HTTPMessageType&
HTTPMessage<HTTPMessageType>::
set_field(
  const char* name,
  size_t name_len,
  const DateTime& value
) {
  static const char* HTTPWeekDays[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };

  static const char* HTTPMonths[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };

  char date[30];
  int date_len;

#ifdef _WIN32
  SYSTEMTIME utc_system_time = value.as_utc_SYSTEMTIME();

  date_len
  = _snprintf_s(
      date,
      30,
      _TRUNCATE,
      "%s, %02d %s %04d %02d:%02d:%02d GMT",
      HTTPWeekDays[utc_system_time.wDayOfWeek],
      utc_system_time.wDay,
      HTTPMonths[utc_system_time.wMonth - 1],
      utc_system_time.wYear,
      utc_system_time.wHour,
      utc_system_time.wMinute,
      utc_system_time.wSecond
    );
#else
  tm utc_tm = value.as_utc_tm();

  date_len
  = snprintf(
      date,
      30,
      "%s, %02d %s %04d %02d:%02d:%02d GMT",
      HTTPWeekDays[utc_tm.tm_wday],
      utc_tm.tm_mday,
      HTTPMonths[utc_tm.tm_mon],
      utc_tm.tm_year + 1900,
      utc_tm.tm_hour,
      utc_tm.tm_min,
      utc_tm.tm_sec
    );
#endif

  return set_field(name, name_len, date, date_len);
}

template <class HTTPMessageType>
HTTPMessageType&
HTTPMessage<HTTPMessageType>::
set_field(
  const char* name,
  size_t name_len,
  size_t value
) {
  char value_str[64];
  int value_str_len;

#ifdef _WIN32
  value_str_len = sprintf_s(value_str, 64, "%u", value);
#else
  value_str_len = snprintf(value_str, 64, "%zu", value);
#endif

  return set_field(name, name_len, value_str, value_str_len);
}

template <class HTTPMessageType>
HTTPMessageType&
HTTPMessage<HTTPMessageType>::
set_field(
  const char* name,
  size_t name_len,
  const void* value,
  size_t value_len
) {
  debug_assert_gt(fields_offset, 0);
  debug_assert_gt(name_len, 0);
  debug_assert_gt(value_len, 0);

  if (!is_finalized()) {
    buffer.put(name, name_len);
    buffer.put(": ", 2);
    buffer.put(value, value_len);
    buffer.put("\r\n");
  }

  return static_cast<HTTPMessageType&>(*this);
}

template class HTTPMessage<HTTPRequest>;
template class HTTPMessage<HTTPResponse>;
}
}

#ifdef _WIN32
#pragma warning(pop)
#endif
//
