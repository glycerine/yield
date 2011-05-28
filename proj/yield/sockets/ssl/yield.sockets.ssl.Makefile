TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -DYIELD_HAVE_OPENSSL -I../../../../include
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -I/usr/local/include
endif
ifeq ($(UNAME), Linux)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Solaris)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wunused
endif
ifeq ($(UNAME), Darwin)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wunused
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


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lnsl -lsocket -lm -lrt -lstdc++
endif
ifeq ($(UNAME), FreeBSD)
	LIBS += -lcrypto -lssl
endif
ifeq ($(UNAME), Linux)
	LIBS += -lcrypto -lssl
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lcrypto -lssl
endif
ifeq ($(UNAME), Darwin)
	LIBS += -lcrypto -lssl
endif
LIBS += -lyield_sockets -lyield


D_FILE_PATHS := $(shell find ../../../../build/yield/sockets/ssl -name "*.d")


O_FILE_PATHS += ../../../../build/yield/sockets/ssl/ssl_context.o ../../../../build/yield/sockets/ssl/ssl_exception.o ../../../../build/yield/sockets/ssl/ssl_socket.o


all: ../../../../lib/yield/libyield_sockets_ssl.a

clean:
	$(RM) ../../../../lib/yield/libyield_sockets_ssl.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../../lib/yield/libyield_sockets.a:
	$(MAKE) -C .. yield.sockets.Makefile


../../../../lib/yield/libyield_sockets_ssl.a: $(O_FILE_PATHS) ../../../../lib/yield/libyield_sockets.a
	-mkdir -p ../../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../../build/yield/sockets/ssl/ssl_context.o: ../../../../src/yield/sockets/ssl/ssl_context.cpp
	-mkdir -p ../../../../build/yield/sockets/ssl 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/ssl/ssl_context.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/ssl/ssl_context.cpp

../../../../build/yield/sockets/ssl/ssl_exception.o: ../../../../src/yield/sockets/ssl/ssl_exception.cpp
	-mkdir -p ../../../../build/yield/sockets/ssl 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/ssl/ssl_exception.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/ssl/ssl_exception.cpp

../../../../build/yield/sockets/ssl/ssl_socket.o: ../../../../src/yield/sockets/ssl/ssl_socket.cpp
	-mkdir -p ../../../../build/yield/sockets/ssl 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/ssl/ssl_socket.o -MD $(CXXFLAGS) ../../../../src/yield/sockets/ssl/ssl_socket.cpp
