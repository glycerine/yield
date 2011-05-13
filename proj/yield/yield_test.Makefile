TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../include
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



LDFLAGS += -L../../lib/yield
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
LIBS += -lyield


D_FILE_PATHS := $(shell find ../../build/yield_test -name "*.d")


O_FILE_PATHS += ../../build/yield_test/atomic_test.o ../../build/yield_test/auto_object_test.o ../../build/yield_test/buffer_test.o ../../build/yield_test/buffers_test.o ../../build/yield_test/channel_test.o ../../build/yield_test/date_time_test.o ../../build/yield_test/event_test.o ../../build/yield_test/exception_test.o ../../build/yield_test/log_test.o ../../build/yield_test/message_test.o ../../build/yield_test/object_test.o ../../build/yield_test/request_test.o ../../build/yield_test/response_test.o ../../build/yield_test/time_test.o ../../build/yield_test/yield_test_main.o


all: ../../bin/yield/yield_test

clean:
	$(RM) ../../bin/yield/yield_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ../../bin/yield/yield_test
	lcov --directory ../../build/yield_test --zerocounters
	-../../bin/yield/yield_test
	lcov --base-directory . --directory ../../build/yield_test --capture --output-file yield_test_lcov-$(TIMESTAMP)
	mkdir yield_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield_test_lcov_html-$(TIMESTAMP) yield_test_lcov-$(TIMESTAMP)
	-cp -R yield_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield_test_lcov_html-$(TIMESTAMP).zip yield_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield_test_lcov_html-$(TIMESTAMP)


../../lib/yield/libyield.a:
	$(MAKE) -C . yield.Makefile


../../bin/yield/yield_test: $(O_FILE_PATHS) ../../lib/yield/libyield.a
	-mkdir -p ../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../build/yield_test/atomic_test.o: ../../test/yield/atomic_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/atomic_test.o -MD $(CXXFLAGS) ../../test/yield/atomic_test.cpp

../../build/yield_test/auto_object_test.o: ../../test/yield/auto_object_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/auto_object_test.o -MD $(CXXFLAGS) ../../test/yield/auto_object_test.cpp

../../build/yield_test/buffer_test.o: ../../test/yield/buffer_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/buffer_test.o -MD $(CXXFLAGS) ../../test/yield/buffer_test.cpp

../../build/yield_test/buffers_test.o: ../../test/yield/buffers_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/buffers_test.o -MD $(CXXFLAGS) ../../test/yield/buffers_test.cpp

../../build/yield_test/channel_test.o: ../../test/yield/channel_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/channel_test.o -MD $(CXXFLAGS) ../../test/yield/channel_test.cpp

../../build/yield_test/date_time_test.o: ../../test/yield/date_time_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/date_time_test.o -MD $(CXXFLAGS) ../../test/yield/date_time_test.cpp

../../build/yield_test/event_test.o: ../../test/yield/event_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/event_test.o -MD $(CXXFLAGS) ../../test/yield/event_test.cpp

../../build/yield_test/exception_test.o: ../../test/yield/exception_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/exception_test.o -MD $(CXXFLAGS) ../../test/yield/exception_test.cpp

../../build/yield_test/log_test.o: ../../test/yield/log_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/log_test.o -MD $(CXXFLAGS) ../../test/yield/log_test.cpp

../../build/yield_test/message_test.o: ../../test/yield/message_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/message_test.o -MD $(CXXFLAGS) ../../test/yield/message_test.cpp

../../build/yield_test/object_test.o: ../../test/yield/object_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/object_test.o -MD $(CXXFLAGS) ../../test/yield/object_test.cpp

../../build/yield_test/request_test.o: ../../test/yield/request_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/request_test.o -MD $(CXXFLAGS) ../../test/yield/request_test.cpp

../../build/yield_test/response_test.o: ../../test/yield/response_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/response_test.o -MD $(CXXFLAGS) ../../test/yield/response_test.cpp

../../build/yield_test/time_test.o: ../../test/yield/time_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/time_test.o -MD $(CXXFLAGS) ../../test/yield/time_test.cpp

../../build/yield_test/yield_test_main.o: ../../test/yield/yield_test_main.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/yield_test_main.o -MD $(CXXFLAGS) ../../test/yield/yield_test_main.cpp
