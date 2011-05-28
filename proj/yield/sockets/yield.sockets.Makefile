TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../include
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -I/usr/local/include
endif
ifeq ($(UNAME), Linux)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Solaris)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Darwin)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wunused
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


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++ -lnsl -lsocket
endif
LIBS += -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/sockets -name "*.d")


ifeq ($(UNAME), Darwin)
	O_FILE_PATHS += ../../../build/yield/sockets/posix/datagram_socket.o ../../../build/yield/sockets/posix/datagram_socket_pair.o ../../../build/yield/sockets/posix/socket.o ../../../build/yield/sockets/posix/socket_address.o ../../../build/yield/sockets/posix/stream_socket.o ../../../build/yield/sockets/posix/stream_socket_pair.o ../../../build/yield/sockets/posix/tcp_socket.o ../../../build/yield/sockets/posix/udp_socket.o
endif
ifeq ($(UNAME), FreeBSD)
	O_FILE_PATHS += ../../../build/yield/sockets/posix/datagram_socket.o ../../../build/yield/sockets/posix/datagram_socket_pair.o ../../../build/yield/sockets/posix/socket.o ../../../build/yield/sockets/posix/socket_address.o ../../../build/yield/sockets/posix/stream_socket.o ../../../build/yield/sockets/posix/stream_socket_pair.o ../../../build/yield/sockets/posix/tcp_socket.o ../../../build/yield/sockets/posix/udp_socket.o
endif
ifeq ($(UNAME), Linux)
	O_FILE_PATHS += ../../../build/yield/sockets/posix/datagram_socket.o ../../../build/yield/sockets/posix/datagram_socket_pair.o ../../../build/yield/sockets/posix/socket.o ../../../build/yield/sockets/posix/socket_address.o ../../../build/yield/sockets/posix/stream_socket.o ../../../build/yield/sockets/posix/stream_socket_pair.o ../../../build/yield/sockets/posix/tcp_socket.o ../../../build/yield/sockets/posix/udp_socket.o
endif
ifeq ($(UNAME), Solaris)
	O_FILE_PATHS += ../../../build/yield/sockets/posix/datagram_socket.o ../../../build/yield/sockets/posix/datagram_socket_pair.o ../../../build/yield/sockets/posix/socket.o ../../../build/yield/sockets/posix/socket_address.o ../../../build/yield/sockets/posix/stream_socket.o ../../../build/yield/sockets/posix/stream_socket_pair.o ../../../build/yield/sockets/posix/tcp_socket.o ../../../build/yield/sockets/posix/udp_socket.o
endif
ifeq ($(UNAME), MINGW32)
	O_FILE_PATHS += ../../../build/yield/sockets/win32/datagram_socket.o ../../../build/yield/sockets/win32/datagram_socket_pair.o ../../../build/yield/sockets/win32/socket.o ../../../build/yield/sockets/win32/socket_address.o ../../../build/yield/sockets/win32/stream_socket.o ../../../build/yield/sockets/win32/stream_socket_pair.o ../../../build/yield/sockets/win32/tcp_socket.o ../../../build/yield/sockets/win32/udp_socket.o
endif


all: ../../../lib/yield/libyield_sockets.a

clean:
	$(RM) ../../../lib/yield/libyield_sockets.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../lib/yield/libyield.a:
	$(MAKE) -C .. yield.Makefile


../../../lib/yield/libyield_sockets.a: $(O_FILE_PATHS) ../../../lib/yield/libyield.a
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../build/yield/sockets/posix/datagram_socket.o: ../../../src/yield/sockets/posix/datagram_socket.cpp
	-mkdir -p ../../../build/yield/sockets/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/posix/datagram_socket.o -MD $(CXXFLAGS) ../../../src/yield/sockets/posix/datagram_socket.cpp

../../../build/yield/sockets/posix/datagram_socket_pair.o: ../../../src/yield/sockets/posix/datagram_socket_pair.cpp
	-mkdir -p ../../../build/yield/sockets/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/posix/datagram_socket_pair.o -MD $(CXXFLAGS) ../../../src/yield/sockets/posix/datagram_socket_pair.cpp

../../../build/yield/sockets/posix/socket.o: ../../../src/yield/sockets/posix/socket.cpp
	-mkdir -p ../../../build/yield/sockets/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/posix/socket.o -MD $(CXXFLAGS) ../../../src/yield/sockets/posix/socket.cpp

../../../build/yield/sockets/posix/socket_address.o: ../../../src/yield/sockets/posix/socket_address.cpp
	-mkdir -p ../../../build/yield/sockets/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/posix/socket_address.o -MD $(CXXFLAGS) ../../../src/yield/sockets/posix/socket_address.cpp

../../../build/yield/sockets/posix/stream_socket.o: ../../../src/yield/sockets/posix/stream_socket.cpp
	-mkdir -p ../../../build/yield/sockets/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/posix/stream_socket.o -MD $(CXXFLAGS) ../../../src/yield/sockets/posix/stream_socket.cpp

../../../build/yield/sockets/posix/stream_socket_pair.o: ../../../src/yield/sockets/posix/stream_socket_pair.cpp
	-mkdir -p ../../../build/yield/sockets/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/posix/stream_socket_pair.o -MD $(CXXFLAGS) ../../../src/yield/sockets/posix/stream_socket_pair.cpp

../../../build/yield/sockets/posix/tcp_socket.o: ../../../src/yield/sockets/posix/tcp_socket.cpp
	-mkdir -p ../../../build/yield/sockets/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/posix/tcp_socket.o -MD $(CXXFLAGS) ../../../src/yield/sockets/posix/tcp_socket.cpp

../../../build/yield/sockets/posix/udp_socket.o: ../../../src/yield/sockets/posix/udp_socket.cpp
	-mkdir -p ../../../build/yield/sockets/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/posix/udp_socket.o -MD $(CXXFLAGS) ../../../src/yield/sockets/posix/udp_socket.cpp

../../../build/yield/sockets/win32/datagram_socket.o: ../../../src/yield/sockets/win32/datagram_socket.cpp
	-mkdir -p ../../../build/yield/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/win32/datagram_socket.o -MD $(CXXFLAGS) ../../../src/yield/sockets/win32/datagram_socket.cpp

../../../build/yield/sockets/win32/datagram_socket_pair.o: ../../../src/yield/sockets/win32/datagram_socket_pair.cpp
	-mkdir -p ../../../build/yield/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/win32/datagram_socket_pair.o -MD $(CXXFLAGS) ../../../src/yield/sockets/win32/datagram_socket_pair.cpp

../../../build/yield/sockets/win32/socket.o: ../../../src/yield/sockets/win32/socket.cpp
	-mkdir -p ../../../build/yield/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/win32/socket.o -MD $(CXXFLAGS) ../../../src/yield/sockets/win32/socket.cpp

../../../build/yield/sockets/win32/socket_address.o: ../../../src/yield/sockets/win32/socket_address.cpp
	-mkdir -p ../../../build/yield/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/win32/socket_address.o -MD $(CXXFLAGS) ../../../src/yield/sockets/win32/socket_address.cpp

../../../build/yield/sockets/win32/stream_socket.o: ../../../src/yield/sockets/win32/stream_socket.cpp
	-mkdir -p ../../../build/yield/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/win32/stream_socket.o -MD $(CXXFLAGS) ../../../src/yield/sockets/win32/stream_socket.cpp

../../../build/yield/sockets/win32/stream_socket_pair.o: ../../../src/yield/sockets/win32/stream_socket_pair.cpp
	-mkdir -p ../../../build/yield/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/win32/stream_socket_pair.o -MD $(CXXFLAGS) ../../../src/yield/sockets/win32/stream_socket_pair.cpp

../../../build/yield/sockets/win32/tcp_socket.o: ../../../src/yield/sockets/win32/tcp_socket.cpp
	-mkdir -p ../../../build/yield/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/win32/tcp_socket.o -MD $(CXXFLAGS) ../../../src/yield/sockets/win32/tcp_socket.cpp

../../../build/yield/sockets/win32/udp_socket.o: ../../../src/yield/sockets/win32/udp_socket.cpp
	-mkdir -p ../../../build/yield/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/win32/udp_socket.o -MD $(CXXFLAGS) ../../../src/yield/sockets/win32/udp_socket.cpp
