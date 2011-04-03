TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
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


LDFLAGS += -L../../../lib/yield
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lnsl -lsocket -lm -lrt -lstdc++
endif
LIBS += -lyield_sockets -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/sockets -name "*.d")


O_FILE_PATHS += ../../../build/yield/sockets/socket_address_test.o ../../../build/yield/sockets/socket_pair_test.o ../../../build/yield/sockets/tcp_socket_test.o ../../../build/yield/sockets/udp_socket_test.o ../../../build/yield/sockets/yield_sockets_test_main.o


all: ../../../bin/yield/yield_sockets_test

clean:
	$(RM) ../../../bin/yield/yield_sockets_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ../../../bin/yield/yield_sockets_test
	lcov --directory ../../../build/yield/sockets --zerocounters
	../../../bin/yield/yield_sockets_test
	lcov --base-directory . --directory ../../../build/yield/sockets --capture --output-file yield.sockets_test_lcov-$(TIMESTAMP)
	mkdir yield.sockets_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.sockets_test_lcov_html-$(TIMESTAMP) yield.sockets_test_lcov-$(TIMESTAMP)
	-cp -R yield.sockets_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.sockets_test_lcov_html-$(TIMESTAMP).zip yield.sockets_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.sockets_test_lcov_html-$(TIMESTAMP)


../../../lib/yield/libyield_sockets.a
	$(MAKE) yield.sockets.Makefile


../../../bin/yield/yield_sockets_test: $(O_FILE_PATHS) ../../../lib/yield/libyield_sockets.a
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../build/yield/sockets/socket_address_test.o: ../../../test/yield/sockets/socket_address_test.cpp
	-mkdir -p ../../../build/yield/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/socket_address_test.o -MD $(CXXFLAGS) ../../../test/yield/sockets/socket_address_test.cpp

../../../build/yield/sockets/socket_pair_test.o: ../../../test/yield/sockets/socket_pair_test.cpp
	-mkdir -p ../../../build/yield/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/socket_pair_test.o -MD $(CXXFLAGS) ../../../test/yield/sockets/socket_pair_test.cpp

../../../build/yield/sockets/tcp_socket_test.o: ../../../test/yield/sockets/tcp_socket_test.cpp
	-mkdir -p ../../../build/yield/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/tcp_socket_test.o -MD $(CXXFLAGS) ../../../test/yield/sockets/tcp_socket_test.cpp

../../../build/yield/sockets/udp_socket_test.o: ../../../test/yield/sockets/udp_socket_test.cpp
	-mkdir -p ../../../build/yield/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/udp_socket_test.o -MD $(CXXFLAGS) ../../../test/yield/sockets/udp_socket_test.cpp

../../../build/yield/sockets/yield_sockets_test_main.o: ../../../test/yield/sockets/yield_sockets_test_main.cpp
	-mkdir -p ../../../build/yield/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/sockets/yield_sockets_test_main.o -MD $(CXXFLAGS) ../../../test/yield/sockets/yield_sockets_test_main.cpp
