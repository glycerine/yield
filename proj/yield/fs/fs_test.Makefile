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


LDFLAGS += -L../../../lib
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


OBJECT_FILE_PATHS += ../../../build/yield/fs_test/memory_mapped_file_test.o ../../../build/yield/fs_test/named_pipe_test.o ../../../build/yield/fs_test/path_test.o ../../../build/yield/fs_test/yield_fs_test_main.o
ifeq ($(UNAME), Darwin)
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/bsd/directory_test.o ../../../build/yield/fs_test/bsd/volume_test.o
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/darwin/file_test.o ../../../build/yield/fs_test/darwin/volume_test.o
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/posix/directory_test.o ../../../build/yield/fs_test/posix/file_test.o ../../../build/yield/fs_test/posix/stat_test.o ../../../build/yield/fs_test/posix/volume_test.o
endif
ifeq ($(UNAME), FreeBSD)
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/bsd/directory_test.o ../../../build/yield/fs_test/bsd/volume_test.o
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/freebsd/file_test.o ../../../build/yield/fs_test/freebsd/volume_test.o
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/posix/directory_test.o ../../../build/yield/fs_test/posix/file_test.o ../../../build/yield/fs_test/posix/stat_test.o ../../../build/yield/fs_test/posix/volume_test.o
endif
ifeq ($(UNAME), Linux)
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/linux/directory_test.o ../../../build/yield/fs_test/linux/file_test.o ../../../build/yield/fs_test/linux/volume_test.o
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/posix/directory_test.o ../../../build/yield/fs_test/posix/file_test.o ../../../build/yield/fs_test/posix/stat_test.o ../../../build/yield/fs_test/posix/volume_test.o
endif
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/win32/directory_test.o ../../../build/yield/fs_test/win32/file_test.o ../../../build/yield/fs_test/win32/stat_test.o ../../../build/yield/fs_test/win32/volume_test.o
endif
ifeq ($(UNAME), Solaris)
	OBJECT_FILE_PATHS += ../../../build/yield/fs_test/posix/directory_test.o ../../../build/yield/fs_test/posix/file_test.o ../../../build/yield/fs_test/posix/stat_test.o ../../../build/yield/fs_test/posix/volume_test.o
endif


../../../bin/yield_fs_test: $(OBJECT_FILE_PATHS)
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../bin/yield_fs_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/fs_test/bsd/directory_test.o: ../../../src/yield/fs/bsd/directory_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/bsd/directory_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/bsd/directory_test.cpp

../../../build/yield/fs_test/bsd/volume_test.o: ../../../src/yield/fs/bsd/volume_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/bsd/volume_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/bsd/volume_test.cpp

../../../build/yield/fs_test/darwin/file_test.o: ../../../src/yield/fs/darwin/file_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/darwin/file_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/darwin/file_test.cpp

../../../build/yield/fs_test/darwin/volume_test.o: ../../../src/yield/fs/darwin/volume_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/darwin/volume_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/darwin/volume_test.cpp

../../../build/yield/fs_test/freebsd/file_test.o: ../../../src/yield/fs/freebsd/file_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/freebsd/file_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/freebsd/file_test.cpp

../../../build/yield/fs_test/freebsd/volume_test.o: ../../../src/yield/fs/freebsd/volume_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/freebsd/volume_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/freebsd/volume_test.cpp

../../../build/yield/fs_test/linux/directory_test.o: ../../../src/yield/fs/linux/directory_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/linux/directory_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/linux/directory_test.cpp

../../../build/yield/fs_test/linux/file_test.o: ../../../src/yield/fs/linux/file_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/linux/file_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/linux/file_test.cpp

../../../build/yield/fs_test/linux/volume_test.o: ../../../src/yield/fs/linux/volume_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/linux/volume_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/linux/volume_test.cpp

../../../build/yield/fs_test/memory_mapped_file_test.o: ../../../src/yield/fs/memory_mapped_file_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/memory_mapped_file_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/memory_mapped_file_test.cpp

../../../build/yield/fs_test/named_pipe_test.o: ../../../src/yield/fs/named_pipe_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/named_pipe_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/named_pipe_test.cpp

../../../build/yield/fs_test/path_test.o: ../../../src/yield/fs/path_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/path_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/path_test.cpp

../../../build/yield/fs_test/posix/directory_test.o: ../../../src/yield/fs/posix/directory_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/posix/directory_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/directory_test.cpp

../../../build/yield/fs_test/posix/file_test.o: ../../../src/yield/fs/posix/file_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/posix/file_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/file_test.cpp

../../../build/yield/fs_test/posix/stat_test.o: ../../../src/yield/fs/posix/stat_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/posix/stat_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/stat_test.cpp

../../../build/yield/fs_test/posix/volume_test.o: ../../../src/yield/fs/posix/volume_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/posix/volume_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/volume_test.cpp

../../../build/yield/fs_test/win32/directory_test.o: ../../../src/yield/fs/win32/directory_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/win32/directory_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/directory_test.cpp

../../../build/yield/fs_test/win32/file_test.o: ../../../src/yield/fs/win32/file_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/win32/file_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/file_test.cpp

../../../build/yield/fs_test/win32/stat_test.o: ../../../src/yield/fs/win32/stat_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/win32/stat_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/stat_test.cpp

../../../build/yield/fs_test/win32/volume_test.o: ../../../src/yield/fs/win32/volume_test.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/win32/volume_test.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/volume_test.cpp

../../../build/yield/fs_test/yield_fs_test_main.o: ../../../src/yield/fs/yield_fs_test_main.cpp
	$(CXX) -c -o ../../../build/yield/fs_test/yield_fs_test_main.o -MD $(CXXFLAGS) ../../../src/yield/fs/yield_fs_test_main.cpp


