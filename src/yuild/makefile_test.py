from unittest import TestSuite

from makefile import Makefile
from project_test import *


class MakefileTest(ProjectTest):
    def get_Makefile( self ):
        return self._get_project( Makefile )

    def get_Makefile_lines( self ):
        return self._get_project_lines( Makefile )


class MakefileCXXFLAGSTest(MakefileTest):
    def __init__( self, CXXFLAGS ):
        MakefileTest.__init__( self )
        self.__CXXFLAGS = ' '.join( CXXFLAGS )

    def runTest( self ):
        CXXFLAGS = self.__CXXFLAGS
        assert 'CXXFLAGS += %(CXXFLAGS)s' % locals()\
            % globals() in self.get_Makefile_lines()


class MakefileLDFLAGSTest(MakefileTest):
    def __init__( self, LDFLAGS ):
        MakefileTest.__init__( self )
        self.__LDFLAGS = ' '.join( LDFLAGS )

    def runTest( self ):
        LDFLAGS = self.__LDFLAGS
        assert 'LDFLAGS += %(LDFLAGS)s' % locals()\
            % globals() in self.get_Makefile_lines()
    

class MakefileOutputFilePathTest(MakefileTest):
    def runTest( self ):
        assert '%(TEST_OUTPUT_FILE_PATH)s: $(OBJECT_FILE_PATHS)'\
            % globals() in self.get_Makefile_lines()


class MakefileSourceFilePathsTest(MakefileTest):
    def runTest( self ):
        Makefile_lines = self.get_Makefile_lines()
        SOURCE_FILE_PATHS = ' '.join( TEST_SOURCE_FILE_PATHS )
        assert 'SOURCE_FILE_PATHS = %(SOURCE_FILE_PATHS)s'\
            % locals() in Makefile_lines


suite = TestSuite()
suite.addTest( MakefileCXXFLAGSTest( ["-D" + cxxdefine
                                      for cxxdefine in TEST_CXXDEFINES] ) )
suite.addTest( MakefileCXXFLAGSTest( TEST_CXXFLAGS ) )
suite.addTest( MakefileCXXFLAGSTest( ["-I" + cxxpath
                                      for cxxpath in TEST_CXXPATH] ) )
suite.addTest( MakefileLDFLAGSTest( TEST_LDFLAGS ) )
suite.addTest( MakefileLDFLAGSTest( ["-L" + libpath
                                      for libpath in TEST_LIBPATH] ) )
suite.addTest( MakefileOutputFilePathTest() )
suite.addTest( MakefileSourceFilePathsTest() )


if __name__ == "__main__":
    from unittest import TextTestRunner
    TextTestRunner().run( suite )