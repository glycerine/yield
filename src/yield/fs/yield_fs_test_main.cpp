// yield/fs/yield_fs_test_main.cpp

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

#include "yunit.hpp"

#include <iostream>


extern yunit::TestSuite& DirectoryTestSuite();
extern yunit::TestSuite& MemoryMappedFileTestSuite();
extern yunit::TestSuite& NamedPipeTestSuite();
extern yunit::TestSuite& PathTestSuite();
#ifdef __unix__
extern yunit::TestSuite& POSIXFileSystemTestSuite();
#endif
#ifdef __unix__
extern yunit::TestSuite& POSIXFileTestSuite();
#endif
#ifdef __unix__
extern yunit::TestSuite& POSIXStatTestSuite();
#endif
#ifdef _WIN32
extern yunit::TestSuite& Win32FileSystemTestSuite();
#endif
#ifdef _WIN32
extern yunit::TestSuite& Win32FileTestSuite();
#endif
#ifdef _WIN32
extern yunit::TestSuite& Win32StatTestSuite();
#endif


int main(int, char**) {
  int failed_test_case_count = 0;

  // Directory
  std::cout << "Directory:" << std::endl;
  failed_test_case_count += DirectoryTestSuite().run();
  std::cout << std::endl;

  // MemoryMappedFile
  std::cout << "MemoryMappedFile:" << std::endl;
  failed_test_case_count += MemoryMappedFileTestSuite().run();
  std::cout << std::endl;

  // NamedPipe
  std::cout << "NamedPipe:" << std::endl;
  failed_test_case_count += NamedPipeTestSuite().run();
  std::cout << std::endl;

  // Path
  std::cout << "Path:" << std::endl;
  failed_test_case_count += PathTestSuite().run();
  std::cout << std::endl;

#ifdef __unix__
  // POSIXFileSystem
  std::cout << "POSIXFileSystem:" << std::endl;
  failed_test_case_count += POSIXFileSystemTestSuite().run();
  std::cout << std::endl;
#endif

#ifdef __unix__
  // POSIXFile
  std::cout << "POSIXFile:" << std::endl;
  failed_test_case_count += POSIXFileTestSuite().run();
  std::cout << std::endl;
#endif

#ifdef __unix__
  // POSIXStat
  std::cout << "POSIXStat:" << std::endl;
  failed_test_case_count += POSIXStatTestSuite().run();
  std::cout << std::endl;
#endif

#ifdef _WIN32
  // Win32FileSystem
  std::cout << "Win32FileSystem:" << std::endl;
  failed_test_case_count += Win32FileSystemTestSuite().run();
  std::cout << std::endl;
#endif

#ifdef _WIN32
  // Win32File
  std::cout << "Win32File:" << std::endl;
  failed_test_case_count += Win32FileTestSuite().run();
  std::cout << std::endl;
#endif

#ifdef _WIN32
  // Win32Stat
  std::cout << "Win32Stat:" << std::endl;
  failed_test_case_count += Win32StatTestSuite().run();
  std::cout << std::endl;
#endif

  return failed_test_case_count;
}
