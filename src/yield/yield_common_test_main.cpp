#include "yunit.hpp"

#include <iostream>


extern yunit::TestSuite& StringBufferTestSuite();
extern yunit::TestSuite& ExceptionTestSuite();
extern yunit::TestSuite& PageTestSuite();
extern yunit::TestSuite& auto_ObjectTestSuite();
extern yunit::TestSuite& ObjectTestSuite();
extern yunit::TestSuite& TimeTestSuite();
extern yunit::TestSuite& RequestTestSuite();
extern yunit::TestSuite& FixedBufferTestSuite();
extern yunit::TestSuite& ResponseTestSuite();
extern yunit::TestSuite& MessageTestSuite();
extern yunit::TestSuite& EventTestSuite();
extern yunit::TestSuite& DateTimeTestSuite();
extern yunit::TestSuite& LogTestSuite();
extern yunit::TestSuite& atomicTestSuite();


int main(int, char**) {
  int failed_test_case_count = 0;

  // StringBuffer
  std::cout << "StringBuffer:" << std::endl;
  failed_test_case_count += StringBufferTestSuite().run();
  std::cout << std::endl;

  // Exception
  std::cout << "Exception:" << std::endl;
  failed_test_case_count += ExceptionTestSuite().run();
  std::cout << std::endl;

  // Page
  std::cout << "Page:" << std::endl;
  failed_test_case_count += PageTestSuite().run();
  std::cout << std::endl;

  // auto_Object
  std::cout << "auto_Object:" << std::endl;
  failed_test_case_count += auto_ObjectTestSuite().run();
  std::cout << std::endl;

  // Object
  std::cout << "Object:" << std::endl;
  failed_test_case_count += ObjectTestSuite().run();
  std::cout << std::endl;

  // Time
  std::cout << "Time:" << std::endl;
  failed_test_case_count += TimeTestSuite().run();
  std::cout << std::endl;

  // Request
  std::cout << "Request:" << std::endl;
  failed_test_case_count += RequestTestSuite().run();
  std::cout << std::endl;

  // FixedBuffer
  std::cout << "FixedBuffer:" << std::endl;
  failed_test_case_count += FixedBufferTestSuite().run();
  std::cout << std::endl;

  // Response
  std::cout << "Response:" << std::endl;
  failed_test_case_count += ResponseTestSuite().run();
  std::cout << std::endl;

  // Message
  std::cout << "Message:" << std::endl;
  failed_test_case_count += MessageTestSuite().run();
  std::cout << std::endl;

  // Event
  std::cout << "Event:" << std::endl;
  failed_test_case_count += EventTestSuite().run();
  std::cout << std::endl;

  // DateTime
  std::cout << "DateTime:" << std::endl;
  failed_test_case_count += DateTimeTestSuite().run();
  std::cout << std::endl;

  // Log
  std::cout << "Log:" << std::endl;
  failed_test_case_count += LogTestSuite().run();
  std::cout << std::endl;

  // atomic
  std::cout << "atomic:" << std::endl;
  failed_test_case_count += atomicTestSuite().run();
  std::cout << std::endl;

  return failed_test_case_count;
}

