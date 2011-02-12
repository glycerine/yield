#include "yunit.hpp"

#include <iostream>


#if defined(__MACH__) || defined(__FreeBSD__)
  extern yunit::TestSuite& KqueueTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& HandleEventQueueTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& SelectorTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& WSAPollerTestSuite();
#endif
#ifdef __linux__
  extern yunit::TestSuite& EPollerTestSuite();
#endif
#ifdef __unix__
  extern yunit::TestSuite& PollerTestSuite();
#endif
#ifdef __sun
  extern yunit::TestSuite& EventPortTestSuite();
#endif


int main(int, char**) {
  int failed_test_case_count = 0;

  #if defined(__MACH__) || defined(__FreeBSD__)
    // Kqueue
    std::cout << "Kqueue:" << std::endl;
    failed_test_case_count += KqueueTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef _WIN32
    // HandleEventQueue
    std::cout << "HandleEventQueue:" << std::endl;
    failed_test_case_count += HandleEventQueueTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef _WIN32
    // Selector
    std::cout << "Selector:" << std::endl;
    failed_test_case_count += SelectorTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef _WIN32
    // WSAPoller
    std::cout << "WSAPoller:" << std::endl;
    failed_test_case_count += WSAPollerTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __linux__
    // EPoller
    std::cout << "EPoller:" << std::endl;
    failed_test_case_count += EPollerTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __unix__
    // Poller
    std::cout << "Poller:" << std::endl;
    failed_test_case_count += PollerTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __sun
    // EventPort
    std::cout << "EventPort:" << std::endl;
    failed_test_case_count += EventPortTestSuite().run();
    std::cout << std::endl;
  #endif

  return failed_test_case_count;
}

