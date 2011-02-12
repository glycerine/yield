#include "yunit.hpp"

#include <iostream>


extern yunit::TestSuite& SEDAStageSchedulerTestSuite();
extern yunit::TestSuite& StageTestSuite();
extern yunit::TestSuite& SynchronizedResponseQueueTestSuite();
extern yunit::TestSuite& WavefrontStageSchedulerTestSuite();
extern yunit::TestSuite& SynchronizedEventQueueTestSuite();


int main(int, char**) {
  int failed_test_case_count = 0;

  // SEDAStageScheduler
  std::cout << "SEDAStageScheduler:" << std::endl;
  failed_test_case_count += SEDAStageSchedulerTestSuite().run();
  std::cout << std::endl;

  // Stage
  std::cout << "Stage:" << std::endl;
  failed_test_case_count += StageTestSuite().run();
  std::cout << std::endl;

  // SynchronizedResponseQueue
  std::cout << "SynchronizedResponseQueue:" << std::endl;
  failed_test_case_count += SynchronizedResponseQueueTestSuite().run();
  std::cout << std::endl;

  // WavefrontStageScheduler
  std::cout << "WavefrontStageScheduler:" << std::endl;
  failed_test_case_count += WavefrontStageSchedulerTestSuite().run();
  std::cout << std::endl;

  // SynchronizedEventQueue
  std::cout << "SynchronizedEventQueue:" << std::endl;
  failed_test_case_count += SynchronizedEventQueueTestSuite().run();
  std::cout << std::endl;

  return failed_test_case_count;
}

