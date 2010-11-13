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
LIBS += -lyield_i18n -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/fs -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/fs/extended_attributes.o ../../../build/yield/fs/file.o ../../../build/yield/fs/file_log.o ../../../build/yield/fs/memory_mapped_file.o ../../../build/yield/fs/stat.o ../../../build/yield/fs/volume.o
ifeq ($(UNAME), Darwin)
	OBJECT_FILE_PATHS += ../../../build/yield/fs/bsd/directory.o ../../../build/yield/fs/bsd/volume.o
	OBJECT_FILE_PATHS += ../../../build/yield/fs/darwin/extended_attributes.o ../../../build/yield/fs/darwin/file.o ../../../build/yield/fs/darwin/volume.o
	OBJECT_FILE_PATHS += ../../../build/yield/fs/posix/directory.o ../../../build/yield/fs/posix/file.o ../../../build/yield/fs/posix/memory_mapped_file.o ../../../build/yield/fs/posix/path.o ../../../build/yield/fs/posix/stat.o ../../../build/yield/fs/posix/volume.o
endif
ifeq ($(UNAME), FreeBSD)
	OBJECT_FILE_PATHS += ../../../build/yield/fs/bsd/directory.o ../../../build/yield/fs/bsd/volume.o
	OBJECT_FILE_PATHS += ../../../build/yield/fs/freebsd/extended_attributes.o ../../../build/yield/fs/freebsd/file.o ../../../build/yield/fs/freebsd/volume.o
	OBJECT_FILE_PATHS += ../../../build/yield/fs/posix/directory.o ../../../build/yield/fs/posix/file.o ../../../build/yield/fs/posix/memory_mapped_file.o ../../../build/yield/fs/posix/path.o ../../../build/yield/fs/posix/stat.o ../../../build/yield/fs/posix/volume.o
endif
ifeq ($(UNAME), Linux)
	OBJECT_FILE_PATHS += ../../../build/yield/fs/linux/directory.o ../../../build/yield/fs/linux/extended_attributes.o ../../../build/yield/fs/linux/file.o ../../../build/yield/fs/linux/volume.o
	OBJECT_FILE_PATHS += ../../../build/yield/fs/posix/directory.o ../../../build/yield/fs/posix/file.o ../../../build/yield/fs/posix/memory_mapped_file.o ../../../build/yield/fs/posix/path.o ../../../build/yield/fs/posix/stat.o ../../../build/yield/fs/posix/volume.o
endif
ifeq ($(UNAME), Solaris)
	OBJECT_FILE_PATHS += ../../../build/yield/fs/posix/directory.o ../../../build/yield/fs/posix/file.o ../../../build/yield/fs/posix/memory_mapped_file.o ../../../build/yield/fs/posix/path.o ../../../build/yield/fs/posix/stat.o ../../../build/yield/fs/posix/volume.o
endif
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../build/yield/fs/win32/directory.o ../../../build/yield/fs/win32/file.o ../../../build/yield/fs/win32/memory_mapped_file.o ../../../build/yield/fs/win32/named_pipe.o ../../../build/yield/fs/win32/path.o ../../../build/yield/fs/win32/stat.o ../../../build/yield/fs/win32/volume.o
endif


../../../lib/libyield_fs.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../lib 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/libyield_fs.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/fs/bsd/directory.o: ../../../src/yield/fs/bsd/directory.cpp
	-mkdir -p ../../../build/yield/fs/bsd 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/bsd/directory.o -MD $(CXXFLAGS) ../../../src/yield/fs/bsd/directory.cpp

../../../build/yield/fs/bsd/volume.o: ../../../src/yield/fs/bsd/volume.cpp
	-mkdir -p ../../../build/yield/fs/bsd 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/bsd/volume.o -MD $(CXXFLAGS) ../../../src/yield/fs/bsd/volume.cpp

../../../build/yield/fs/darwin/extended_attributes.o: ../../../src/yield/fs/darwin/extended_attributes.cpp
	-mkdir -p ../../../build/yield/fs/darwin 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/darwin/extended_attributes.o -MD $(CXXFLAGS) ../../../src/yield/fs/darwin/extended_attributes.cpp

../../../build/yield/fs/darwin/file.o: ../../../src/yield/fs/darwin/file.cpp
	-mkdir -p ../../../build/yield/fs/darwin 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/darwin/file.o -MD $(CXXFLAGS) ../../../src/yield/fs/darwin/file.cpp

../../../build/yield/fs/darwin/volume.o: ../../../src/yield/fs/darwin/volume.cpp
	-mkdir -p ../../../build/yield/fs/darwin 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/darwin/volume.o -MD $(CXXFLAGS) ../../../src/yield/fs/darwin/volume.cpp

../../../build/yield/fs/extended_attributes.o: ../../../src/yield/fs/extended_attributes.cpp
	-mkdir -p ../../../build/yield/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/extended_attributes.o -MD $(CXXFLAGS) ../../../src/yield/fs/extended_attributes.cpp

../../../build/yield/fs/file.o: ../../../src/yield/fs/file.cpp
	-mkdir -p ../../../build/yield/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/file.o -MD $(CXXFLAGS) ../../../src/yield/fs/file.cpp

../../../build/yield/fs/file_log.o: ../../../src/yield/fs/file_log.cpp
	-mkdir -p ../../../build/yield/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/file_log.o -MD $(CXXFLAGS) ../../../src/yield/fs/file_log.cpp

../../../build/yield/fs/freebsd/extended_attributes.o: ../../../src/yield/fs/freebsd/extended_attributes.cpp
	-mkdir -p ../../../build/yield/fs/freebsd 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/freebsd/extended_attributes.o -MD $(CXXFLAGS) ../../../src/yield/fs/freebsd/extended_attributes.cpp

../../../build/yield/fs/freebsd/file.o: ../../../src/yield/fs/freebsd/file.cpp
	-mkdir -p ../../../build/yield/fs/freebsd 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/freebsd/file.o -MD $(CXXFLAGS) ../../../src/yield/fs/freebsd/file.cpp

../../../build/yield/fs/freebsd/volume.o: ../../../src/yield/fs/freebsd/volume.cpp
	-mkdir -p ../../../build/yield/fs/freebsd 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/freebsd/volume.o -MD $(CXXFLAGS) ../../../src/yield/fs/freebsd/volume.cpp

../../../build/yield/fs/linux/directory.o: ../../../src/yield/fs/linux/directory.cpp
	-mkdir -p ../../../build/yield/fs/linux 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/linux/directory.o -MD $(CXXFLAGS) ../../../src/yield/fs/linux/directory.cpp

../../../build/yield/fs/linux/extended_attributes.o: ../../../src/yield/fs/linux/extended_attributes.cpp
	-mkdir -p ../../../build/yield/fs/linux 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/linux/extended_attributes.o -MD $(CXXFLAGS) ../../../src/yield/fs/linux/extended_attributes.cpp

../../../build/yield/fs/linux/file.o: ../../../src/yield/fs/linux/file.cpp
	-mkdir -p ../../../build/yield/fs/linux 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/linux/file.o -MD $(CXXFLAGS) ../../../src/yield/fs/linux/file.cpp

../../../build/yield/fs/linux/volume.o: ../../../src/yield/fs/linux/volume.cpp
	-mkdir -p ../../../build/yield/fs/linux 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/linux/volume.o -MD $(CXXFLAGS) ../../../src/yield/fs/linux/volume.cpp

../../../build/yield/fs/memory_mapped_file.o: ../../../src/yield/fs/memory_mapped_file.cpp
	-mkdir -p ../../../build/yield/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/memory_mapped_file.o -MD $(CXXFLAGS) ../../../src/yield/fs/memory_mapped_file.cpp

../../../build/yield/fs/posix/directory.o: ../../../src/yield/fs/posix/directory.cpp
	-mkdir -p ../../../build/yield/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/posix/directory.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/directory.cpp

../../../build/yield/fs/posix/file.o: ../../../src/yield/fs/posix/file.cpp
	-mkdir -p ../../../build/yield/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/posix/file.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/file.cpp

../../../build/yield/fs/posix/memory_mapped_file.o: ../../../src/yield/fs/posix/memory_mapped_file.cpp
	-mkdir -p ../../../build/yield/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/posix/memory_mapped_file.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/memory_mapped_file.cpp

../../../build/yield/fs/posix/path.o: ../../../src/yield/fs/posix/path.cpp
	-mkdir -p ../../../build/yield/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/posix/path.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/path.cpp

../../../build/yield/fs/posix/stat.o: ../../../src/yield/fs/posix/stat.cpp
	-mkdir -p ../../../build/yield/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/posix/stat.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/stat.cpp

../../../build/yield/fs/posix/volume.o: ../../../src/yield/fs/posix/volume.cpp
	-mkdir -p ../../../build/yield/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/posix/volume.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/volume.cpp

../../../build/yield/fs/stat.o: ../../../src/yield/fs/stat.cpp
	-mkdir -p ../../../build/yield/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/stat.o -MD $(CXXFLAGS) ../../../src/yield/fs/stat.cpp

../../../build/yield/fs/volume.o: ../../../src/yield/fs/volume.cpp
	-mkdir -p ../../../build/yield/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/volume.o -MD $(CXXFLAGS) ../../../src/yield/fs/volume.cpp

../../../build/yield/fs/win32/directory.o: ../../../src/yield/fs/win32/directory.cpp
	-mkdir -p ../../../build/yield/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/win32/directory.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/directory.cpp

../../../build/yield/fs/win32/file.o: ../../../src/yield/fs/win32/file.cpp
	-mkdir -p ../../../build/yield/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/win32/file.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/file.cpp

../../../build/yield/fs/win32/memory_mapped_file.o: ../../../src/yield/fs/win32/memory_mapped_file.cpp
	-mkdir -p ../../../build/yield/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/win32/memory_mapped_file.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/memory_mapped_file.cpp

../../../build/yield/fs/win32/named_pipe.o: ../../../src/yield/fs/win32/named_pipe.cpp
	-mkdir -p ../../../build/yield/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/win32/named_pipe.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/named_pipe.cpp

../../../build/yield/fs/win32/path.o: ../../../src/yield/fs/win32/path.cpp
	-mkdir -p ../../../build/yield/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/win32/path.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/path.cpp

../../../build/yield/fs/win32/stat.o: ../../../src/yield/fs/win32/stat.cpp
	-mkdir -p ../../../build/yield/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/win32/stat.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/stat.cpp

../../../build/yield/fs/win32/volume.o: ../../../src/yield/fs/win32/volume.cpp
	-mkdir -p ../../../build/yield/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/win32/volume.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/volume.cpp


