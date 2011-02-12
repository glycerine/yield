#include "yunit.hpp"

#include <iostream>


extern yunit::TestSuite& UUIDTestSuite();
extern yunit::TestSuite& URITestSuite();
extern yunit::TestSuite& UDPSocketTestSuite();
extern yunit::TestSuite& SocketPairTestSuite();
extern yunit::TestSuite& SocketAddressTestSuite();
extern yunit::TestSuite& TCPSocketTestSuite();
extern yunit::TestSuite& SSLContextTestSuite();


int main(int, char**) {
  int failed_test_case_count = 0;

  // UUID
  std::cout << "UUID:" << std::endl;
  failed_test_case_count += UUIDTestSuite().run();
  std::cout << std::endl;

  // URI
  std::cout << "URI:" << std::endl;
  failed_test_case_count += URITestSuite().run();
  std::cout << std::endl;

  // UDPSocket
  std::cout << "UDPSocket:" << std::endl;
  failed_test_case_count += UDPSocketTestSuite().run();
  std::cout << std::endl;

  // SocketPair
  std::cout << "SocketPair:" << std::endl;
  failed_test_case_count += SocketPairTestSuite().run();
  std::cout << std::endl;

  // SocketAddress
  std::cout << "SocketAddress:" << std::endl;
  failed_test_case_count += SocketAddressTestSuite().run();
  std::cout << std::endl;

  // TCPSocket
  std::cout << "TCPSocket:" << std::endl;
  failed_test_case_count += TCPSocketTestSuite().run();
  std::cout << std::endl;

  // SSLContext
  std::cout << "SSLContext:" << std::endl;
  failed_test_case_count += SSLContextTestSuite().run();
  std::cout << std::endl;

  return failed_test_case_count;
}

