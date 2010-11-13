// yield/fs/volume_test.hpp

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


#ifndef _YIELD_FS_VOLUME_TEST_HPP_
#define _YIELD_FS_VOLUME_TEST_HPP_


#include "extended_attributes_test.hpp"
#include "yield/auto_object.hpp"
#include "yield/fs/file.hpp"
#include "yield/fs/stat.hpp"
#include "yield/fs/volume.hpp"
#include "yunit.hpp"

#include <fcntl.h>
#ifndef _WIN32
  #include <sys/statvfs.h>
#endif


namespace yield
{
  namespace fs
  {
    class VolumeTest : public yunit::Test
    {
    public:
      virtual ~VolumeTest() { }

    protected:
      VolumeTest( Volume& volume )
        : test_dir_name( "volume_test" ),
          test_file_name( "volume_test.txt" ),
          test_link_name( "volume_test_link.txt" ),
          volume( volume.inc_ref() )
      { }

      const Path& get_test_dir_name() const { return test_dir_name; }
      const Path& get_test_file_name() const { return test_file_name; }
      const Path& get_test_link_name() const { return test_link_name; }
      Volume& get_volume() const { return volume; }

      // yunit::Test
      void setup()
      {
        teardown();
        volume.touch( get_test_file_name() );
      }

      void teardown()
      {
        volume.rmtree( get_test_dir_name() );
        volume.unlink( get_test_file_name() );
        volume.unlink( get_test_link_name() );
      }

    private:
      Path test_dir_name, test_file_name, test_link_name;
      Volume& volume;
    };


    class VolumeAccessTest : public VolumeTest
    {
    public:
      VolumeAccessTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        throw_assert( get_volume().access( get_test_file_name(), O_RDONLY ) );
      }
    };


    class VolumeChmodTest : public VolumeTest
    {
    public:
      VolumeChmodTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        mode_t mode = Volume::FILE_MODE_DEFAULT;
        if ( get_volume().chmod( get_test_file_name(), mode ) )
          return;
        else if ( Exception::get_last_error_code() != ENOTSUP )
          throw Exception();
      }
    };


    class VolumeChownTest : public VolumeTest
    {
    public:
      VolumeChownTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        uid_t uid; gid_t gid;
#ifdef _WIN32
        uid = 0; gid = 0;
#else
        uid = getuid(); gid = getgid();
#endif

        if ( get_volume().chown( get_test_file_name(), uid, gid ) )
          return;
        else if ( Exception::get_last_error_code() != ENOTSUP )
          throw Exception();
      }
    };


    class VolumeCreatTest : public VolumeTest
    {
    public:
      VolumeCreatTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        File* file = get_volume().creat( get_test_file_name() );
        if ( file != NULL )
          File::dec_ref( *file );
        else
          throw Exception();
      }
    };


    class VolumeExistsTest : public VolumeTest
    {
    public:
      VolumeExistsTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        if ( !get_volume().exists( get_test_file_name() ) )
          throw Exception();

        throw_assert_false( get_volume().exists( Path( "some other file.txt" ) ) );
      }
    };


    class VolumeExtendedAttributesGetTest
      : public VolumeTest,
        private ExtendedAttributesGetTest
    {
    public:
      VolumeExtendedAttributesGetTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        ExtendedAttributesGetTest::run
        (
           get_volume().openxattrs( get_test_file_name() )
        );
      }
    };


    class VolumeExtendedAttributesListTest
      : public VolumeTest,
        private ExtendedAttributesListTest
    {
    public:
      VolumeExtendedAttributesListTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        ExtendedAttributesListTest::run
        (
          get_volume().openxattrs( get_test_file_name() )
        );
      }
    };


    class VolumeExtendedAttributesRemoveTest
      : public VolumeTest,
        private ExtendedAttributesRemoveTest
    {
    public:
      VolumeExtendedAttributesRemoveTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        ExtendedAttributesRemoveTest::run
        (
          get_volume().openxattrs( get_test_file_name() )
        );
      }
    };


    class VolumeExtendedAttributesSetTest
      : public VolumeTest,
        private ExtendedAttributesSetTest
    {
    public:
      VolumeExtendedAttributesSetTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        ExtendedAttributesSetTest::run
        (
          get_volume().openxattrs( get_test_file_name() )
        );
      }
    };


    class VolumeGetattrTest : public VolumeTest
    {
    public:
      VolumeGetattrTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        auto_Object<Stat> stbuf = get_volume().getattr( get_test_file_name() );
      }
    };


    class VolumeIsDirTest : public VolumeTest
    {
    public:
      VolumeIsDirTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        get_volume().mkdir( get_test_dir_name() );
        throw_assert( get_volume().isdir( get_test_dir_name() ) );
        throw_assert_false( get_volume().isdir( get_test_file_name() ) );
        throw_assert_false( get_volume().isdir( Path( "nodir" ) ) );
      }
    };


    class VolumeIsFileTest : public VolumeTest
    {
    public:
      VolumeIsFileTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        throw_assert( get_volume().isfile( get_test_file_name() ) );
        get_volume().mkdir( get_test_dir_name() );
        throw_assert_false( get_volume().isfile( get_test_dir_name() ) );
        throw_assert_false( get_volume().isfile( Path( "nofile.txt" ) ) );
      }
    };


    class VolumeLinkTest : public VolumeTest
    {
    public:
      VolumeLinkTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        if ( get_volume().link( get_test_file_name(), get_test_link_name() ) )
          return;
        else if ( Exception::get_last_error_code() != ENOTSUP )
          throw Exception();
      }
    };


    class VolumeMkdirTest : public VolumeTest
    {
    public:
      VolumeMkdirTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        if ( !get_volume().mkdir( get_test_dir_name() ) )
          throw Exception();
      }
    };


    class VolumeMktreeTest : public VolumeTest
    {
    public:
      VolumeMktreeTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        Path subdir_path( Path( "volume_test" ) + Path( "subdir" ) );
        if ( !get_volume().mktree( subdir_path ) ) throw Exception();
        throw_assert( get_volume().exists( subdir_path ) );
      }
    };


    class VolumeOpenTest : public VolumeTest
    {
    public:
      VolumeOpenTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        auto_Object<File> file = get_volume().open( get_test_file_name() );

        File* no_file = get_volume().open( Path( "nofile.txt" ), O_RDONLY );
        if ( no_file != NULL )
        {
          File::dec_ref( *no_file );
          throw_assert( false );
        }          
      }
    };


    class VolumeReadLinkTest : public VolumeTest
    {
    public:
      VolumeReadLinkTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        if ( get_volume().symlink( get_test_file_name(), get_test_link_name() ))
        {
          Path target_path;
          if ( get_volume().readlink( get_test_link_name(), target_path ) )
          {
            throw_assert_eq( target_path, get_test_file_name() );
          }
          else if ( Exception::get_last_error_code() != ENOTSUP )
            throw Exception();
        }
        else if ( Exception::get_last_error_code() != ENOTSUP )
          throw Exception();
      }
    };


    class VolumeRealPathTest : public VolumeTest
    {
    public:
      VolumeRealPathTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // yunit::Test
      void run()
      {
        Path realpath;
        if ( !get_volume().realpath( get_test_file_name(), realpath ) )
          throw Exception();
        throw_assert_false( realpath.empty() );
        throw_assert_ne( get_test_file_name(), realpath );
        throw_assert_lt( get_test_file_name().size(), realpath.size() );
      }
    };


    class VolumeRenameTest : public VolumeTest
    {
    public:
      VolumeRenameTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        if ( !get_volume().rename( get_test_file_name(), Path( "volume_test2.txt" ) ) )
          throw Exception();
        get_volume().unlink( Path( "volume_test2.txt" ) );
      }
    };


    class VolumeRmDirTest : public VolumeTest
    {
    public:
      VolumeRmDirTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        get_volume().mkdir( get_test_dir_name() );
        if ( !get_volume().rmdir( get_test_dir_name() ) )
          throw Exception();
      }
    };


    class VolumeRmTreeTest : public VolumeTest
    {
    public:
      VolumeRmTreeTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        if ( !get_volume().mkdir( get_test_dir_name() ) )
          throw Exception();

        if ( !get_volume().touch ( get_test_dir_name() / get_test_file_name() ) )
          throw Exception();

        if ( !get_volume().rmtree( get_test_dir_name() ) )
          throw Exception();

        throw_assert_false( get_volume().exists( get_test_dir_name() ) );
      }
    };


    class VolumeStatVFSTest : public VolumeTest
    {
    public:
      VolumeStatVFSTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        get_volume().mkdir( get_test_dir_name() );
        struct statvfs stbuf;
        if ( !get_volume().statvfs( get_test_dir_name(), stbuf ) )
          throw Exception();
        throw_assert_gt( stbuf.f_bsize, 0 );
        throw_assert_gt( stbuf.f_blocks, 0 );
        throw_assert_gt( stbuf.f_bfree, 0 );
        throw_assert_ge( stbuf.f_blocks, stbuf.f_bfree );
        throw_assert_ge( stbuf.f_namemax, 0 );
      }
    };


    class VolumeTouchTest : public VolumeTest
    {
    public:
      VolumeTouchTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        if ( !get_volume().touch( get_test_file_name() ) )
          throw Exception();

        if ( get_volume().touch( get_test_dir_name() / get_test_file_name() ) )
        {
          throw_assert( false );
        }
      }
    };


    class VolumeSymlinkTest : public VolumeTest
    {
    public:
      VolumeSymlinkTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        if ( get_volume().symlink( get_test_file_name(), get_test_link_name() ))
          return;
        else if ( Exception::get_last_error_code() != ENOTSUP )
          throw Exception();
      }
    };


    class VolumeTruncateTest : public VolumeTest
    {
    public:
      VolumeTruncateTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        {
          auto_Object<File> file
            = get_volume().open( get_test_file_name(), O_WRONLY );
          ssize_t write_ret = file->write( "test", 4 );
          if ( write_ret != 4 ) throw Exception();
          if ( !file->sync() ) throw Exception();
          auto_Object<Stat> stbuf = get_volume().getattr( get_test_file_name() );
          throw_assert_eq( stbuf->get_size(), 4 );
        }

        if ( get_volume().truncate( get_test_file_name(), 0 ) )
        {
          auto_Object<Stat> stbuf = get_volume().getattr( get_test_file_name() );
          throw_assert_eq( stbuf->get_size(), 0 );
        }
        else
          throw Exception();
      }
    };


    class VolumeUnlinkTest : public VolumeTest
    {
    public:
      VolumeUnlinkTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        if ( !get_volume().unlink( get_test_file_name() ) )
          throw Exception();
      }
    };


    class VolumeUtimeTest : public VolumeTest
    {
    public:
      VolumeUtimeTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        DateTime atime = DateTime::now(),
                 mtime = DateTime::now();

        if ( get_volume().utime( get_test_file_name(), atime, mtime ) )
        {
          auto_Object<Stat> stbuf = get_volume().getattr( get_test_file_name() );
          throw_assert_le( stbuf->get_atime() - atime, Time::NS_IN_S );
          throw_assert_le( stbuf->get_mtime() - mtime, Time::NS_IN_S );
        }
        else
          throw Exception();

        atime = DateTime::now();
        mtime = DateTime::now();
        DateTime ctime = DateTime::now();

        if ( get_volume().utime( get_test_file_name(), atime, mtime, ctime ) )
        {
          auto_Object<Stat> stbuf = get_volume().getattr( get_test_file_name() );
          throw_assert_le( stbuf->get_atime() - atime, Time::NS_IN_S );
          throw_assert_le( stbuf->get_mtime() - mtime, Time::NS_IN_S );
          throw_assert_le( stbuf->get_ctime() - ctime, Time::NS_IN_S );
        }
        else if ( Exception::get_last_error_code() != ENOTSUP )
          throw Exception();
      }
    };


    class VolumeVolnameTest : public VolumeTest
    {
    public:
      VolumeVolnameTest( Volume& volume )
        : VolumeTest( volume )
      { }

      // Test
      void run()
      {
        Path volname;
        if ( get_volume().volname( get_test_file_name(), volname ) )
        {
          throw_assert_false( volname.empty() );
        }
        else if ( Exception::get_last_error_code() != ENOTSUP )
          throw Exception();
      }
    };


    template <class VolumeType>
    class VolumeTestSuite : public yunit::TestSuite
    {
    public:
      VolumeTestSuite( YO_NEW_REF VolumeType* volume = NULL )
      {
        if ( volume == NULL )
          volume = new VolumeType;

        add( "Volume::access", new VolumeAccessTest( *volume ) );
        add( "Volume::chmod", new VolumeChmodTest( *volume ) );
        add( "Volume::chown", new VolumeChownTest( *volume ) );

        add( "Volume::creat", new VolumeCreatTest( *volume ) );

        add( "Volume::exists", new VolumeExistsTest( *volume ) );

        add( "Volume::getattr", new VolumeGetattrTest( *volume ) );

        add
        (
          "Volume::getxattr",
          new VolumeExtendedAttributesGetTest( *volume )
        );

        add( "Volume::isdir", new VolumeIsDirTest( *volume ) );
        add( "Volume::isfile", new VolumeIsFileTest( *volume ) );

        add
        (
          "Volume::listxattr",
          new VolumeExtendedAttributesListTest( *volume )
        );

        add( "Volume::link", new VolumeLinkTest( *volume ) );

        add( "Volume::mkdir", new VolumeMkdirTest( *volume ) );
        add( "Volume::mktree", new VolumeMktreeTest( *volume ) );

        add( "Volume::open", new VolumeOpenTest( *volume ) );

        add( "Volume::readlink", new VolumeReadLinkTest( *volume ) );

        add( "Volume::realpath", new VolumeRealPathTest( *volume ) );

        add
        (
          "Volume::removexattr",
          new VolumeExtendedAttributesRemoveTest( *volume )
        );

        add( "Volume::rename", new VolumeRenameTest( *volume ) );

        add( "Volume::rmdir", new VolumeRmDirTest( *volume ) );
        add( "Volume::rmtree", new VolumeRmTreeTest( *volume ) );

        add
        (
          "Volume::setxattr",
          new VolumeExtendedAttributesSetTest( *volume )
        );

        add( "Volume::statvfs", new VolumeStatVFSTest( *volume ) );

        add( "Volume::symlink", new VolumeSymlinkTest( *volume ) );

        add( "Volume::touch", new VolumeTouchTest( *volume ) );

        add( "Volume::truncate", new VolumeTruncateTest( *volume ) );

        add( "Volume::unlink", new VolumeUnlinkTest( *volume ) );

        add( "Volume::utime", new VolumeUtimeTest( *volume ) );

        add( "Volume::volname", new VolumeVolnameTest( *volume ) );

        Volume::dec_ref( *volume );
      }
    };
  }
}


#endif
