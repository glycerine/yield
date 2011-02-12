#include "yunit.hpp"

#include <iostream>


extern yunit::TestSuite& FileBIOQueueTestSuite();
#ifdef __unix__
  extern yunit::TestSuite& POSIXFileAIOQueueTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& Win32FileAIOQueueTestSuite();
#endif
extern yunit::TestSuite& SocketBIOQueueTestSuite();
extern yunit::TestSuite& SocketNBIOQueueTestSuite();
#ifdef _WIN32
  extern yunit::TestSuite& Win32SocketAIOQueueTestSuite();
#endif


int main(int, char**) {
  int failed_test_case_count = 0;

  // FileBIOQueue
  std::cout << "FileBIOQueue:" << std::endl;
  failed_test_case_count += FileBIOQueueTestSuite().run();
  std::cout << std::endl;

  #ifdef __unix__
    // POSIXFileAIOQueue
    std::cout << "POSIXFileAIOQueue:" << std::endl;
    failed_test_case_count += POSIXFileAIOQueueTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef _WIN32
    // Win32FileAIOQueue
    std::cout << "Win32FileAIOQueue:" << std::endl;
    failed_test_case_count += Win32FileAIOQueueTestSuite().run();
    std::cout << std::endl;
  #endif

  // SocketBIOQueue
  std::cout << "SocketBIOQueue:" << std::endl;
  failed_test_case_count += SocketBIOQueueTestSuite().run();
  std::cout << std::endl;

  // SocketNBIOQueue
  std::cout << "SocketNBIOQueue:" << std::endl;
  failed_test_case_count += SocketNBIOQueueTestSuite().run();
  std::cout << std::endl;

  #ifdef _WIN32
    // Win32SocketAIOQueue
    std::cout << "Win32SocketAIOQueue:" << std::endl;
    failed_test_case_count += Win32SocketAIOQueueTestSuite().run();
    std::cout << std::endl;
  #endif

  return failed_test_case_count;
}

