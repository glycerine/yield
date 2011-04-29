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
	LIBS += -liconv -lkstat -lnsl -lsocket -lm -lrt -lstdc++
endif
LIBS += -lyield_http_server -lyield_fs -lyield_i18n -lyield_http -lyield_uri -lyield_sockets_aio -lyield_aio -lyield_sockets_poll -lyield_poll -lyield_thread -lyield_sockets -lyield


D_FILE_PATHS := $(shell find ../../../../build/yield/http/server -name "*.d")


O_FILE_PATHS += ../../../../build/yield/http/server/http_request_queue_test.o ../../../../build/yield/http/server/yield_http_server_test_main.o


all: ../../../../bin/yield/yield_http_server_test

clean:
	$(RM) ../../../../bin/yield/yield_http_server_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ../../../../bin/yield/yield_http_server_test
	lcov --directory ../../../../build/yield/http/server --zerocounters
	../../../../bin/yield/yield_http_server_test
	lcov --base-directory . --directory ../../../../build/yield/http/server --capture --output-file yield.http.server_test_lcov-$(TIMESTAMP)
	mkdir yield.http.server_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.http.server_test_lcov_html-$(TIMESTAMP) yield.http.server_test_lcov-$(TIMESTAMP)
	-cp -R yield.http.server_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.http.server_test_lcov_html-$(TIMESTAMP).zip yield.http.server_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.http.server_test_lcov_html-$(TIMESTAMP)


../../../../lib/yield/libyield_http_server.a:
	$(MAKE) -C . yield.http.server.Makefile


../../../../bin/yield/yield_http_server_test: $(O_FILE_PATHS) ../../../../lib/yield/libyield_http_server.a
	-mkdir -p ../../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../../build/yield/http/server/http_request_queue_test.o: ../../../../test/yield/http/server/http_request_queue_test.cpp
	-mkdir -p ../../../../build/yield/http/server 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/http/server/http_request_queue_test.o -MD $(CXXFLAGS) ../../../../test/yield/http/server/http_request_queue_test.cpp

../../../../build/yield/http/server/yield_http_server_test_main.o: ../../../../test/yield/http/server/yield_http_server_test_main.cpp
	-mkdir -p ../../../../build/yield/http/server 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/http/server/yield_http_server_test_main.o -MD $(CXXFLAGS) ../../../../test/yield/http/server/yield_http_server_test_main.cpp
