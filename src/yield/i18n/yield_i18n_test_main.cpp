#include "yunit.hpp"

#include <iostream>


extern yunit::TestSuite& iconvTestSuite();
extern yunit::TestSuite& tstringTestSuite();


int main(int, char**) {
  int failed_test_case_count = 0;

  // iconv
  std::cout << "iconv:" << std::endl;
  failed_test_case_count += iconvTestSuite().run();
  std::cout << std::endl;

  // tstring
  std::cout << "tstring:" << std::endl;
  failed_test_case_count += tstringTestSuite().run();
  std::cout << std::endl;

  return failed_test_case_count;
}

