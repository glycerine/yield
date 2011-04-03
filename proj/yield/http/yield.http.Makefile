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
	LIBS += -lm -lrt -lstdc++
endif
LIBS += -lyield_uri -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/http -name "*.d")


O_FILE_PATHS += ../../../build/yield/http/http_message.o ../../../build/yield/http/http_message_parser.o ../../../build/yield/http/http_request.o ../../../build/yield/http/http_request_handler.o ../../../build/yield/http/http_request_parser.o ../../../build/yield/http/http_response.o ../../../build/yield/http/http_response_parser.o


all: ../../../lib/yield/libyield_http.a

clean:
	$(RM) ../../../lib/yield/libyield_http.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../lib/yield/libyield_uri.a:
	$(MAKE) -C ../uri yield.uri.Makefile


../../../lib/yield/libyield_http.a: $(O_FILE_PATHS) ../../../lib/yield/libyield_uri.a
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../build/yield/http/http_message.o: ../../../src/yield/http/http_message.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_message.o -MD $(CXXFLAGS) ../../../src/yield/http/http_message.cpp

../../../build/yield/http/http_message_parser.o: ../../../src/yield/http/http_message_parser.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_message_parser.o -MD $(CXXFLAGS) ../../../src/yield/http/http_message_parser.cpp

../../../build/yield/http/http_request.o: ../../../src/yield/http/http_request.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_request.o -MD $(CXXFLAGS) ../../../src/yield/http/http_request.cpp

../../../build/yield/http/http_request_handler.o: ../../../src/yield/http/http_request_handler.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_request_handler.o -MD $(CXXFLAGS) ../../../src/yield/http/http_request_handler.cpp

../../../build/yield/http/http_request_parser.o: ../../../src/yield/http/http_request_parser.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_request_parser.o -MD $(CXXFLAGS) ../../../src/yield/http/http_request_parser.cpp

../../../build/yield/http/http_response.o: ../../../src/yield/http/http_response.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_response.o -MD $(CXXFLAGS) ../../../src/yield/http/http_response.cpp

../../../build/yield/http/http_response_parser.o: ../../../src/yield/http/http_response_parser.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_response_parser.o -MD $(CXXFLAGS) ../../../src/yield/http/http_response_parser.cpp
