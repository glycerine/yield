// yield/fs/named_pipe_test.cpp

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


#include "../channel_test.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/volume.hpp"

#include <fcntl.h> // For O_*


namespace yield {
namespace fs {
class NamedPipePair : public ChannelPair {
public:
  NamedPipePair( const Path& path, Volume& volume )
    : path( path ),
      volume( volume.inc_ref() ) {
    read_file = write_file = NULL;
  }

  ~NamedPipePair() {
    File::dec_ref( read_file );
    File::dec_ref( write_file );
    volume.unlink( path );
    Volume::dec_ref( volume );
  }

  File& get_read_file() {
    if ( read_file == NULL ) {
      read_file = volume.mkfifo( path );
      if ( read_file == NULL )
        throw Exception();
    }

    return *read_file;
  }

  File& get_write_file() {
    if ( write_file == NULL ) {
      get_read_file(); // To start the server if necessary
      write_file = volume.open( path, O_WRONLY );
      if ( write_file == NULL )
        throw Exception();
    }

    return *write_file;
  }

  // yield::ChannelPair
  Channel& get_read_channel() {
    return get_read_file();
  }
  Channel& get_write_channel() {
    return get_write_file();
  }

private:
  Path path;
  File *read_file, *write_file;
  Volume& volume;
};


class NamedPipePairFactory : public ChannelPairFactory {
public:
  NamedPipePairFactory( const Path& path, Volume& volume )
    : path( path ),
      volume( volume.inc_ref() )
  { }

  ~NamedPipePairFactory() {
    Volume::dec_ref( volume );
  }

  // yield::Object
  NamedPipePairFactory& inc_ref() {
    return Object::inc_ref( *this );
  }

  // yield::ChannelPairFactory
  ChannelPair& createChannelPair() {
    return *new NamedPipePair( path, volume );
  }

private:
  Path path;
  Volume& volume;
};


class NamedPipeTestSuite : public ChannelTestSuite {
public:
  NamedPipeTestSuite( YO_NEW_REF Volume* volume = NULL )
    : ChannelTestSuite
    (
      *new NamedPipePairFactory
      (
#ifdef _WIN32
        "\\\\.\\pipe\\named_pipe_test.txt",
#else
        "named_pipe_test.txt",
#endif
        ( volume != NULL ? *volume : *Volume::create() )
      )
    )
  { }
};
}
}

TEST_SUITE_EX( NamedPipe, yield::fs::NamedPipeTestSuite );
