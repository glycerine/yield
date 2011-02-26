# SHELL = /bin/bash
UNAME := $(shell uname)

CXXFLAGS += -I../../../include
ifeq ($(UNAME), Linux)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), Darwin)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), Solaris)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), MINGW32)
	CXXFLAGS += /EHsc /GR- /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
endif
ifneq ($COVERAGE,)
	CXXFLAGS += -fprofile-arcs -ftest-coverage
	LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
endif
ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif


LDFLAGS += -L../../../lib/yield
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Darwin)
	LIBS += -liconv
endif
ifeq ($(UNAME), FreeBSD)
	LIBS += -liconv -lintl
endif
ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lm -lrt -lstdc++
endif
LIBS += -lyield_fs -lyield_i18n -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/fs_test -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/fs_test/directory_test.o ../../../build/yield/fs_test/memory_mapped_file_test.o ../../../build/yield/fs_test/named_pipe_test.o ../../../build/yield/fs_test/path_test.o ../../../build/yield/fs_test/yield_fs_test_main.o
ifeq ($(UNAME), Darwin)
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/posix/file_system_test.o ../../../build/yield/fs_test/posix/file_test.o ../../../build/yield/fs_test/posix/stat_test.o
endif
ifeq ($(UNAME), FreeBSD)
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/posix/file_system_test.o ../../../build/yield/fs_test/posix/file_test.o ../../../build/yield/fs_test/posix/stat_test.o
endif
ifeq ($(UNAME), Linux)
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/posix/file_system_test.o ../../../build/yield/fs_test/posix/file_test.o ../../../build/yield/fs_test/posix/stat_test.o
endif
ifeq ($(UNAME), Solaris)
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/posix/file_system_test.o ../../../build/yield/fs_test/posix/file_test.o ../../../build/yield/fs_test/posix/stat_test.o
endif
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/win32/file_system_test.o ../../../build/yield/fs_test/win32/file_test.o ../../../build/yield/fs_test/win32/stat_test.o
endif


../../../bin/yield/yield_fs_test: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../bin/yield/yield_fs_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/fs_test/directory_test.o: ../../../src/yield/fs/directory_test.cpp
	-mkdir -p ../../../build/yield/fs_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/directory_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/directory_test.cpp

../../../build/yield/fs_test/memory_mapped_file_test.o: ../../../src/yield/fs/memory_mapped_file_test.cpp
	-mkdir -p ../../../build/yield/fs_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/memory_mapped_file_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/memory_mapped_file_test.cpp

../../../build/yield/fs_test/named_pipe_test.o: ../../../src/yield/fs/named_pipe_test.cpp
	-mkdir -p ../../../build/yield/fs_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/named_pipe_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/named_pipe_test.cpp

../../../build/yield/fs_test/path_test.o: ../../../src/yield/fs/path_test.cpp
	-mkdir -p ../../../build/yield/fs_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/path_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/path_test.cpp

../../../build/yield/fs_test/posix/file_system_test.o: ../../../src/yield/fs/posix/file_system_test.cpp
	-mkdir -p ../../../build/yield/fs_test/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/posix/file_system_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/file_system_test.cpp

../../../build/yield/fs_test/posix/file_test.o: ../../../src/yield/fs/posix/file_test.cpp
	-mkdir -p ../../../build/yield/fs_test/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/posix/file_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/file_test.cpp

../../../build/yield/fs_test/posix/stat_test.o: ../../../src/yield/fs/posix/stat_test.cpp
	-mkdir -p ../../../build/yield/fs_test/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/posix/stat_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/stat_test.cpp

../../../build/yield/fs_test/win32/file_system_test.o: ../../../src/yield/fs/win32/file_system_test.cpp
	-mkdir -p ../../../build/yield/fs_test/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/win32/file_system_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/file_system_test.cpp

../../../build/yield/fs_test/win32/file_test.o: ../../../src/yield/fs/win32/file_test.cpp
	-mkdir -p ../../../build/yield/fs_test/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/win32/file_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/file_test.cpp

../../../build/yield/fs_test/win32/stat_test.o: ../../../src/yield/fs/win32/stat_test.cpp
	-mkdir -p ../../../build/yield/fs_test/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/win32/stat_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/stat_test.cpp

../../../build/yield/fs_test/yield_fs_test_main.o: ../../../src/yield/fs/yield_fs_test_main.cpp
	-mkdir -p ../../../build/yield/fs_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/yield_fs_test_main.o -MD $(CXXFLAGS) ../../../src/yield/fs/yield_fs_test_main.cpp


