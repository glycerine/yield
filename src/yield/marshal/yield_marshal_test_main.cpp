#include "yunit.hpp"

#include <iostream>


extern yunit::TestSuite& PrettyPrinterTestSuite();
extern yunit::TestSuite& XDRMarshallerTestSuite();
extern yunit::TestSuite& XMLMarshallerTestSuite();
extern yunit::TestSuite& JSONMarshallerTestSuite();


int main(int, char**) {
  int failed_test_case_count = 0;

  // PrettyPrinter
  std::cout << "PrettyPrinter:" << std::endl;
  failed_test_case_count += PrettyPrinterTestSuite().run();
  std::cout << std::endl;

  // XDRMarshaller
  std::cout << "XDRMarshaller:" << std::endl;
  failed_test_case_count += XDRMarshallerTestSuite().run();
  std::cout << std::endl;

  // XMLMarshaller
  std::cout << "XMLMarshaller:" << std::endl;
  failed_test_case_count += XMLMarshallerTestSuite().run();
  std::cout << std::endl;

  // JSONMarshaller
  std::cout << "JSONMarshaller:" << std::endl;
  failed_test_case_count += JSONMarshallerTestSuite().run();
  std::cout << std::endl;

  return failed_test_case_count;
}

