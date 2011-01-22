// yield/fs/posix/stat_test.cpp

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


#include "../stat_test.hpp"
#include "stat.hpp"
#include "volume.hpp"


namespace yield {
namespace fs {
namespace posix {
class StatStructStatTest : public StatTest {
public:
  StatStructStatTest( yield::fs::Volume& volume )
    : StatTest( volume )
  { }

  // yunit::Test
  void run() {
    auto_Object<Stat> stbuf1
    = static_cast<Stat*>( get_volume().stat( get_test_file_name() ) );
    DateTime current_date_time = DateTime::now();
    struct stat stbuf2 = stbuf1->operator struct stat();
    throw_assert_ne( stbuf2.st_mode, 0 );
    throw_assert_le( stbuf2.st_atime, current_date_time );
    throw_assert_le( stbuf2.st_mtime, current_date_time );
    throw_assert_le( stbuf2.st_ctime, current_date_time );

    Stat stbuf3( stbuf2 );
    throw_assert_eq( stbuf3, *stbuf1 );
  }
};


class StatTestSuite : public yield::fs::StatTestSuite<Volume> {
public:
  StatTestSuite() {
    add( "Stat+struct stat", new StatStructStatTest( get_volume() ) );
  }
};
}
}
}


TEST_SUITE_EX( POSIXStat, yield::fs::posix::StatTestSuite );
