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

