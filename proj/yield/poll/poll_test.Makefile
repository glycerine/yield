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


ifeq ($(UNAME), FreeBSD)
	LIBS += -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lnsl -lsocket -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_poll -lyield_net -lyield_thread -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/poll_test -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/poll_test/yield_poll_test_main.o
ifeq ($(UNAME), Darwin)
	OBJECT_FILE_PATHS += ../../../build/yield/poll_test/bsd/kqueue_test.o
	OBJECT_FILE_PATHS += ../../../build/yield/poll_test/posix/poller_test.o
endif
ifeq ($(UNAME), FreeBSD)
	OBJECT_FILE_PATHS += ../../../build/yield/poll_test/bsd/kqueue_test.o
	OBJECT_FILE_PATHS += ../../../build/yield/poll_test/posix/poller_test.o
endif
ifeq ($(UNAME), Linux)
	OBJECT_FILE_PATHS += ../../../build/yield/poll_test/linux/epoller_test.o
	OBJECT_FILE_PATHS += ../../../build/yield/poll_test/posix/poller_test.o
endif
ifeq ($(UNAME), Solaris)
	OBJECT_FILE_PATHS += ../../../build/yield/poll_test/posix/poller_test.o
	OBJECT_FILE_PATHS += ../../../build/yield/poll_test/sunos/event_port_test.o
endif
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../build/yield/poll_test/win32/handle_event_queue_test.o ../../../build/yield/poll_test/win32/selector_test.o ../../../build/yield/poll_test/win32/wsapoller_test.o
endif


../../../bin/yield/yield_poll_test: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../bin/yield/yield_poll_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/poll_test/bsd/kqueue_test.o: ../../../src/yield/poll/bsd/kqueue_test.cpp
	-mkdir -p ../../../build/yield/poll_test/bsd 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll_test/bsd/kqueue_test.o -MD $(CXXFLAGS) ../../../src/yield/poll/bsd/kqueue_test.cpp

../../../build/yield/poll_test/linux/epoller_test.o: ../../../src/yield/poll/linux/epoller_test.cpp
	-mkdir -p ../../../build/yield/poll_test/linux 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll_test/linux/epoller_test.o -MD $(CXXFLAGS) ../../../src/yield/poll/linux/epoller_test.cpp

../../../build/yield/poll_test/posix/poller_test.o: ../../../src/yield/poll/posix/poller_test.cpp
	-mkdir -p ../../../build/yield/poll_test/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll_test/posix/poller_test.o -MD $(CXXFLAGS) ../../../src/yield/poll/posix/poller_test.cpp

../../../build/yield/poll_test/sunos/event_port_test.o: ../../../src/yield/poll/sunos/event_port_test.cpp
	-mkdir -p ../../../build/yield/poll_test/sunos 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll_test/sunos/event_port_test.o -MD $(CXXFLAGS) ../../../src/yield/poll/sunos/event_port_test.cpp

../../../build/yield/poll_test/win32/handle_event_queue_test.o: ../../../src/yield/poll/win32/handle_event_queue_test.cpp
	-mkdir -p ../../../build/yield/poll_test/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll_test/win32/handle_event_queue_test.o -MD $(CXXFLAGS) ../../../src/yield/poll/win32/handle_event_queue_test.cpp

../../../build/yield/poll_test/win32/selector_test.o: ../../../src/yield/poll/win32/selector_test.cpp
	-mkdir -p ../../../build/yield/poll_test/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll_test/win32/selector_test.o -MD $(CXXFLAGS) ../../../src/yield/poll/win32/selector_test.cpp

../../../build/yield/poll_test/win32/wsapoller_test.o: ../../../src/yield/poll/win32/wsapoller_test.cpp
	-mkdir -p ../../../build/yield/poll_test/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll_test/win32/wsapoller_test.o -MD $(CXXFLAGS) ../../../src/yield/poll/win32/wsapoller_test.cpp

../../../build/yield/poll_test/yield_poll_test_main.o: ../../../src/yield/poll/yield_poll_test_main.cpp
	-mkdir -p ../../../build/yield/poll_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll_test/yield_poll_test_main.o -MD $(CXXFLAGS) ../../../src/yield/poll/yield_poll_test_main.cpp


