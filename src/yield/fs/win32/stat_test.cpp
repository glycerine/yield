// yield/fs/win32/stat_test.cpp

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

#include <Windows.h>



namespace yield
{
  namespace fs
  {
    namespace win32
    {
      class StatBY_HANDLE_FILE_INFORMATIONTest : public StatTest
      {
      public:
        StatBY_HANDLE_FILE_INFORMATIONTest( yield::fs::Volume& volume )
          : StatTest( volume )
        { }

        virtual void run()
        {
          auto_Object<Stat> stbuf1
            = static_cast<Stat*>( get_volume().stat( get_test_file_name() ) );

          BY_HANDLE_FILE_INFORMATION bhfi = *stbuf1;

          throw_assert_ne( bhfi.dwFileAttributes, 0 );

          throw_assert_ne( DateTime( bhfi.ftCreationTime ), 0 );

          throw_assert_ge
          (
            DateTime( bhfi.ftLastAccessTime ),
            DateTime( bhfi.ftCreationTime )
          );

          throw_assert_ge
          (
            DateTime( bhfi.ftLastWriteTime ),
            DateTime( bhfi.ftCreationTime )
          );

          throw_assert_eq( bhfi.nFileSizeLow, 0 );
          throw_assert_eq( bhfi.nFileSizeHigh, 0 );

          throw_assert_eq( bhfi.nNumberOfLinks, 1 );

          throw_assert_eq( bhfi.nFileIndexLow, 0 );
          throw_assert_eq( bhfi.nFileIndexHigh, 0 );

          Stat stbuf2( bhfi );
          throw_assert_eq( stbuf2, *stbuf1 );

          stbuf2 = bhfi;
          throw_assert_eq( stbuf2, *stbuf1 );
        }
      };


      class StatWIN32_FILE_ATTRIBUTE_DATATest : public StatTest
      {
      public:
        StatWIN32_FILE_ATTRIBUTE_DATATest( yield::fs::Volume& volume )
          : StatTest( volume )
        { }

        virtual void run()
        {
          auto_Object<Stat> stbuf1
            = static_cast<Stat*>( get_volume().stat( get_test_file_name() ) );

          WIN32_FILE_ATTRIBUTE_DATA fad = *stbuf1;

          throw_assert_ne( fad.dwFileAttributes, 0 );

          throw_assert_ne( DateTime( fad.ftCreationTime ), 0 );

          throw_assert_ge
          (
            DateTime( fad.ftLastAccessTime ),
            DateTime( fad.ftCreationTime )
          );

          throw_assert_ge
          (
            DateTime( fad.ftLastWriteTime ),
            DateTime( fad.ftCreationTime )
          );

          throw_assert_eq( fad.nFileSizeLow, 0 );
          throw_assert_eq( fad.nFileSizeHigh, 0 );

          Stat stbuf2( fad );
          throw_assert_eq( stbuf2, *stbuf1 );

          stbuf2 = fad;
          throw_assert_eq( stbuf2, *stbuf1 );
        }
      };


      class StatWIN32_FIND_DATATest : public StatTest
      {
      public:
        StatWIN32_FIND_DATATest( yield::fs::Volume& volume )
          : StatTest( volume )
        { }

        virtual void run()
        {
          auto_Object<Stat> stbuf1
            = static_cast<Stat*>( get_volume().stat( get_test_file_name() ) );

          WIN32_FIND_DATA fd = *stbuf1;

          throw_assert_ne( fd.dwFileAttributes, 0 );

          throw_assert_ne( DateTime( fd.ftCreationTime ), 0 );

          throw_assert_ge
          (
            DateTime( fd.ftLastAccessTime ),
            DateTime( fd.ftCreationTime )
          );

          throw_assert_ge
          (
            DateTime( fd.ftLastWriteTime ),
            DateTime( fd.ftCreationTime )
          );

          throw_assert_eq( fd.nFileSizeLow, 0 );
          throw_assert_eq( fd.nFileSizeHigh, 0 );

          throw_assert_eq( fd.cFileName[0], 0 );
          throw_assert_eq( fd.cAlternateFileName[0], 0 );

          Stat stbuf2( fd );
          throw_assert_eq( stbuf2, *stbuf1 );

          stbuf2 = fd;
          throw_assert_eq( stbuf2, *stbuf1 );
        }
      };


      class StatTestSuite : public yield::fs::StatTestSuite<Volume>
      {
      public:
        StatTestSuite()
        {
          add
          (
            "Stat+BY_HANDLE_FILE_INFORMATION()",
            new StatBY_HANDLE_FILE_INFORMATIONTest( get_volume() )
          );

          add
          (
            "Stat+WIN32_FILE_ATTRIBUTE_DATA",
            new StatWIN32_FILE_ATTRIBUTE_DATATest( get_volume() )
          );

          add
          (
            "Stat+WIN32_FIND_DATA",
            new StatWIN32_FIND_DATATest( get_volume() )
          );
        }
      };
    }
  }
}

TEST_SUITE_EX( Win32Stat, yield::fs::win32::StatTestSuite );
