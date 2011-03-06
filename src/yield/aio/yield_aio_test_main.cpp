// yield/aio/yield_aio_test_main.cpp

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


extern yunit::TestSuite& FileBIOQueueTestSuite();
extern yunit::TestSuite& SocketBIOQueueTestSuite();
extern yunit::TestSuite& SocketNBIOQueueTestSuite();
#ifdef __unix__
extern yunit::TestSuite& POSIXFileAIOQueueTestSuite();
#endif
#ifdef _WIN32
extern yunit::TestSuite& Win32FileAIOQueueTestSuite();
#endif
#ifdef _WIN32
extern yunit::TestSuite& Win32SocketAIOQueueTestSuite();
#endif


int main(int, char**) {
  int failed_test_case_count = 0;

  // FileBIOQueue
  std::cout << "FileBIOQueue:" << std::endl;
  failed_test_case_count += FileBIOQueueTestSuite().run();
  std::cout << std::endl;

  // SocketBIOQueue
  std::cout << "SocketBIOQueue:" << std::endl;
  failed_test_case_count += SocketBIOQueueTestSuite().run();
  std::cout << std::endl;

  // SocketNBIOQueue
  std::cout << "SocketNBIOQueue:" << std::endl;
  failed_test_case_count += SocketNBIOQueueTestSuite().run();
  std::cout << std::endl;

#ifdef __unix__
  // POSIXFileAIOQueue
  std::cout << "POSIXFileAIOQueue:" << std::endl;
  failed_test_case_count += POSIXFileAIOQueueTestSuite().run();
  std::cout << std::endl;
#endif

#ifdef _WIN32
  // Win32FileAIOQueue
  std::cout << "Win32FileAIOQueue:" << std::endl;
  failed_test_case_count += Win32FileAIOQueueTestSuite().run();
  std::cout << std::endl;
#endif

#ifdef _WIN32
  // Win32SocketAIOQueue
  std::cout << "Win32SocketAIOQueue:" << std::endl;
  failed_test_case_count += Win32SocketAIOQueueTestSuite().run();
  std::cout << std::endl;
#endif

  return failed_test_case_count;
}
