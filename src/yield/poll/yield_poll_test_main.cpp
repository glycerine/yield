// yield/poll/yield_poll_test_main.cpp

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


#if defined(__MACH__) || defined(__FreeBSD__)
  extern yunit::TestSuite& KqueueTestSuite();
#endif
#ifdef __linux__
  extern yunit::TestSuite& EPollerTestSuite();
#endif
#ifdef __unix__
  extern yunit::TestSuite& PollerTestSuite();
#endif
#ifdef __sun
  extern yunit::TestSuite& EventPortTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& HandleEventQueueTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& SelectorTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& WSAPollerTestSuite();
#endif


int main(int, char**) {
  int failed_test_case_count = 0;

  #if defined(__MACH__) || defined(__FreeBSD__)
    // Kqueue
    std::cout << "Kqueue:" << std::endl;
    failed_test_case_count += KqueueTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __linux__
    // EPoller
    std::cout << "EPoller:" << std::endl;
    failed_test_case_count += EPollerTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __unix__
    // Poller
    std::cout << "Poller:" << std::endl;
    failed_test_case_count += PollerTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __sun
    // EventPort
    std::cout << "EventPort:" << std::endl;
    failed_test_case_count += EventPortTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef _WIN32
    // HandleEventQueue
    std::cout << "HandleEventQueue:" << std::endl;
    failed_test_case_count += HandleEventQueueTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef _WIN32
    // Selector
    std::cout << "Selector:" << std::endl;
    failed_test_case_count += SelectorTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef _WIN32
    // WSAPoller
    std::cout << "WSAPoller:" << std::endl;
    failed_test_case_count += WSAPollerTestSuite().run();
    std::cout << std::endl;
  #endif

  return failed_test_case_count;
}
