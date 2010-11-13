# yutil_test.py

# Copyright (c) 2010 Minor Gordon
# All rights reserved

# This source file is part of the yutil project.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# * Neither the name of the yutil project nor the
# names of its contributors may be used to endorse or promote products
# derived from this software without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL Minor Gordon BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


from os import makedirs, mkdir, unlink
from os.path import curdir, exists, join, pardir
from shutil import rmtree
from unittest import TestCase, TestSuite

from yutil import *


class CopyFileTest(TestCase):
    def _get_source_file_name( self ):
        return "source.txt"

    def _get_target_file_name( self ):
        return "target.txt"

    def runTest( self ):
        assert copy_file( self._get_source_file_name(), self._get_target_file_name() )
        fd = open( self._get_target_file_name() )
        contents = fd.read()
        fd.close()
        assert contents == "test"

        assert not copy_file( self._get_source_file_name(), self._get_target_file_name() )

    def setUp( self ):
        self.tearDown()
        open( self._get_source_file_name(), "w+" ).write( "test" )

    def tearDown( self ):
        try: unlink( self._get_source_file_name() )
        except: pass
        try: unlink( self._get_target_file_name() )
        except: pass


class IsHiddenTest(TestCase):
    def runTest( self ):
        assert ishidden( ".svn" )
        assert ishidden( join( "test", ".svn" ) )

        assert not ishidden( "svn" )
        assert not ishidden( join( "test", "svn" ) )
        assert not ishidden( curdir )
        assert not ishidden( pardir )


class ListFilesTest(TestCase):
    def runTest( self ):
        files = list( list_files( "files" ) )
        assert len( files ) == 3
        for i in xrange( len( files ) ):
            assert files[i] == join( "files", "file" + str( i ) + ".txt" )

    def setUp( self ):
        self.tearDown()
        mkdir( "files" )
        for i in xrange( 3 ):
            touch( join( "files", "file" + str( i ) + ".txt" ) )

    def tearDown( self ):
        try: rmtree( "files" )
        except: pass


class ListSubdirectoriesTest(TestCase):
    def runTest( self ):
        subdirs = list( list_subdirectories( "subdir" ) )
        assert len( subdirs ) == 3
        for i in xrange( len( subdirs ) ):
            assert subdirs[i] == join( "subdir", "subdir" + str( i ) )

    def setUp( self ):
        self.tearDown()
        for i in xrange( 3 ):
            makedirs( join( "subdir", "subdir" + str( i ) ) )

    def tearDown( self ):
        try: rmtree( "subdir" )
        except: pass



class MirrorSubdirectoriesTest(TestCase):
    def runTest( self ):
        mirror_subdirectories( "source0", "target0" )
        assert exists( join( "target0", "source1" ) )
        assert exists( join( "target0", "source1", "source2" ) )

    def setUp( self ):
        self.tearDown()
        makedirs( join( "source0", "source1", "source2" ) )
        mkdir( "target0" )

    def tearDown( self ):
        try: rmtree( "source0" )
        except: pass
        try: rmtree( "target0" )
        except: pass



class rglobDirectoryTest(TestCase):
    def runTest( self ):
        paths = list( rglob( "rglob0" ) )
        assert len( paths ) == 1, paths
        assert paths[0] == "rglob0", paths[0]

        paths = list( rglob( join( "rglob0", '*' ) ) )
        assert len( paths ) == 1, paths
        assert paths[0] == join( "rglob0", "rglob1" ), paths[0]

        paths = list( rglob( join( "rglob0", "*", "rglob2" ) ) )
        assert len( paths ) == 1, paths
        assert paths[0] == join( "rglob0", "rglob1", "rglob2" ), paths[0]

        paths = list( rglob( join( "rglob0", "**" ) ) )
        assert len( paths ) == 2, paths
        assert paths[0] == join( "rglob0", "rglob1" ), paths[0]
        assert paths[1] == join( "rglob0", "rglob1", "rglob2" ), paths[1]

        paths = list( rglob( join( "rglob0", "**", "rglob2" ) ) )
        assert len( paths ) == 1, paths
        assert paths[0] == join( "rglob0", "rglob1", "rglob2" ), paths[0]

        paths = list( rglob( join( "rglob0", "rgl*1*" ) ) )
        assert len( paths ) == 1, paths
        assert paths[0] == join( "rglob0", "rglob1" ), paths[0]

    def setUp( self ):
        self.tearDown()
        makedirs( join( "rglob0", "rglob1", "rglob2" ) )

    def tearDown( self ):
        try: rmtree( "rglob0" )
        except: pass


class rglobFileTest(TestCase):
    def runTest( self ):
        paths = list( rglob( join( "rglob0", "*.txt" ) ) )
        assert len( paths ) == 1, paths
        assert paths[0] == join( "rglob0", "rglob0.txt" )

        paths = list( rglob( join( "rglob0", "rglob1", "*.txt" ) ) )
        assert len( paths ) == 1, paths
        assert paths[0] == join( "rglob0", "rglob1", "rglob1.txt" )

        paths = list( rglob( join( "rglob0", "**", "*.txt" ) ) )
        assert len( paths ) == 3, paths
        assert paths[0] == join( "rglob0", "rglob0.txt" )
        assert paths[1] == join( "rglob0", "rglob1", "rglob1.txt" )
        assert paths[2] == join( "rglob0", "rglob1", "rglob2", "rglob2.txt" )

    def setUp( self ):
        self.tearDown()
        makedirs( join( "rglob0", "rglob1", "rglob2" ) )
        touch( join( "rglob0", "rglob0.txt" ) )
        touch( join( "rglob0", "rglob1", "rglob1.txt" ) )
        touch( join( "rglob0", "rglob1", "rglob2", "rglob2.txt" ) )

    def tearDown( self ):
        try: rmtree( "rglob0" )
        except: pass


class TouchTest(TestCase):
    def _get_test_file_name( self ):
        return "touch_test.txt"

    def runTest( self ):
        touch( self._get_test_file_name() )
        assert exists( self._get_test_file_name() )

        fd = open( self._get_test_file_name(), "w+" )
        fd.write( "hello" )
        fd.close()
        touch( self._get_test_file_name() )
        assert open( self._get_test_file_name() ).read() == "hello"

    def setUp( self ):
        self.tearDown()

    def tearDown( self ):
        try: unlink( self._get_test_file_name() )
        except: pass


class WalkTest(TestCase):
    def runTest( self ):
        root_dir_paths, dir_paths, file_paths = self._walk( "walk0", include_hidden=False )
        assert len( root_dir_paths ) == 3, root_dir_paths
        assert root_dir_paths[0] == "walk0"
        assert root_dir_paths[1] == join( "walk0", "walk1" )
        assert root_dir_paths[2] == join( "walk0", "walk1", "walk2" )
        assert len( dir_paths ) == 2
        for i in xrange( 2 ):
            assert dir_paths[i] == "walk" + str( i+1 )
        assert len( file_paths ) == 3
        for i in xrange( 3 ):
            assert file_paths[i] == "walk" + str( i ) + ".txt"

        root_dir_paths, dir_paths, file_paths = self._walk( "walk0", include_hidden=True )
        assert len( root_dir_paths ) == 4
        assert len( dir_paths ) == 3
        assert len( file_paths ) == 3

    def setUp( self ):
        self.tearDown()
        makedirs( join( "walk0", "walk1", "walk2" ) )
        touch( join( "walk0", "walk0.txt" ) )
        touch( join( "walk0", "walk1", "walk1.txt" ) )
        touch( join( "walk0", "walk1", "walk2", "walk2.txt" ) )
        makedirs( join( "walk0", ".svn" ) )

    def tearDown( self ):
        try: rmtree( "walk0" )
        except: pass

    def _walk( self, *args, **kwds ):
        all_root_dir_paths = []; all_dir_paths = []; all_file_paths = []
        for root_dir_path, dir_paths, file_paths in walk( *args, **kwds ):
            all_root_dir_paths.append( root_dir_path )
            all_dir_paths.extend( list( dir_paths ) )
            all_file_paths.extend( list( file_paths ) )
        return all_root_dir_paths, all_dir_paths, all_file_paths


class WriteFileTest(TestCase):
    def _get_test_file_name( self ):
        return "write_file_test.txt"

    def runTest( self ):
        # Write the same contents twice
        assert write_file( self._get_test_file_name(), "hello" )
        assert not write_file( self._get_test_file_name(), "hello" )
        assert not write_file( self._get_test_file_name(), "hello" )
        self.tearDown()

        # Write the same contents twice, only differing with righthand padding
        assert write_file( self._get_test_file_name(), "hello \nworld" )
        assert not write_file( self._get_test_file_name(), "hello\nworld" )
        self.tearDown()

        # Write a sequence of lines
        assert write_file( self._get_test_file_name(), ( "hello", "world" ) )
        assert not write_file( self._get_test_file_name(), "hello\nworld" )
        self.tearDown()

        # Write A B A
        assert write_file( self._get_test_file_name(), "hello" )
        assert write_file( self._get_test_file_name(), "hellox" )
        assert write_file( self._get_test_file_name(), "hello" )
        self.tearDown()

        # Write with force=True
        assert write_file( self._get_test_file_name(), "hello" )
        assert write_file( self._get_test_file_name(), "hello", force=True )
        self.tearDown()

        # Write a non-string, non-sequence
        assert write_file( self._get_test_file_name(), 1 )
        assert not write_file( self._get_test_file_name(), "1" )
        self.tearDown()

    def setUp( self ):
        self.tearDown()

    def tearDown( self ):
      try: unlink( self._get_test_file_name() )
      except: pass


suite = TestSuite()
suite.addTest( CopyFileTest() )
suite.addTest( IsHiddenTest() )
suite.addTest( ListFilesTest() )
suite.addTest( ListSubdirectoriesTest() )
suite.addTest( MirrorSubdirectoriesTest() )
suite.addTest( rglobDirectoryTest() )
suite.addTest( rglobFileTest() )
suite.addTest( TouchTest() )
suite.addTest( WalkTest() )
suite.addTest( WriteFileTest() )


if __name__ == "__main__":
    import unittest
    unittest.TextTestRunner().run( suite )
