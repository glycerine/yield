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
LIBS += -lyield_fs -lyield_i18n -lyield_http -lyield_uri -lyield_sockets_server -lyield_sockets_aio -lyield_aio -lyield_sockets_poll -lyield_poll -lyield_sockets -lyield_stage -lyield_thread -lyield


D_FILE_PATHS := $(shell find ../../../../build/yield/http/server -name "*.d")


O_FILE_PATHS += ../../../../build/yield/http/server/access_log.o ../../../../build/yield/http/server/file_access_log.o ../../../../build/yield/http/server/http_server.o


all: ../../../../lib/yield/libyield_http_server.a

clean:
	$(RM) ../../../../lib/yield/libyield_http_server.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../../lib/yield/libyield_http_server.a: $(O_FILE_PATHS)
	-mkdir -p ../../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../../build/yield/http/server/access_log.o: ../../../../src/yield/http/server/access_log.cpp
	-mkdir -p ../../../../build/yield/http/server 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/http/server/access_log.o -MD $(CXXFLAGS) ../../../../src/yield/http/server/access_log.cpp

../../../../build/yield/http/server/file_access_log.o: ../../../../src/yield/http/server/file_access_log.cpp
	-mkdir -p ../../../../build/yield/http/server 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/http/server/file_access_log.o -MD $(CXXFLAGS) ../../../../src/yield/http/server/file_access_log.cpp

../../../../build/yield/http/server/http_server.o: ../../../../src/yield/http/server/http_server.cpp
	-mkdir -p ../../../../build/yield/http/server 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/http/server/http_server.o -MD $(CXXFLAGS) ../../../../src/yield/http/server/http_server.cpp
