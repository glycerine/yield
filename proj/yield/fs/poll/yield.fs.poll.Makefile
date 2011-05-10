TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../../include
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -I/usr/local/include
endif
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
ifneq ($(COVERAGE),)
	CXXFLAGS += -fprofile-arcs -ftest-coverage
	LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
endif
ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif


LDFLAGS += -L../../../../lib/yield
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
	LIBS += -liconv -lintl -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_fs -lyield_i18n -lyield_thread -lyield


D_FILE_PATHS := $(shell find ../../../../build/yield/fs/poll -name "*.d")


O_FILE_PATHS += ../../../../build/yield/fs/poll/fs_event.o ../../../../build/yield/fs/poll/scanning_directory_watch.o ../../../../build/yield/fs/poll/scanning_file_watch.o ../../../../build/yield/fs/poll/scanning_fs_event_queue.o ../../../../build/yield/fs/poll/scanning_watch.o ../../../../build/yield/fs/poll/watch.o
ifeq ($(UNAME), Darwin)
	O_FILE_PATHS += ../../../../build/yield/fs/poll/bsd/fs_event_queue.o
endif
ifeq ($(UNAME), FreeBSD)
	O_FILE_PATHS += ../../../../build/yield/fs/poll/bsd/fs_event_queue.o
endif
ifeq ($(UNAME), Linux)
	O_FILE_PATHS += ../../../../build/yield/fs/poll/linux/fs_event_queue.o ../../../../build/yield/fs/poll/linux/watch.o ../../../../build/yield/fs/poll/linux/watches.o
endif
ifeq ($(UNAME), MINGW32)
	O_FILE_PATHS += ../../../../build/yield/fs/poll/win32/directory_watch.o ../../../../build/yield/fs/poll/win32/file_watch.o ../../../../build/yield/fs/poll/win32/fs_event_queue.o ../../../../build/yield/fs/poll/win32/watch.o
endif


all: ../../../../lib/yield/libyield_fs_poll.a

clean:
	$(RM) ../../../../lib/yield/libyield_fs_poll.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../../lib/yield/libyield_fs.a:
	$(MAKE) -C .. yield.fs.Makefile


../../../../lib/yield/libyield_queue.a:
	$(MAKE) -C ../../queue yield.queue.Makefile


../../../../lib/yield/libyield_fs_poll.a: $(O_FILE_PATHS) ../../../../lib/yield/libyield_fs.a ../../../../lib/yield/libyield_queue.a
	-mkdir -p ../../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../../build/yield/fs/poll/bsd/fs_event_queue.o: ../../../../src/yield/fs/poll/bsd/fs_event_queue.cpp
	-mkdir -p ../../../../build/yield/fs/poll/bsd 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/bsd/fs_event_queue.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/bsd/fs_event_queue.cpp

../../../../build/yield/fs/poll/fs_event.o: ../../../../src/yield/fs/poll/fs_event.cpp
	-mkdir -p ../../../../build/yield/fs/poll 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/fs_event.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/fs_event.cpp

../../../../build/yield/fs/poll/linux/fs_event_queue.o: ../../../../src/yield/fs/poll/linux/fs_event_queue.cpp
	-mkdir -p ../../../../build/yield/fs/poll/linux 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/linux/fs_event_queue.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/linux/fs_event_queue.cpp

../../../../build/yield/fs/poll/linux/watch.o: ../../../../src/yield/fs/poll/linux/watch.cpp
	-mkdir -p ../../../../build/yield/fs/poll/linux 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/linux/watch.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/linux/watch.cpp

../../../../build/yield/fs/poll/linux/watches.o: ../../../../src/yield/fs/poll/linux/watches.cpp
	-mkdir -p ../../../../build/yield/fs/poll/linux 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/linux/watches.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/linux/watches.cpp

../../../../build/yield/fs/poll/scanning_directory_watch.o: ../../../../src/yield/fs/poll/scanning_directory_watch.cpp
	-mkdir -p ../../../../build/yield/fs/poll 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/scanning_directory_watch.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/scanning_directory_watch.cpp

../../../../build/yield/fs/poll/scanning_file_watch.o: ../../../../src/yield/fs/poll/scanning_file_watch.cpp
	-mkdir -p ../../../../build/yield/fs/poll 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/scanning_file_watch.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/scanning_file_watch.cpp

../../../../build/yield/fs/poll/scanning_fs_event_queue.o: ../../../../src/yield/fs/poll/scanning_fs_event_queue.cpp
	-mkdir -p ../../../../build/yield/fs/poll 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/scanning_fs_event_queue.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/scanning_fs_event_queue.cpp

../../../../build/yield/fs/poll/scanning_watch.o: ../../../../src/yield/fs/poll/scanning_watch.cpp
	-mkdir -p ../../../../build/yield/fs/poll 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/scanning_watch.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/scanning_watch.cpp

../../../../build/yield/fs/poll/watch.o: ../../../../src/yield/fs/poll/watch.cpp
	-mkdir -p ../../../../build/yield/fs/poll 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/watch.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/watch.cpp

../../../../build/yield/fs/poll/win32/directory_watch.o: ../../../../src/yield/fs/poll/win32/directory_watch.cpp
	-mkdir -p ../../../../build/yield/fs/poll/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/win32/directory_watch.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/win32/directory_watch.cpp

../../../../build/yield/fs/poll/win32/file_watch.o: ../../../../src/yield/fs/poll/win32/file_watch.cpp
	-mkdir -p ../../../../build/yield/fs/poll/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/win32/file_watch.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/win32/file_watch.cpp

../../../../build/yield/fs/poll/win32/fs_event_queue.o: ../../../../src/yield/fs/poll/win32/fs_event_queue.cpp
	-mkdir -p ../../../../build/yield/fs/poll/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/win32/fs_event_queue.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/win32/fs_event_queue.cpp

../../../../build/yield/fs/poll/win32/watch.o: ../../../../src/yield/fs/poll/win32/watch.cpp
	-mkdir -p ../../../../build/yield/fs/poll/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/poll/win32/watch.o -MD $(CXXFLAGS) ../../../../src/yield/fs/poll/win32/watch.cpp
