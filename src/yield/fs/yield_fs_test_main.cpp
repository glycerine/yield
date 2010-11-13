// yield/fs/yield_fs_test_main.cpp

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


#include "yunit.hpp"

#include <iostream>


extern yunit::TestSuite& MemoryMappedFileTestSuite();
extern yunit::TestSuite& NamedPipeTestSuite();
extern yunit::TestSuite& PathTestSuite();
#if defined(__MACH__) || defined(__FreeBSD__)
  extern yunit::TestSuite& BSDDirectoryTestSuite();
#endif
#if defined(__MACH__) || defined(__FreeBSD__)
  extern yunit::TestSuite& BSDVolumeTestSuite();
#endif
#ifdef __MACH__
  extern yunit::TestSuite& DarwinFileTestSuite();
#endif
#ifdef __MACH__
  extern yunit::TestSuite& DarwinVolumeTestSuite();
#endif
#ifdef __FreeBSD__
  extern yunit::TestSuite& FreeBSDFileTestSuite();
#endif
#ifdef __FreeBSD__
  extern yunit::TestSuite& FreeBSDVolumeTestSuite();
#endif
#ifdef __linux__
  extern yunit::TestSuite& LinuxDirectoryTestSuite();
#endif
#ifdef __linux__
  extern yunit::TestSuite& LinuxFileTestSuite();
#endif
#ifdef __linux__
  extern yunit::TestSuite& LinuxVolumeTestSuite();
#endif
#ifdef __unix__
  extern yunit::TestSuite& POSIXDirectoryTestSuite();
#endif
#ifdef __unix__
  extern yunit::TestSuite& POSIXFileTestSuite();
#endif
#ifdef __unix__
  extern yunit::TestSuite& POSIXStatTestSuite();
#endif
#ifdef __unix__
  extern yunit::TestSuite& POSIXVolumeTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& Win32DirectoryTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& Win32FileTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& Win32StatTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& Win32VolumeTestSuite();
#endif


int main( int, char** )
{
  int failed_test_case_count = 0;

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

  #if defined(__MACH__) || defined(__FreeBSD__)
    // BSDDirectory
    std::cout << "BSDDirectory:" << std::endl;
    failed_test_case_count += BSDDirectoryTestSuite().run();
    std::cout << std::endl;
  #endif

  #if defined(__MACH__) || defined(__FreeBSD__)
    // BSDVolume
    std::cout << "BSDVolume:" << std::endl;
    failed_test_case_count += BSDVolumeTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __MACH__
    // DarwinFile
    std::cout << "DarwinFile:" << std::endl;
    failed_test_case_count += DarwinFileTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __MACH__
    // DarwinVolume
    std::cout << "DarwinVolume:" << std::endl;
    failed_test_case_count += DarwinVolumeTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __FreeBSD__
    // FreeBSDFile
    std::cout << "FreeBSDFile:" << std::endl;
    failed_test_case_count += FreeBSDFileTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __FreeBSD__
    // FreeBSDVolume
    std::cout << "FreeBSDVolume:" << std::endl;
    failed_test_case_count += FreeBSDVolumeTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __linux__
    // LinuxDirectory
    std::cout << "LinuxDirectory:" << std::endl;
    failed_test_case_count += LinuxDirectoryTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __linux__
    // LinuxFile
    std::cout << "LinuxFile:" << std::endl;
    failed_test_case_count += LinuxFileTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __linux__
    // LinuxVolume
    std::cout << "LinuxVolume:" << std::endl;
    failed_test_case_count += LinuxVolumeTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __unix__
    // POSIXDirectory
    std::cout << "POSIXDirectory:" << std::endl;
    failed_test_case_count += POSIXDirectoryTestSuite().run();
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

  #ifdef __unix__
    // POSIXVolume
    std::cout << "POSIXVolume:" << std::endl;
    failed_test_case_count += POSIXVolumeTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef _WIN32
    // Win32Directory
    std::cout << "Win32Directory:" << std::endl;
    failed_test_case_count += Win32DirectoryTestSuite().run();
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

  #ifdef _WIN32
    // Win32Volume
    std::cout << "Win32Volume:" << std::endl;
    failed_test_case_count += Win32VolumeTestSuite().run();
    std::cout << std::endl;
  #endif

  return failed_test_case_count;
}
