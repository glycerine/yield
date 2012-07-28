TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../../include
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
ifeq ($(UNAME), MINGW32)
	LIBS += -lgtestd
endif
ifeq ($(UNAME), Darwin)
	LIBS += -lgtest
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lgtest
endif
ifeq ($(UNAME), FreeBSD)
	LIBS += -lgtest
endif
ifeq ($(UNAME), Linux)
	LIBS += -lgtest
endif
LIBS += -lyield_fs -lyield_i18n -lyield_sockets_aio -lyield_poll -lyield_thread -lyield_sockets -lyield


D_FILE_PATHS := $(shell find ../../../../build/yield/sockets/aio -name "*.d")


O_FILE_PATHS += ../../../../build/yield/sockets/aio/nbio_queue_test.o ../../../../build/yield/sockets/aio/yield_sockets_aio_test_main.o
ifeq ($(UNAME), MINGW32)
	O_FILE_PATHS += ../../../../build/yield/sockets/aio_test/win32/aio_queue_test.o
endif


all: ../../../../bin/yield/yield_sockets_aio_test

clean:
	$(RM) ../../../../bin/yield/yield_sockets_aio_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)

lcov: ../../../../bin/yield/yield_sockets_aio_test
	lcov --directory ../../../../build/yield/sockets/aio --zerocounters
	-../../../../bin/yield/yield_sockets_aio_test
	lcov --base-directory . --directory ../../../../build/yield/sockets/aio --capture --output-file yield.sockets.aio_test_lcov-$(TIMESTAMP)
	mkdir yield.sockets.aio_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.sockets.aio_test_lcov_html-$(TIMESTAMP) yield.sockets.aio_test_lcov-$(TIMESTAMP)
	-cp -R yield.sockets.aio_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.sockets.aio_test_lcov_html-$(TIMESTAMP).zip yield.sockets.aio_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.sockets.aio_test_lcov_html-$(TIMESTAMP)


../../../../lib/yield/libyield_fs.a:
	$(MAKE) -C ../../fs yield.fs.Makefile


../../../../lib/yield/libyield_sockets_aio.a:
	$(MAKE) -C . yield.sockets.aio.Makefile


../../../../bin/yield/yield_sockets_aio_test: $(O_FILE_PATHS) ../../../../lib/yield/libyield_fs.a ../../../../lib/yield/libyield_sockets_aio.a
	-mkdir -p ../../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../../build/yield/sockets/aio/nbio_queue_test.o: ../../../../test/yield/sockets/aio/nbio_queue_test.cpp
	-mkdir -p ../../../../build/yield/sockets/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/nbio_queue_test.o -MD $(CXXFLAGS) ../../../../test/yield/sockets/aio/nbio_queue_test.cpp

../../../../build/yield/sockets/aio/yield_sockets_aio_test_main.o: ../../../../test/yield/sockets/aio/yield_sockets_aio_test_main.cpp
	-mkdir -p ../../../../build/yield/sockets/aio 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio/yield_sockets_aio_test_main.o -MD $(CXXFLAGS) ../../../../test/yield/sockets/aio/yield_sockets_aio_test_main.cpp

../../../../build/yield/sockets/aio_test/win32/aio_queue_test.o: ../../../../test/yield/sockets/aio/win32/aio_queue_test.cpp
	-mkdir -p ../../../../build/yield/sockets/aio_test/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio_test/win32/aio_queue_test.o -MD $(CXXFLAGS) ../../../../test/yield/sockets/aio/win32/aio_queue_test.cpp
