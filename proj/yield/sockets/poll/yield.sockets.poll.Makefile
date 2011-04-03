TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
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
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lnsl -lsocket -lm -lrt -lstdc++
endif
LIBS += -lyield_poll -lyield_sockets -lyield


D_FILE_PATHS := $(shell find ../../../../build/yield/sockets/poll -name "*.d")


O_FILE_PATHS += ../../../../build/yield/sockets/poll/socket_event_queue.o
ifeq ($(UNAME), MINGW32)
	O_FILE_PATHS += ../../../../build/yield/sockets/poll/win32/selector.o ../../../../build/yield/sockets/poll/win32/wsapoller.o
endif


all: ../../../../lib/yield/libyield_sockets_poll.a

clean:
	$(RM) ../../../../lib/yield/libyield_sockets_poll.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../../lib/yield/libyield_sockets_poll.a: $(O_FILE_PATHS) ..\..\..\..\lib\yield\libyield_poll.a ..\..\..\..\lib\yield\libyield_sockets.a
	-mkdir -p ../../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../../build/yield/sockets/poll/socket_event_queue.o: ../../../../src/yield/sockets/poll/socket_event_queue.cpp
	-mkdir -p ../../../../build/yield/sockets/poll 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/poll/socket_event_queue.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/poll/socket_event_queue.cpp

../../../../build/yield/sockets/poll/win32/selector.o: ../../../../src/yield/sockets/poll/win32/selector.cpp
	-mkdir -p ../../../../build/yield/sockets/poll/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/poll/win32/selector.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/poll/win32/selector.cpp

../../../../build/yield/sockets/poll/win32/wsapoller.o: ../../../../src/yield/sockets/poll/win32/wsapoller.cpp
	-mkdir -p ../../../../build/yield/sockets/poll/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/poll/win32/wsapoller.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/poll/win32/wsapoller.cpp
