// yield/net/uuid_test.cpp

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
#include "yield/auto_object.hpp"
#include "yield/net/uuid.hpp"
#include "yunit.hpp"


TEST_SUITE( UUID );

namespace yield {
namespace net {
#if defined(__sun) || defined(_WIN32) || defined(YIELD_HAVE_LINUX_LIBUUID)
TEST( UUID, create ) {
  UUID uuid;
}

TEST( UUID, cast_to_string ) {
  UUID uuid;
  string uuid_str = uuid;
  throw_assert_false( uuid_str.empty() );
#if defined(_WIN32) || defined(YIELD_HAVE_LINUX_LIBUUID)
  throw_assert_eq( uuid_str.size(), 36 );
#endif
}

TEST( UUID, compare ) {
  UUID uuid1, uuid2;
  throw_assert_ne( uuid1, uuid2 );
}

TEST( UUID, parse ) {
  UUID uuid1;
  string uuid1_str = uuid1;

  UUID uuid2( uuid1_str );
  throw_assert_eq( uuid1, uuid2 );
}
#endif
}
}
