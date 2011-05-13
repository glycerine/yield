TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../include
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
LIBS += -lyield_thread -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/queue_test -name "*.d")


O_FILE_PATHS += ../../../build/yield/queue_test/blocking_concurrent_queue_test.o ../../../build/yield/queue_test/non_blocking_concurrent_queue_test.o ../../../build/yield/queue_test/rendezvous_concurrent_queue_test.o ../../../build/yield/queue_test/stl_event_queue_test.o ../../../build/yield/queue_test/synchronized_event_queue_test.o ../../../build/yield/queue_test/synchronized_queue_test.o ../../../build/yield/queue_test/synchronized_response_queue_test.o ../../../build/yield/queue_test/tls_concurrent_queue_test.o ../../../build/yield/queue_test/yield_queue_test_main.o


all: ../../../bin/yield/yield_queue_test

clean:
	$(RM) ../../../bin/yield/yield_queue_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ../../../bin/yield/yield_queue_test
	lcov --directory ../../../build/yield/queue_test --zerocounters
	-../../../bin/yield/yield_queue_test
	lcov --base-directory . --directory ../../../build/yield/queue_test --capture --output-file yield.queue_test_lcov-$(TIMESTAMP)
	mkdir yield.queue_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.queue_test_lcov_html-$(TIMESTAMP) yield.queue_test_lcov-$(TIMESTAMP)
	-cp -R yield.queue_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.queue_test_lcov_html-$(TIMESTAMP).zip yield.queue_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.queue_test_lcov_html-$(TIMESTAMP)


../../../lib/yield/libyield_queue.a:
	$(MAKE) -C . yield.queue.Makefile


../../../bin/yield/yield_queue_test: $(O_FILE_PATHS) ../../../lib/yield/libyield_queue.a
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../build/yield/queue_test/blocking_concurrent_queue_test.o: ../../../test/yield/queue/blocking_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/queue_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/queue_test/blocking_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/queue/blocking_concurrent_queue_test.cpp

../../../build/yield/queue_test/non_blocking_concurrent_queue_test.o: ../../../test/yield/queue/non_blocking_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/queue_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/queue_test/non_blocking_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/queue/non_blocking_concurrent_queue_test.cpp

../../../build/yield/queue_test/rendezvous_concurrent_queue_test.o: ../../../test/yield/queue/rendezvous_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/queue_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/queue_test/rendezvous_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/queue/rendezvous_concurrent_queue_test.cpp

../../../build/yield/queue_test/stl_event_queue_test.o: ../../../test/yield/queue/stl_event_queue_test.cpp
	-mkdir -p ../../../build/yield/queue_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/queue_test/stl_event_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/queue/stl_event_queue_test.cpp

../../../build/yield/queue_test/synchronized_event_queue_test.o: ../../../test/yield/queue/synchronized_event_queue_test.cpp
	-mkdir -p ../../../build/yield/queue_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/queue_test/synchronized_event_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/queue/synchronized_event_queue_test.cpp

../../../build/yield/queue_test/synchronized_queue_test.o: ../../../test/yield/queue/synchronized_queue_test.cpp
	-mkdir -p ../../../build/yield/queue_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/queue_test/synchronized_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/queue/synchronized_queue_test.cpp

../../../build/yield/queue_test/synchronized_response_queue_test.o: ../../../test/yield/queue/synchronized_response_queue_test.cpp
	-mkdir -p ../../../build/yield/queue_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/queue_test/synchronized_response_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/queue/synchronized_response_queue_test.cpp

../../../build/yield/queue_test/tls_concurrent_queue_test.o: ../../../test/yield/queue/tls_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/queue_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/queue_test/tls_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/queue/tls_concurrent_queue_test.cpp

../../../build/yield/queue_test/yield_queue_test_main.o: ../../../test/yield/queue/yield_queue_test_main.cpp
	-mkdir -p ../../../build/yield/queue_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/queue_test/yield_queue_test_main.o -MD $(CXXFLAGS) ../../../test/yield/queue/yield_queue_test_main.cpp
