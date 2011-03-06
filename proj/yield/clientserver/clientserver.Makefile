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
	LIBS += -liconv -lintl -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -laio -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lnsl -lsocket -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_aio -lyield_fs -lyield_i18n -lyield_poll -lyield_sockets -lyield_stage -lyield_thread -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/clientserver -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/clientserver/socket_client.o ../../../build/yield/clientserver/socket_peer.o ../../../build/yield/clientserver/socket_server.o ../../../build/yield/clientserver/stream_socket_client.o ../../../build/yield/clientserver/stream_socket_peer.o ../../../build/yield/clientserver/stream_socket_server.o


../../../lib/yield/libyield_clientserver.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/yield/libyield_clientserver.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/clientserver/socket_client.o: ../../../src/yield/clientserver/socket_client.cpp
	-mkdir -p ../../../build/yield/clientserver 2>/dev/null
	$(CXX) -c -o ../../../build/yield/clientserver/socket_client.o -MD $(CXXFLAGS) ../../../src/yield/clientserver/socket_client.cpp

../../../build/yield/clientserver/socket_peer.o: ../../../src/yield/clientserver/socket_peer.cpp
	-mkdir -p ../../../build/yield/clientserver 2>/dev/null
	$(CXX) -c -o ../../../build/yield/clientserver/socket_peer.o -MD $(CXXFLAGS) ../../../src/yield/clientserver/socket_peer.cpp

../../../build/yield/clientserver/socket_server.o: ../../../src/yield/clientserver/socket_server.cpp
	-mkdir -p ../../../build/yield/clientserver 2>/dev/null
	$(CXX) -c -o ../../../build/yield/clientserver/socket_server.o -MD $(CXXFLAGS) ../../../src/yield/clientserver/socket_server.cpp

../../../build/yield/clientserver/stream_socket_client.o: ../../../src/yield/clientserver/stream_socket_client.cpp
	-mkdir -p ../../../build/yield/clientserver 2>/dev/null
	$(CXX) -c -o ../../../build/yield/clientserver/stream_socket_client.o -MD $(CXXFLAGS) ../../../src/yield/clientserver/stream_socket_client.cpp

../../../build/yield/clientserver/stream_socket_peer.o: ../../../src/yield/clientserver/stream_socket_peer.cpp
	-mkdir -p ../../../build/yield/clientserver 2>/dev/null
	$(CXX) -c -o ../../../build/yield/clientserver/stream_socket_peer.o -MD $(CXXFLAGS) ../../../src/yield/clientserver/stream_socket_peer.cpp

../../../build/yield/clientserver/stream_socket_server.o: ../../../src/yield/clientserver/stream_socket_server.cpp
	-mkdir -p ../../../build/yield/clientserver 2>/dev/null
	$(CXX) -c -o ../../../build/yield/clientserver/stream_socket_server.o -MD $(CXXFLAGS) ../../../src/yield/clientserver/stream_socket_server.cpp


