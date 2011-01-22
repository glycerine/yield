// yield/thread/yield_thread_test_main.cpp

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


extern yunit::TestSuite& BlockingConcurrentQueueTestSuite();
extern yunit::TestSuite& ConditionVariableTestSuite();
extern yunit::TestSuite& LightweightMutexTestSuite();
extern yunit::TestSuite& MutexTestSuite();
extern yunit::TestSuite& NonBlockingConcurrentQueueTestSuite();
extern yunit::TestSuite& PerformanceCounterSetTestSuite();
extern yunit::TestSuite& PipeTestSuite();
extern yunit::TestSuite& ProcessorSetTestSuite();
extern yunit::TestSuite& ReaderWriterLockTestSuite();
extern yunit::TestSuite& SamplerTestSuite();
extern yunit::TestSuite& SemaphoreTestSuite();
extern yunit::TestSuite& SynchronizedQueueTestSuite();
extern yunit::TestSuite& ThreadTestSuite();
extern yunit::TestSuite& TLSConcurrentQueueTestSuite();
extern yunit::TestSuite& UnitConcurrentQueueTestSuite();


int main(int, char**) {
  int failed_test_case_count = 0;

  // BlockingConcurrentQueue
  std::cout << "BlockingConcurrentQueue:" << std::endl;
  failed_test_case_count += BlockingConcurrentQueueTestSuite().run();
  std::cout << std::endl;

  // ConditionVariable
  std::cout << "ConditionVariable:" << std::endl;
  failed_test_case_count += ConditionVariableTestSuite().run();
  std::cout << std::endl;

  // LightweightMutex
  std::cout << "LightweightMutex:" << std::endl;
  failed_test_case_count += LightweightMutexTestSuite().run();
  std::cout << std::endl;

  // Mutex
  std::cout << "Mutex:" << std::endl;
  failed_test_case_count += MutexTestSuite().run();
  std::cout << std::endl;

  // NonBlockingConcurrentQueue
  std::cout << "NonBlockingConcurrentQueue:" << std::endl;
  failed_test_case_count += NonBlockingConcurrentQueueTestSuite().run();
  std::cout << std::endl;

  // PerformanceCounterSet
  std::cout << "PerformanceCounterSet:" << std::endl;
  failed_test_case_count += PerformanceCounterSetTestSuite().run();
  std::cout << std::endl;

  // Pipe
  std::cout << "Pipe:" << std::endl;
  failed_test_case_count += PipeTestSuite().run();
  std::cout << std::endl;

  // ProcessorSet
  std::cout << "ProcessorSet:" << std::endl;
  failed_test_case_count += ProcessorSetTestSuite().run();
  std::cout << std::endl;

  // ReaderWriterLock
  std::cout << "ReaderWriterLock:" << std::endl;
  failed_test_case_count += ReaderWriterLockTestSuite().run();
  std::cout << std::endl;

  // Sampler
  std::cout << "Sampler:" << std::endl;
  failed_test_case_count += SamplerTestSuite().run();
  std::cout << std::endl;

  // Semaphore
  std::cout << "Semaphore:" << std::endl;
  failed_test_case_count += SemaphoreTestSuite().run();
  std::cout << std::endl;

  // SynchronizedQueue
  std::cout << "SynchronizedQueue:" << std::endl;
  failed_test_case_count += SynchronizedQueueTestSuite().run();
  std::cout << std::endl;

  // Thread
  std::cout << "Thread:" << std::endl;
  failed_test_case_count += ThreadTestSuite().run();
  std::cout << std::endl;

  // TLSConcurrentQueue
  std::cout << "TLSConcurrentQueue:" << std::endl;
  failed_test_case_count += TLSConcurrentQueueTestSuite().run();
  std::cout << std::endl;

  // UnitConcurrentQueue
  std::cout << "UnitConcurrentQueue:" << std::endl;
  failed_test_case_count += UnitConcurrentQueueTestSuite().run();
  std::cout << std::endl;

  return failed_test_case_count;
}
