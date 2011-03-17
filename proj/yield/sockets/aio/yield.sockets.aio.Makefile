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


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lnsl -lsocket -lm -lrt -lstdc++
endif
LIBS += -lyield_aio -lyield_sockets_poll -lyield_poll -lyield_sockets -lyield


DEP_FILE_PATHS := $(shell find ../../../../build/yield/sockets/aio -name "*.d")


OBJECT_FILE_PATHS += ../../../../build/yield/sockets/aio/accept_aiocb.o ../../../../build/yield/sockets/aio/aiocb.o ../../../../build/yield/sockets/aio/connect_aiocb.o ../../../../build/yield/sockets/aio/nbio_queue.o ../../../../build/yield/sockets/aio/recv_aiocb.o ../../../../build/yield/sockets/aio/send_aiocb.o
ifeq ($(UNAME), Darwin)
	OBJECT_FILE_PATHS += ../../../../build/yield/sockets/aio/posix/accept_aiocb.o
endif
ifeq ($(UNAME), FreeBSD)
	OBJECT_FILE_PATHS += ../../../../build/yield/sockets/aio/posix/accept_aiocb.o
endif
ifeq ($(UNAME), Linux)
	OBJECT_FILE_PATHS += ../../../../build/yield/sockets/aio/posix/accept_aiocb.o
endif
ifeq ($(UNAME), Solaris)
	OBJECT_FILE_PATHS += ../../../../build/yield/sockets/aio/posix/accept_aiocb.o
endif
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../../build/yield/sockets/aio/win32/accept_aiocb.o ../../../../build/yield/sockets/aio/win32/aio_queue.o ../../../../build/yield/sockets/aio/win32/aiocb.o ../../../../build/yield/sockets/aio/win32/connect_aiocb.o ../../../../build/yield/sockets/aio/win32/recv_aiocb.o ../../../../build/yield/sockets/aio/win32/send_aiocb.o
endif


../../../../lib/yield/libyield_sockets_aio.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../../lib/yield/libyield_sockets_aio.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../../build/yield/sockets/aio/accept_aiocb.o: ../../../../src/yield/sockets/aio/accept_aiocb.cpp
	-mkdir -p ../../../../build/yield/sockets/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/accept_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/aio/accept_aiocb.cpp

../../../../build/yield/sockets/aio/aiocb.o: ../../../../src/yield/sockets/aio/aiocb.cpp
	-mkdir -p ../../../../build/yield/sockets/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/aio/aiocb.cpp

../../../../build/yield/sockets/aio/connect_aiocb.o: ../../../../src/yield/sockets/aio/connect_aiocb.cpp
	-mkdir -p ../../../../build/yield/sockets/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/connect_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/aio/connect_aiocb.cpp

../../../../build/yield/sockets/aio/nbio_queue.o: ../../../../src/yield/sockets/aio/nbio_queue.cpp
	-mkdir -p ../../../../build/yield/sockets/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/nbio_queue.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/aio/nbio_queue.cpp

../../../../build/yield/sockets/aio/posix/accept_aiocb.o: ../../../../src/yield/sockets/aio/posix/accept_aiocb.cpp
	-mkdir -p ../../../../build/yield/sockets/aio/posix 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/posix/accept_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/aio/posix/accept_aiocb.cpp

../../../../build/yield/sockets/aio/recv_aiocb.o: ../../../../src/yield/sockets/aio/recv_aiocb.cpp
	-mkdir -p ../../../../build/yield/sockets/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/recv_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/aio/recv_aiocb.cpp

../../../../build/yield/sockets/aio/send_aiocb.o: ../../../../src/yield/sockets/aio/send_aiocb.cpp
	-mkdir -p ../../../../build/yield/sockets/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/send_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/aio/send_aiocb.cpp

../../../../build/yield/sockets/aio/win32/accept_aiocb.o: ../../../../src/yield/sockets/aio/win32/accept_aiocb.cpp
	-mkdir -p ../../../../build/yield/sockets/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/win32/accept_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/aio/win32/accept_aiocb.cpp

../../../../build/yield/sockets/aio/win32/aio_queue.o: ../../../../src/yield/sockets/aio/win32/aio_queue.cpp
	-mkdir -p ../../../../build/yield/sockets/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/win32/aio_queue.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/aio/win32/aio_queue.cpp

../../../../build/yield/sockets/aio/win32/aiocb.o: ../../../../src/yield/sockets/aio/win32/aiocb.cpp
	-mkdir -p ../../../../build/yield/sockets/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/win32/aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/aio/win32/aiocb.cpp

../../../../build/yield/sockets/aio/win32/connect_aiocb.o: ../../../../src/yield/sockets/aio/win32/connect_aiocb.cpp
	-mkdir -p ../../../../build/yield/sockets/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/win32/connect_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/aio/win32/connect_aiocb.cpp

../../../../build/yield/sockets/aio/win32/recv_aiocb.o: ../../../../src/yield/sockets/aio/win32/recv_aiocb.cpp
	-mkdir -p ../../../../build/yield/sockets/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/win32/recv_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/aio/win32/recv_aiocb.cpp

../../../../build/yield/sockets/aio/win32/send_aiocb.o: ../../../../src/yield/sockets/aio/win32/send_aiocb.cpp
	-mkdir -p ../../../../build/yield/sockets/aio/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/win32/send_aiocb.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/aio/win32/send_aiocb.cpp


