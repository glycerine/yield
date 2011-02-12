#include "yunit.hpp"

#include <iostream>


extern yunit::TestSuite& DirectoryTestSuite();
extern yunit::TestSuite& MemoryMappedFileTestSuite();
extern yunit::TestSuite& NamedPipeTestSuite();
extern yunit::TestSuite& PathTestSuite();
#ifdef __unix__
  extern yunit::TestSuite& POSIXFileSystemTestSuite();
#endif
#ifdef __unix__
  extern yunit::TestSuite& FileTestSuite();
#endif
#ifdef __unix__
  extern yunit::TestSuite& POSIXStatTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& Win32FileSystemTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& Win32FileTestSuite();
#endif
#ifdef _WIN32
  extern yunit::TestSuite& Win32StatTestSuite();
#endif


int main(int, char**) {
  int failed_test_case_count = 0;

  // Directory
  std::cout << "Directory:" << std::endl;
  failed_test_case_count += DirectoryTestSuite().run();
  std::cout << std::endl;

  // MemoryMappedFile
  std::cout << "MemoryMappedFile:" << std::endl;
  failed_test_case_count += MemoryMappedFileTestSuite().run();
  std::cout << std::endl;

  // NamedPipe
  std::cout << "NamedPipe:" << std::endl;
  failed_test_case_count += NamedPipeTestSuite().run();
  std::cout << std::endl;

  // Path
  std::cout << "Path:" << std::endl;
  failed_test_case_count += PathTestSuite().run();
  std::cout << std::endl;

  #ifdef __unix__
    // POSIXFileSystem
    std::cout << "POSIXFileSystem:" << std::endl;
    failed_test_case_count += POSIXFileSystemTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __unix__
    // File
    std::cout << "File:" << std::endl;
    failed_test_case_count += FileTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef __unix__
    // POSIXStat
    std::cout << "POSIXStat:" << std::endl;
    failed_test_case_count += POSIXStatTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef _WIN32
    // Win32FileSystem
    std::cout << "Win32FileSystem:" << std::endl;
    failed_test_case_count += Win32FileSystemTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef _WIN32
    // Win32File
    std::cout << "Win32File:" << std::endl;
    failed_test_case_count += Win32FileTestSuite().run();
    std::cout << std::endl;
  #endif

  #ifdef _WIN32
    // Win32Stat
    std::cout << "Win32Stat:" << std::endl;
    failed_test_case_count += Win32StatTestSuite().run();
    std::cout << std::endl;
  #endif

  return failed_test_case_count;
}

