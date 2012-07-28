TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../../../include
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



LDFLAGS += -L../../../../../lib/yield
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
	LIBS += -lpthread -liconv -lintl
endif
ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++ -lpthread
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++ -lkstat -lnsl -lsocket -liconv
endif
LIBS += -lyield -lyield_thread -lyield_stage -lyield_sockets -lyield_poll -lyield_sockets_aio -lyield_uri -lyield_http -lyield_i18n -lyield_fs -lyield_http_server


D_FILE_PATHS := $(shell find ../../../../../build/yield/http/server/ygi -name "*.d")


O_FILE_PATHS += ../../../../../build/yield/http/server/ygi/ygi_request.o ../../../../../build/yield/http/server/ygi/ygi_request_handler.o


all: ../../../../../lib/yield/libyield_http_server_ygi.a

clean:
	$(RM) ../../../../../lib/yield/libyield_http_server_ygi.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../../../lib/yield/libyield_http_server.a:
	$(MAKE) -C .. yield.http.server.Makefile


../../../../../lib/yield/libyield_http_server_ygi.a: $(O_FILE_PATHS) ../../../../../lib/yield/libyield_http_server.a
	-mkdir -p ../../../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../../../build/yield/http/server/ygi/ygi_request.o: ../../../../../src/yield/http/server/ygi/ygi_request.cpp
	-mkdir -p ../../../../../build/yield/http/server/ygi 2>/dev/null
	$(CXX) -c -o ../../../../../build/yield/http/server/ygi/ygi_request.o -MD $(CXXFLAGS) ../../../../../src/yield/http/server/ygi/ygi_request.cpp

../../../../../build/yield/http/server/ygi/ygi_request_handler.o: ../../../../../src/yield/http/server/ygi/ygi_request_handler.cpp
	-mkdir -p ../../../../../build/yield/http/server/ygi 2>/dev/null
	$(CXX) -c -o ../../../../../build/yield/http/server/ygi/ygi_request_handler.o -MD $(CXXFLAGS) ../../../../../src/yield/http/server/ygi/ygi_request_handler.cpp
