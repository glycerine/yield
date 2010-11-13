// yield/fs/directory_test.hpp

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


#ifndef _YIELD_FS_DIRECTORY_TEST_HPP_
#define _YIELD_FS_DIRECTORY_TEST_HPP_


#include "yield/assert.hpp"
#include "yield/auto_object.hpp"
#include "yield/fs/directory.hpp"
#include "yield/fs/volume.hpp"
#include "yunit.hpp"


namespace yield
{
  namespace fs
  {
    class DirectoryTest : public yunit::Test
    {
    public:
      virtual ~DirectoryTest()
      {
        Volume::dec_ref( volume );
      }

    protected:
      DirectoryTest( Volume& volume )
        : test_dir_name( "directory_test" ),
          test_file_name( "directory_test.txt" ),
          test_file_path( test_dir_name / test_file_name ),
          volume( volume.inc_ref() )
      {
        directory = NULL;
      }

      // yunit::Test
      void setup()
      {
        teardown();

        if ( !get_volume().mkdir( get_test_dir_name() ) )
          throw Exception();

        if ( !get_volume().touch( get_test_file_path() ) )
          throw Exception();

        directory = get_volume().opendir( get_test_dir_name() );
        if ( directory == NULL )
          throw Exception();
      }

      void teardown()
      {
        Directory::dec_ref( directory );
        directory = NULL;

        if ( get_volume().exists( get_test_dir_name() ) )
        {
          if ( !get_volume().rmtree( get_test_dir_name() ) )
            throw Exception();
        }
      }

    protected:
      Directory& get_directory() const { return *directory; }
      const Path& get_test_dir_name() const { return test_dir_name; }
      const Path& get_test_file_name() const { return test_file_name; }
      const Path& get_test_file_path() const { return test_file_path; }
      Volume& get_volume() const { return volume; }

    private:
      Directory* directory;
      Path test_dir_name, test_file_name, test_file_path;
      Volume& volume;
    };


#define YIELD_PLATFORM_DIRECTORY_TEST( TestName ) \
    class Directory_##TestName##Test : public DirectoryTest \
    { \
    public:\
      Directory_##TestName##Test( yield::fs::Volume* volume = NULL ) \
        : DirectoryTest( "Directory_" # TestName "Test", volume ) \
      { } \
      void run(); \
    };\
      inline void Directory_##TestName##Test::run()


    class DirectoryCloseTest : public DirectoryTest
    {
    public:
      DirectoryCloseTest( Volume& volume )
        : DirectoryTest( volume )
      { }

      // Test
      void run()
      {
        if ( !get_directory().close() ) throw Exception();
      }
    };


    class DirectoryReadTest : public DirectoryTest
    {
    public:
      DirectoryReadTest( Volume& volume )
        : DirectoryTest( volume )
      { }

      // Test
      void run()
      {
        {
          auto_Object<Directory::Entry> dentry = get_directory().read();
          throw_assert_eq( dentry->get_name(), Path::CURRENT_DIRECTORY );
          throw_assert( dentry->ISDIR() );
        }

        {
          auto_Object<Directory::Entry> dentry = get_directory().read();
          throw_assert_eq( dentry->get_name(), Path::PARENT_DIRECTORY );
          throw_assert( dentry->ISDIR() );
        }

        {
          auto_Object<Directory::Entry> dentry = get_directory().read();
          throw_assert_eq( dentry->get_name(), get_test_file_name() );
          throw_assert( dentry->has_atime() );
          throw_assert( dentry->has_ctime() );
          throw_assert( dentry->has_mtime() );
          throw_assert( dentry->has_nlink() );
          throw_assert( dentry->has_size() );
          throw_assert_false( dentry->is_hidden() );
          throw_assert( dentry->ISREG() );
        }

        {
          Directory::Entry* dentry = get_directory().read();
          if ( dentry != NULL )
          {
            Directory::Entry::dec_ref( *dentry );
            throw_assert( false );
          }
        }
      }
    };


    class DirectoryRewindTest : public DirectoryTest
    {
    public:
      DirectoryRewindTest( Volume& volume )
        : DirectoryTest( volume )
      { }

      // Test
      void run()
      {
        {
          auto_Object<Directory::Entry> dentry = get_directory().read();
          throw_assert_eq( dentry->get_name(), Path::CURRENT_DIRECTORY );
        }

        get_directory().rewind();

        {
          auto_Object<Directory::Entry> dentry = get_directory().read();
          throw_assert_eq( dentry->get_name(), Path::CURRENT_DIRECTORY );
        }
      }
    };


    template <class VolumeType>
    class DirectoryTestSuite : public yunit::TestSuite
    {
    public:
      DirectoryTestSuite( YO_NEW_REF VolumeType* volume = NULL )
      {
        if ( volume == NULL )
          volume = new VolumeType;

        add( "Directory::close", new DirectoryCloseTest( *volume ) );
        add( "Directory::read", new DirectoryReadTest( *volume ) );
        add( "Directory::rewind", new DirectoryRewindTest( *volume ) );

        Volume::dec_ref( *volume );
      }
    };
  }
}


#endif
