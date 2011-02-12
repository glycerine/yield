#include "yunit.hpp"

#include <iostream>


extern yunit::TestSuite& ProcessTestSuite();
extern yunit::TestSuite& SharedLibraryTestSuite();


int main(int, char**) {
  int failed_test_case_count = 0;

  // Process
  std::cout << "Process:" << std::endl;
  failed_test_case_count += ProcessTestSuite().run();
  std::cout << std::endl;

  // SharedLibrary
  std::cout << "SharedLibrary:" << std::endl;
  failed_test_case_count += SharedLibraryTestSuite().run();
  std::cout << std::endl;

  return failed_test_case_count;
}

