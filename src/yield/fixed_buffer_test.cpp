// yield/fixed_buffer_test.cpp

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


#include "buffer_test.hpp"
#include "yield/fixed_buffer.hpp"


TEST_SUITE(FixedBuffer);

namespace yield {
TEST_EX(FixedBuffer, copy, BufferCopyTest< FixedBuffer<512> >) {
  BufferCopyTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, data, BufferDataTest< FixedBuffer<512> >) {
  BufferDataTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, empty, BufferEmptyTest< FixedBuffer<512> >) {
  BufferEmptyTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, get_next_buffer, BufferGetNextBufferTest< FixedBuffer<512> >) {
  BufferGetNextBufferTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, init_c_string, BufferInitCStringTest< FixedBuffer<512> >) {
  BufferInitCStringTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, init_string, BufferInitStringTest< FixedBuffer<512> >) {
  BufferInitStringTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, operator_array, BufferOperatorArrayTest< FixedBuffer<512> >) {
  BufferOperatorArrayTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, operator_cast, BufferOperatorCastTest< FixedBuffer<512> >) {
  BufferOperatorCastTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, operator_equals, BufferOperatorEqualsTest< FixedBuffer<512> >) {
  BufferOperatorEqualsTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, put_buffer, BufferPutBufferTest< FixedBuffer<512> >) {
  BufferPutBufferTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, put_char, BufferPutCharTest< FixedBuffer<512> >) {
  BufferPutCharTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, put_c_string, BufferPutCStringTest< FixedBuffer<512> >) {
  BufferPutCStringTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, put_c_string_size, BufferPutCStringSizeTest< FixedBuffer<512> >) {
  BufferPutCStringSizeTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, put_iovec, BufferPutIOVecTest< FixedBuffer<512> >) {
  BufferPutIOVecTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, put_string, BufferPutStringTest< FixedBuffer<512> >) {
  BufferPutStringTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, set_next_buffer, BufferSetNextBufferTest< FixedBuffer<512> >) {
  BufferSetNextBufferTest< FixedBuffer<512> >::run();
}

TEST_EX(FixedBuffer, size, BufferSizeTest< FixedBuffer<512> >) {
  BufferSizeTest< FixedBuffer<512> >::run();
}
}
