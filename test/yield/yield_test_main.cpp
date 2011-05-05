// yield_test_main.cpp

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

extern yunit::TestSuite& atomicTestSuite();
extern yunit::TestSuite& auto_ObjectTestSuite();
extern yunit::TestSuite& BuffersTestSuite();
extern yunit::TestSuite& BufferTestSuite();
extern yunit::TestSuite& ChannelTestSuite();
extern yunit::TestSuite& DateTimeTestSuite();
extern yunit::TestSuite& EventTestSuite();
extern yunit::TestSuite& ExceptionTestSuite();
extern yunit::TestSuite& LogTestSuite();
extern yunit::TestSuite& MessageTestSuite();
extern yunit::TestSuite& ObjectTestSuite();
extern yunit::TestSuite& RequestTestSuite();
extern yunit::TestSuite& ResponseTestSuite();
extern yunit::TestSuite& TimeTestSuite();

int main(int, char**) {
  int failed_test_case_count = 0;

  // atomic
  std::cout << "atomic:" << std::endl;
  failed_test_case_count += atomicTestSuite().run();
  std::cout << std::endl;

  // auto_Object
  std::cout << "auto_Object:" << std::endl;
  failed_test_case_count += auto_ObjectTestSuite().run();
  std::cout << std::endl;

  // Buffers
  std::cout << "Buffers:" << std::endl;
  failed_test_case_count += BuffersTestSuite().run();
  std::cout << std::endl;

  // Buffer
  std::cout << "Buffer:" << std::endl;
  failed_test_case_count += BufferTestSuite().run();
  std::cout << std::endl;

  // Channel
  std::cout << "Channel:" << std::endl;
  failed_test_case_count += ChannelTestSuite().run();
  std::cout << std::endl;

  // DateTime
  std::cout << "DateTime:" << std::endl;
  failed_test_case_count += DateTimeTestSuite().run();
  std::cout << std::endl;

  // Event
  std::cout << "Event:" << std::endl;
  failed_test_case_count += EventTestSuite().run();
  std::cout << std::endl;

  // Exception
  std::cout << "Exception:" << std::endl;
  failed_test_case_count += ExceptionTestSuite().run();
  std::cout << std::endl;

  // Log
  std::cout << "Log:" << std::endl;
  failed_test_case_count += LogTestSuite().run();
  std::cout << std::endl;

  // Message
  std::cout << "Message:" << std::endl;
  failed_test_case_count += MessageTestSuite().run();
  std::cout << std::endl;

  // Object
  std::cout << "Object:" << std::endl;
  failed_test_case_count += ObjectTestSuite().run();
  std::cout << std::endl;

  // Request
  std::cout << "Request:" << std::endl;
  failed_test_case_count += RequestTestSuite().run();
  std::cout << std::endl;

  // Response
  std::cout << "Response:" << std::endl;
  failed_test_case_count += ResponseTestSuite().run();
  std::cout << std::endl;

  // Time
  std::cout << "Time:" << std::endl;
  failed_test_case_count += TimeTestSuite().run();
  std::cout << std::endl;

  return failed_test_case_count;
}
