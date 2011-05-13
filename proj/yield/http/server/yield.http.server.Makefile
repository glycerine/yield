TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../../include
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -I/usr/local/include
endif
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
else ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif



LDFLAGS += -L../../../../lib/yield
ifeq ($(UNAME), FreeBSD)
	LDFLAGS += -L/usr/local/lib
endif
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
LIBS += -lyield_fs -lyield_i18n -lyield_http -lyield_uri -lyield_sockets_aio -lyield_sockets_poll -lyield_poll -lyield_thread -lyield_sockets -lyield


D_FILE_PATHS := $(shell find ../../../../build/yield/http/server -name "*.d")


O_FILE_PATHS += ../../../../build/yield/http/server/http_connection.o ../../../../build/yield/http/server/http_message_body_chunk.o ../../../../build/yield/http/server/http_request.o ../../../../build/yield/http/server/http_request_parser.o ../../../../build/yield/http/server/http_request_queue.o


all: ../../../../lib/yield/libyield_http_server.a

clean:
	$(RM) ../../../../lib/yield/libyield_http_server.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../../lib/yield/libyield_fs.a:
	$(MAKE) -C ../../fs yield.fs.Makefile


../../../../lib/yield/libyield_http.a:
	$(MAKE) -C .. yield.http.Makefile


../../../../lib/yield/libyield_sockets_aio.a:
	$(MAKE) -C ../../sockets/aio yield.sockets.aio.Makefile


../../../../lib/yield/libyield_http_server.a: $(O_FILE_PATHS) ../../../../lib/yield/libyield_fs.a ../../../../lib/yield/libyield_http.a ../../../../lib/yield/libyield_sockets_aio.a
	-mkdir -p ../../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../../build/yield/http/server/http_connection.o: ../../../../src/yield/http/server/http_connection.cpp
	-mkdir -p ../../../../build/yield/http/server 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/http/server/http_connection.o -MD $(CXXFLAGS) ../../../../src/yield/http/server/http_connection.cpp

../../../../build/yield/http/server/http_message_body_chunk.o: ../../../../src/yield/http/server/http_message_body_chunk.cpp
	-mkdir -p ../../../../build/yield/http/server 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/http/server/http_message_body_chunk.o -MD $(CXXFLAGS) ../../../../src/yield/http/server/http_message_body_chunk.cpp

../../../../build/yield/http/server/http_request.o: ../../../../src/yield/http/server/http_request.cpp
	-mkdir -p ../../../../build/yield/http/server 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/http/server/http_request.o -MD $(CXXFLAGS) ../../../../src/yield/http/server/http_request.cpp

../../../../build/yield/http/server/http_request_parser.o: ../../../../src/yield/http/server/http_request_parser.cpp
	-mkdir -p ../../../../build/yield/http/server 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/http/server/http_request_parser.o -MD $(CXXFLAGS) ../../../../src/yield/http/server/http_request_parser.cpp

../../../../build/yield/http/server/http_request_queue.o: ../../../../src/yield/http/server/http_request_queue.cpp
	-mkdir -p ../../../../build/yield/http/server 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/http/server/http_request_queue.o -MD $(CXXFLAGS) ../../../../src/yield/http/server/http_request_queue.cpp
