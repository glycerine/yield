// yield_queue_test_main.cpp

// Copyright (c) 2012 Minor Gordon
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

extern yunit::TestSuite& BlockingConcurrentQueueTestSuite();
extern yunit::TestSuite& NonBlockingConcurrentQueueTestSuite();
extern yunit::TestSuite& RendezvousConcurrentQueueTestSuite();
extern yunit::TestSuite& STLEventQueueTestSuite();
extern yunit::TestSuite& SynchronizedEventQueueTestSuite();
extern yunit::TestSuite& SynchronizedQueueTestSuite();
extern yunit::TestSuite& TLSConcurrentQueueTestSuite();

int main(int, char**) {
  int failed_test_case_count = 0;

  // BlockingConcurrentQueue
  std::cout << "BlockingConcurrentQueue:" << std::endl;
  failed_test_case_count += BlockingConcurrentQueueTestSuite().run();
  std::cout << std::endl;

  // NonBlockingConcurrentQueue
  std::cout << "NonBlockingConcurrentQueue:" << std::endl;
  failed_test_case_count += NonBlockingConcurrentQueueTestSuite().run();
  std::cout << std::endl;

  // RendezvousConcurrentQueue
  std::cout << "RendezvousConcurrentQueue:" << std::endl;
  failed_test_case_count += RendezvousConcurrentQueueTestSuite().run();
  std::cout << std::endl;

  // STLEventQueue
  std::cout << "STLEventQueue:" << std::endl;
  failed_test_case_count += STLEventQueueTestSuite().run();
  std::cout << std::endl;

  // SynchronizedEventQueue
  std::cout << "SynchronizedEventQueue:" << std::endl;
  failed_test_case_count += SynchronizedEventQueueTestSuite().run();
  std::cout << std::endl;

  // SynchronizedQueue
  std::cout << "SynchronizedQueue:" << std::endl;
  failed_test_case_count += SynchronizedQueueTestSuite().run();
  std::cout << std::endl;

  // TLSConcurrentQueue
  std::cout << "TLSConcurrentQueue:" << std::endl;
  failed_test_case_count += TLSConcurrentQueueTestSuite().run();
  std::cout << std::endl;

  return failed_test_case_count;
}
