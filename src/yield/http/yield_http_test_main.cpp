#include "yunit.hpp"

#include <iostream>


extern yunit::TestSuite& HTTPClientTestSuite();
extern yunit::TestSuite& HTTPRequestParserTestSuite();
extern yunit::TestSuite& HTTPResponseParserTestSuite();
extern yunit::TestSuite& HTTPServerTestSuite();


int main(int, char**) {
  int failed_test_case_count = 0;

  // HTTPClient
  std::cout << "HTTPClient:" << std::endl;
  failed_test_case_count += HTTPClientTestSuite().run();
  std::cout << std::endl;

  // HTTPRequestParser
  std::cout << "HTTPRequestParser:" << std::endl;
  failed_test_case_count += HTTPRequestParserTestSuite().run();
  std::cout << std::endl;

  // HTTPResponseParser
  std::cout << "HTTPResponseParser:" << std::endl;
  failed_test_case_count += HTTPResponseParserTestSuite().run();
  std::cout << std::endl;

  // HTTPServer
  std::cout << "HTTPServer:" << std::endl;
  failed_test_case_count += HTTPServerTestSuite().run();
  std::cout << std::endl;

  return failed_test_case_count;
}

