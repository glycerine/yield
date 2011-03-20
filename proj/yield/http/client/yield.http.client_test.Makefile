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
	LIBS += -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lnsl -lsocket -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_http_client -lyield_sockets_client -lyield_http_server -lyield_fs -lyield_i18n -lyield_http -lyield_uri -lyield_sockets_server -lyield_sockets_aio -lyield_aio -lyield_sockets_poll -lyield_poll -lyield_sockets -lyield_stage -lyield_thread -lyield


D_FILE_PATHS := $(shell find ../../../../build/yield/http/client -name "*.d")


O_FILE_PATHS += ../../../../build/yield/http/client/http_client_test.o ../../../../build/yield/http/client/yield_http_client_test_main.o


all: ../../../../bin/yield/yield_http_client_test

clean:
	$(RM) ../../../../bin/yield/yield_http_client_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ../../../../bin/yield/yield_http_client_test
	lcov --directory ../../../../build/yield/http/client --zerocounters
	../../../../bin/yield/yield_http_client_test
	lcov --base-directory . --directory ../../../../build/yield/http/client --capture --output-file yield.http.client_test_lcov-$(TIMESTAMP)
	mkdir yield.http.client_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.http.client_test_lcov_html-$(TIMESTAMP) yield.http.client_test_lcov-$(TIMESTAMP)
	-cp -R yield.http.client_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.http.client_test_lcov_html-$(TIMESTAMP).zip yield.http.client_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.http.client_test_lcov_html-$(TIMESTAMP)


../../../../bin/yield/yield_http_client_test: $(O_FILE_PATHS)
	-mkdir -p ../../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../../build/yield/http/client/http_client_test.o: ../../../../test/yield/http/client/http_client_test.cpp
	-mkdir -p ../../../../build/yield/http/client 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/http/client/http_client_test.o -MD $(CXXFLAGS) ../../../../test/yield/http/client/http_client_test.cpp

../../../../build/yield/http/client/yield_http_client_test_main.o: ../../../../test/yield/http/client/yield_http_client_test_main.cpp
	-mkdir -p ../../../../build/yield/http/client 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/http/client/yield_http_client_test_main.o -MD $(CXXFLAGS) ../../../../test/yield/http/client/yield_http_client_test_main.cpp
