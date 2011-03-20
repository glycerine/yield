# SHELL = /bin/bash
UNAME := $(shell uname)

CXXFLAGS += -I../../../../include
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
	CXXFLAGS += /EHsc /GR- /MP /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
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


LDFLAGS += -L../../../../lib/yield
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Darwin)
	LIBS += -liconv
endif
ifeq ($(UNAME), FreeBSD)
	LIBS += -liconv -lintl -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lpthread -lrt -lstdc++ -laio
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_aio -lyield_fs -lyield_i18n -lyield_thread -lyield


D_FILE_PATHS := $(shell find ../../../../build/yield/fs/aio -name "*.d")


O_FILE_PATHS += ../../../../build/yield/fs/aio/aiocb.o ../../../../build/yield/fs/aio/bio_queue.o ../../../../build/yield/fs/aio/fdatasync_aiocb.o ../../../../build/yield/fs/aio/fsync_aiocb.o ../../../../build/yield/fs/aio/pread_aiocb.o ../../../../build/yield/fs/aio/pwrite_aiocb.o ../../../../build/yield/fs/aio/setlk_aiocb.o ../../../../build/yield/fs/aio/unlk_aiocb.o
ifeq ($(UNAME), Darwin)
	O_FILE_PATHS += ../../../../build/yield/fs/aio/posix/aio_queue.o ../../../../build/yield/fs/aio/posix/fdatasync_aiocb.o ../../../../build/yield/fs/aio/posix/fsync_aiocb.o ../../../../build/yield/fs/aio/posix/pread_aiocb.o ../../../../build/yield/fs/aio/posix/pwrite_aiocb.o
endif
ifeq ($(UNAME), FreeBSD)
	O_FILE_PATHS += ../../../../build/yield/fs/aio/posix/aio_queue.o ../../../../build/yield/fs/aio/posix/fdatasync_aiocb.o ../../../../build/yield/fs/aio/posix/fsync_aiocb.o ../../../../build/yield/fs/aio/posix/pread_aiocb.o ../../../../build/yield/fs/aio/posix/pwrite_aiocb.o
endif
ifeq ($(UNAME), Linux)
	O_FILE_PATHS += ../../../../build/yield/fs/aio/posix/aio_queue.o ../../../../build/yield/fs/aio/posix/fdatasync_aiocb.o ../../../../build/yield/fs/aio/posix/fsync_aiocb.o ../../../../build/yield/fs/aio/posix/pread_aiocb.o ../../../../build/yield/fs/aio/posix/pwrite_aiocb.o
endif
ifeq ($(UNAME), Solaris)
	O_FILE_PATHS += ../../../../build/yield/fs/aio/posix/aio_queue.o ../../../../build/yield/fs/aio/posix/fdatasync_aiocb.o ../../../../build/yield/fs/aio/posix/fsync_aiocb.o ../../../../build/yield/fs/aio/posix/pread_aiocb.o ../../../../build/yield/fs/aio/posix/pwrite_aiocb.o
endif
ifeq ($(UNAME), MINGW32)
	O_FILE_PATHS += ../../../../build/yield/fs/aio/win32/aio_queue.o ../../../../build/yield/fs/aio/win32/aiocb.o ../../../../build/yield/fs/aio/win32/pread_aiocb.o ../../../../build/yield/fs/aio/win32/pwrite_aiocb.o ../../../../build/yield/fs/aio/win32/setlk_aiocb.o ../../../../build/yield/fs/aio/win32/unlk_aiocb.o
endif


all: ../../../../lib/yield/libyield_fs_aio.a

clean:
	$(RM) ../../../../lib/yield/libyield_fs_aio.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../../lib/yield/libyield_fs_aio.a: $(O_FILE_PATHS)
	-mkdir -p ../../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../../build/yield/fs/aio/aiocb.o: ../../../../src/yield/fs/aio/aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/aiocb.cpp

../../../../build/yield/fs/aio/bio_queue.o: ../../../../src/yield/fs/aio/bio_queue.cpp
	-mkdir -p ../../../../build/yield/fs/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/bio_queue.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/bio_queue.cpp

../../../../build/yield/fs/aio/fdatasync_aiocb.o: ../../../../src/yield/fs/aio/fdatasync_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/fdatasync_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/fdatasync_aiocb.cpp

../../../../build/yield/fs/aio/fsync_aiocb.o: ../../../../src/yield/fs/aio/fsync_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/fsync_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/fsync_aiocb.cpp

../../../../build/yield/fs/aio/posix/aio_queue.o: ../../../../src/yield/fs/aio/posix/aio_queue.cpp
	-mkdir -p ../../../../build/yield/fs/aio/posix 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/posix/aio_queue.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/posix/aio_queue.cpp

../../../../build/yield/fs/aio/posix/fdatasync_aiocb.o: ../../../../src/yield/fs/aio/posix/fdatasync_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio/posix 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/posix/fdatasync_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/posix/fdatasync_aiocb.cpp

../../../../build/yield/fs/aio/posix/fsync_aiocb.o: ../../../../src/yield/fs/aio/posix/fsync_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio/posix 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/posix/fsync_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/posix/fsync_aiocb.cpp

../../../../build/yield/fs/aio/posix/pread_aiocb.o: ../../../../src/yield/fs/aio/posix/pread_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio/posix 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/posix/pread_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/posix/pread_aiocb.cpp

../../../../build/yield/fs/aio/posix/pwrite_aiocb.o: ../../../../src/yield/fs/aio/posix/pwrite_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio/posix 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/posix/pwrite_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/posix/pwrite_aiocb.cpp

../../../../build/yield/fs/aio/pread_aiocb.o: ../../../../src/yield/fs/aio/pread_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/pread_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/pread_aiocb.cpp

../../../../build/yield/fs/aio/pwrite_aiocb.o: ../../../../src/yield/fs/aio/pwrite_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/pwrite_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/pwrite_aiocb.cpp

../../../../build/yield/fs/aio/setlk_aiocb.o: ../../../../src/yield/fs/aio/setlk_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/setlk_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/setlk_aiocb.cpp

../../../../build/yield/fs/aio/unlk_aiocb.o: ../../../../src/yield/fs/aio/unlk_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/unlk_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/unlk_aiocb.cpp

../../../../build/yield/fs/aio/win32/aio_queue.o: ../../../../src/yield/fs/aio/win32/aio_queue.cpp
	-mkdir -p ../../../../build/yield/fs/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/win32/aio_queue.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/win32/aio_queue.cpp

../../../../build/yield/fs/aio/win32/aiocb.o: ../../../../src/yield/fs/aio/win32/aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/win32/aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/win32/aiocb.cpp

../../../../build/yield/fs/aio/win32/pread_aiocb.o: ../../../../src/yield/fs/aio/win32/pread_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/win32/pread_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/win32/pread_aiocb.cpp

../../../../build/yield/fs/aio/win32/pwrite_aiocb.o: ../../../../src/yield/fs/aio/win32/pwrite_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/win32/pwrite_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/win32/pwrite_aiocb.cpp

../../../../build/yield/fs/aio/win32/setlk_aiocb.o: ../../../../src/yield/fs/aio/win32/setlk_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/win32/setlk_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/win32/setlk_aiocb.cpp

../../../../build/yield/fs/aio/win32/unlk_aiocb.o: ../../../../src/yield/fs/aio/win32/unlk_aiocb.cpp
	-mkdir -p ../../../../build/yield/fs/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio/win32/unlk_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/fs/aio/win32/unlk_aiocb.cpp


