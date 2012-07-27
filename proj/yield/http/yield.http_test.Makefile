TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../include
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -I/usr/local/include
endif
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Linux)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Darwin)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Solaris)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused
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
	LIBS += -lm -lrt -lstdc++
endif
ifeq ($(UNAME), MINGW32)
	LIBS += -lgtestd
endif
LIBS += -lyield_http -lyield_uri -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/http -name "*.d")


O_FILE_PATHS += ../../../build/yield/http/http_message_parser_test.o ../../../build/yield/http/http_message_test.o ../../../build/yield/http/http_request_parser_test.o ../../../build/yield/http/http_request_test.o ../../../build/yield/http/http_response_parser_test.o ../../../build/yield/http/http_response_test.o ../../../build/yield/http/yield_http_test_main.o


all: ../../../bin/yield/yield_http_test

clean:
	$(RM) ../../../bin/yield/yield_http_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)

lcov: ../../../bin/yield/yield_http_test
	lcov --directory ../../../build/yield/http --zerocounters
	-../../../bin/yield/yield_http_test
	lcov --base-directory . --directory ../../../build/yield/http --capture --output-file yield.http_test_lcov-$(TIMESTAMP)
	mkdir yield.http_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.http_test_lcov_html-$(TIMESTAMP) yield.http_test_lcov-$(TIMESTAMP)
	-cp -R yield.http_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.http_test_lcov_html-$(TIMESTAMP).zip yield.http_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.http_test_lcov_html-$(TIMESTAMP)


../../../lib/yield/libyield_http.a:
	$(MAKE) -C . yield.http.Makefile


../../../bin/yield/yield_http_test: $(O_FILE_PATHS) ../../../lib/yield/libyield_http.a
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../build/yield/http/http_message_parser_test.o: ../../../test/yield/http/http_message_parser_test.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_message_parser_test.o -MD $(CXXFLAGS) ../../../test/yield/http/http_message_parser_test.cpp

../../../build/yield/http/http_message_test.o: ../../../test/yield/http/http_message_test.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_message_test.o -MD $(CXXFLAGS) ../../../test/yield/http/http_message_test.cpp

../../../build/yield/http/http_request_parser_test.o: ../../../test/yield/http/http_request_parser_test.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_request_parser_test.o -MD $(CXXFLAGS) ../../../test/yield/http/http_request_parser_test.cpp

../../../build/yield/http/http_request_test.o: ../../../test/yield/http/http_request_test.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_request_test.o -MD $(CXXFLAGS) ../../../test/yield/http/http_request_test.cpp

../../../build/yield/http/http_response_parser_test.o: ../../../test/yield/http/http_response_parser_test.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_response_parser_test.o -MD $(CXXFLAGS) ../../../test/yield/http/http_response_parser_test.cpp

../../../build/yield/http/http_response_test.o: ../../../test/yield/http/http_response_test.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_response_test.o -MD $(CXXFLAGS) ../../../test/yield/http/http_response_test.cpp

../../../build/yield/http/yield_http_test_main.o: ../../../test/yield/http/yield_http_test_main.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/yield_http_test_main.o -MD $(CXXFLAGS) ../../../test/yield/http/yield_http_test_main.cpp
