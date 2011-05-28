TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../include
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -I/usr/local/include
endif
ifeq ($(UNAME), MINGW32)
	CXXFLAGS += /EHsc /GR- /MP /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
endif
ifeq ($(UNAME), Solaris)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wpadded -Wunused
endif
ifeq ($(UNAME), Darwin)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wpadded -Wunused
endif
ifeq ($(UNAME), Linux)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wpadded -Wunused
endif
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wold-style-cast -Wpadded -Wunused
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


ifeq ($(UNAME), FreeBSD)
	LIBS += -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_poll -lyield_thread -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/poll -name "*.d")


O_FILE_PATHS += ../../../build/yield/poll/fd_event_queue_test.o ../../../build/yield/poll/fd_event_test.o ../../../build/yield/poll/yield_poll_test_main.o


all: ../../../bin/yield/yield_poll_test

clean:
	$(RM) ../../../bin/yield/yield_poll_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ../../../bin/yield/yield_poll_test
	lcov --directory ../../../build/yield/poll --zerocounters
	-../../../bin/yield/yield_poll_test
	lcov --base-directory . --directory ../../../build/yield/poll --capture --output-file yield.poll_test_lcov-$(TIMESTAMP)
	mkdir yield.poll_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.poll_test_lcov_html-$(TIMESTAMP) yield.poll_test_lcov-$(TIMESTAMP)
	-cp -R yield.poll_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.poll_test_lcov_html-$(TIMESTAMP).zip yield.poll_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.poll_test_lcov_html-$(TIMESTAMP)


../../../lib/yield/libyield_poll.a:
	$(MAKE) -C . yield.poll.Makefile


../../../bin/yield/yield_poll_test: $(O_FILE_PATHS) ../../../lib/yield/libyield_poll.a
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../build/yield/poll/fd_event_queue_test.o: ../../../test/yield/poll/fd_event_queue_test.cpp
	-mkdir -p ../../../build/yield/poll 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll/fd_event_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/poll/fd_event_queue_test.cpp

../../../build/yield/poll/fd_event_test.o: ../../../test/yield/poll/fd_event_test.cpp
	-mkdir -p ../../../build/yield/poll 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll/fd_event_test.o -MD $(CXXFLAGS) ../../../test/yield/poll/fd_event_test.cpp

../../../build/yield/poll/yield_poll_test_main.o: ../../../test/yield/poll/yield_poll_test_main.cpp
	-mkdir -p ../../../build/yield/poll 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll/yield_poll_test_main.o -MD $(CXXFLAGS) ../../../test/yield/poll/yield_poll_test_main.cpp
