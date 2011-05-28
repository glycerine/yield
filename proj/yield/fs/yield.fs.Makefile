TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../include
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -I/usr/local/include
endif
ifeq ($(UNAME), Linux)
	CXXFLAGS += -fno-rtti -Wall -Wconversion -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wpadded -Wsign-conversion -Wunused
endif
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -fno-rtti -Wall -Wconversion -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wpadded -Wsign-conversion -Wunused
endif
ifeq ($(UNAME), Solaris)
	CXXFLAGS += -fno-rtti -Wall -Wconversion -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wpadded -Wsign-conversion -Wunused
endif
ifeq ($(UNAME), Darwin)
	CXXFLAGS += -fno-rtti -Wall -Wconversion -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wpadded -Wsign-conversion -Wunused
endif
ifeq ($(UNAME), MINGW32)
	CXXFLAGS += /EHsc /GR- /MP /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
endif
ifneq ($(COVERAGE),)
	CXXFLAGS += -fprofile-arcs -ftest-coverage
	LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
else ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif



LDFLAGS += -L../../../lib/yield
ifeq ($(UNAME), FreeBSD)
	LDFLAGS += -L/usr/local/lib
endif
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
LIBS += -lyield_i18n -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/fs -name "*.d")


O_FILE_PATHS += ../../../build/yield/fs/file_log.o
ifeq ($(UNAME), Darwin)
	O_FILE_PATHS += ../../../build/yield/fs/posix/directory.o ../../../build/yield/fs/posix/file.o ../../../build/yield/fs/posix/file_system.o ../../../build/yield/fs/posix/path.o ../../../build/yield/fs/posix/stat.o
endif
ifeq ($(UNAME), FreeBSD)
	O_FILE_PATHS += ../../../build/yield/fs/posix/directory.o ../../../build/yield/fs/posix/file.o ../../../build/yield/fs/posix/file_system.o ../../../build/yield/fs/posix/path.o ../../../build/yield/fs/posix/stat.o
endif
ifeq ($(UNAME), Linux)
	O_FILE_PATHS += ../../../build/yield/fs/posix/directory.o ../../../build/yield/fs/posix/file.o ../../../build/yield/fs/posix/file_system.o ../../../build/yield/fs/posix/path.o ../../../build/yield/fs/posix/stat.o
endif
ifeq ($(UNAME), Solaris)
	O_FILE_PATHS += ../../../build/yield/fs/posix/directory.o ../../../build/yield/fs/posix/file.o ../../../build/yield/fs/posix/file_system.o ../../../build/yield/fs/posix/path.o ../../../build/yield/fs/posix/stat.o
endif
ifeq ($(UNAME), MINGW32)
	O_FILE_PATHS += ../../../build/yield/fs/win32/directory.o ../../../build/yield/fs/win32/file.o ../../../build/yield/fs/win32/file_system.o ../../../build/yield/fs/win32/named_pipe.o ../../../build/yield/fs/win32/path.o ../../../build/yield/fs/win32/stat.o
endif


all: ../../../lib/yield/libyield_fs.a

clean:
	$(RM) ../../../lib/yield/libyield_fs.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../lib/yield/libyield_i18n.a:
	$(MAKE) -C ../i18n yield.i18n.Makefile


../../../lib/yield/libyield_fs.a: $(O_FILE_PATHS) ../../../lib/yield/libyield_i18n.a
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../build/yield/fs/file_log.o: ../../../src/yield/fs/file_log.cpp
	-mkdir -p ../../../build/yield/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/file_log.o -MD $(CXXFLAGS) ../../../src/yield/fs/file_log.cpp

../../../build/yield/fs/posix/directory.o: ../../../src/yield/fs/posix/directory.cpp
	-mkdir -p ../../../build/yield/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/posix/directory.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/directory.cpp

../../../build/yield/fs/posix/file.o: ../../../src/yield/fs/posix/file.cpp
	-mkdir -p ../../../build/yield/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/posix/file.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/file.cpp

../../../build/yield/fs/posix/file_system.o: ../../../src/yield/fs/posix/file_system.cpp
	-mkdir -p ../../../build/yield/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/posix/file_system.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/file_system.cpp

../../../build/yield/fs/posix/path.o: ../../../src/yield/fs/posix/path.cpp
	-mkdir -p ../../../build/yield/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/posix/path.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/path.cpp

../../../build/yield/fs/posix/stat.o: ../../../src/yield/fs/posix/stat.cpp
	-mkdir -p ../../../build/yield/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/posix/stat.o -MD $(CXXFLAGS) ../../../src/yield/fs/posix/stat.cpp

../../../build/yield/fs/win32/directory.o: ../../../src/yield/fs/win32/directory.cpp
	-mkdir -p ../../../build/yield/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/win32/directory.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/directory.cpp

../../../build/yield/fs/win32/file.o: ../../../src/yield/fs/win32/file.cpp
	-mkdir -p ../../../build/yield/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/win32/file.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/file.cpp

../../../build/yield/fs/win32/file_system.o: ../../../src/yield/fs/win32/file_system.cpp
	-mkdir -p ../../../build/yield/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/win32/file_system.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/file_system.cpp

../../../build/yield/fs/win32/named_pipe.o: ../../../src/yield/fs/win32/named_pipe.cpp
	-mkdir -p ../../../build/yield/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/win32/named_pipe.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/named_pipe.cpp

../../../build/yield/fs/win32/path.o: ../../../src/yield/fs/win32/path.cpp
	-mkdir -p ../../../build/yield/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/win32/path.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/path.cpp

../../../build/yield/fs/win32/stat.o: ../../../src/yield/fs/win32/stat.cpp
	-mkdir -p ../../../build/yield/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs/win32/stat.o -MD $(CXXFLAGS) ../../../src/yield/fs/win32/stat.cpp
