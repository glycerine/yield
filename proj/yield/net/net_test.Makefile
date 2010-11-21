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


LDFLAGS += -L../../../lib
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++ -lnsl -lsocket -luuid
endif
LIBS += -lyield_net -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/net_test -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/net_test/sockets/socket_address_test.o ../../../build/yield/net_test/sockets/socket_pair_test.o ../../../build/yield/net_test/sockets/ssl/ssl_context_test.o ../../../build/yield/net_test/sockets/tcp_socket_test.o ../../../build/yield/net_test/sockets/udp_socket_test.o ../../../build/yield/net_test/uri_test.o ../../../build/yield/net_test/uuid_test.o ../../../build/yield/net_test/yield_net_test_main.o


../../../bin/yield_net_test: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../bin 2>/dev/null
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../bin/yield_net_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/net_test/sockets/socket_address_test.o: ../../../src/yield/net/sockets/socket_address_test.cpp
	-mkdir -p ../../../build/yield/net_test/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/net_test/sockets/socket_address_test.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/socket_address_test.cpp

../../../build/yield/net_test/sockets/socket_pair_test.o: ../../../src/yield/net/sockets/socket_pair_test.cpp
	-mkdir -p ../../../build/yield/net_test/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/net_test/sockets/socket_pair_test.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/socket_pair_test.cpp

../../../build/yield/net_test/sockets/ssl/ssl_context_test.o: ../../../src/yield/net/sockets/ssl/ssl_context_test.cpp
	-mkdir -p ../../../build/yield/net_test/sockets/ssl 2>/dev/null
	$(CXX) -c -o ../../../build/yield/net_test/sockets/ssl/ssl_context_test.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/ssl/ssl_context_test.cpp

../../../build/yield/net_test/sockets/tcp_socket_test.o: ../../../src/yield/net/sockets/tcp_socket_test.cpp
	-mkdir -p ../../../build/yield/net_test/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/net_test/sockets/tcp_socket_test.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/tcp_socket_test.cpp

../../../build/yield/net_test/sockets/udp_socket_test.o: ../../../src/yield/net/sockets/udp_socket_test.cpp
	-mkdir -p ../../../build/yield/net_test/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/net_test/sockets/udp_socket_test.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/udp_socket_test.cpp

../../../build/yield/net_test/uri_test.o: ../../../src/yield/net/uri_test.cpp
	-mkdir -p ../../../build/yield/net_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/net_test/uri_test.o -MD $(CXXFLAGS) ../../../src/yield/net/uri_test.cpp

../../../build/yield/net_test/uuid_test.o: ../../../src/yield/net/uuid_test.cpp
	-mkdir -p ../../../build/yield/net_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/net_test/uuid_test.o -MD $(CXXFLAGS) ../../../src/yield/net/uuid_test.cpp

../../../build/yield/net_test/yield_net_test_main.o: ../../../src/yield/net/yield_net_test_main.cpp
	-mkdir -p ../../../build/yield/net_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/net_test/yield_net_test_main.o -MD $(CXXFLAGS) ../../../src/yield/net/yield_net_test_main.cpp


