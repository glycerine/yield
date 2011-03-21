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


ifeq ($(UNAME), FreeBSD)
	LIBS += -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_thread -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/thread -name "*.d")


O_FILE_PATHS += ../../../build/yield/thread/blocking_concurrent_queue_test.o ../../../build/yield/thread/condition_variable_test.o ../../../build/yield/thread/lightweight_mutex_test.o ../../../build/yield/thread/mutex_test.o ../../../build/yield/thread/non_blocking_concurrent_queue_test.o ../../../build/yield/thread/performance_counter_set_test.o ../../../build/yield/thread/pipe_test.o ../../../build/yield/thread/processor_set_test.o ../../../build/yield/thread/reader_writer_lock_test.o ../../../build/yield/thread/sampler_test.o ../../../build/yield/thread/semaphore_test.o ../../../build/yield/thread/synchronized_event_queue_test.o ../../../build/yield/thread/synchronized_queue_test.o ../../../build/yield/thread/synchronized_response_queue_test.o ../../../build/yield/thread/thread_test.o ../../../build/yield/thread/tls_concurrent_queue_test.o ../../../build/yield/thread/unit_concurrent_queue_test.o ../../../build/yield/thread/yield_thread_test_main.o


all: ../../../bin/yield/yield_thread_test

clean:
	$(RM) ../../../bin/yield/yield_thread_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ../../../bin/yield/yield_thread_test
	lcov --directory ../../../build/yield/thread --zerocounters
	../../../bin/yield/yield_thread_test
	lcov --base-directory . --directory ../../../build/yield/thread --capture --output-file yield.thread_test_lcov-$(TIMESTAMP)
	mkdir yield.thread_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.thread_test_lcov_html-$(TIMESTAMP) yield.thread_test_lcov-$(TIMESTAMP)
	-cp -R yield.thread_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.thread_test_lcov_html-$(TIMESTAMP).zip yield.thread_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.thread_test_lcov_html-$(TIMESTAMP)


../../../bin/yield/yield_thread_test: $(O_FILE_PATHS)
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../build/yield/thread/blocking_concurrent_queue_test.o: ../../../test/yield/thread/blocking_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/blocking_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/blocking_concurrent_queue_test.cpp

../../../build/yield/thread/condition_variable_test.o: ../../../test/yield/thread/condition_variable_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/condition_variable_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/condition_variable_test.cpp

../../../build/yield/thread/lightweight_mutex_test.o: ../../../test/yield/thread/lightweight_mutex_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/lightweight_mutex_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/lightweight_mutex_test.cpp

../../../build/yield/thread/mutex_test.o: ../../../test/yield/thread/mutex_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/mutex_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/mutex_test.cpp

../../../build/yield/thread/non_blocking_concurrent_queue_test.o: ../../../test/yield/thread/non_blocking_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/non_blocking_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/non_blocking_concurrent_queue_test.cpp

../../../build/yield/thread/performance_counter_set_test.o: ../../../test/yield/thread/performance_counter_set_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/performance_counter_set_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/performance_counter_set_test.cpp

../../../build/yield/thread/pipe_test.o: ../../../test/yield/thread/pipe_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/pipe_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/pipe_test.cpp

../../../build/yield/thread/processor_set_test.o: ../../../test/yield/thread/processor_set_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/processor_set_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/processor_set_test.cpp

../../../build/yield/thread/reader_writer_lock_test.o: ../../../test/yield/thread/reader_writer_lock_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/reader_writer_lock_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/reader_writer_lock_test.cpp

../../../build/yield/thread/sampler_test.o: ../../../test/yield/thread/sampler_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/sampler_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/sampler_test.cpp

../../../build/yield/thread/semaphore_test.o: ../../../test/yield/thread/semaphore_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/semaphore_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/semaphore_test.cpp

../../../build/yield/thread/synchronized_event_queue_test.o: ../../../test/yield/thread/synchronized_event_queue_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/synchronized_event_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/synchronized_event_queue_test.cpp

../../../build/yield/thread/synchronized_queue_test.o: ../../../test/yield/thread/synchronized_queue_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/synchronized_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/synchronized_queue_test.cpp

../../../build/yield/thread/synchronized_response_queue_test.o: ../../../test/yield/thread/synchronized_response_queue_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/synchronized_response_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/synchronized_response_queue_test.cpp

../../../build/yield/thread/thread_test.o: ../../../test/yield/thread/thread_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/thread_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/thread_test.cpp

../../../build/yield/thread/tls_concurrent_queue_test.o: ../../../test/yield/thread/tls_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/tls_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/tls_concurrent_queue_test.cpp

../../../build/yield/thread/unit_concurrent_queue_test.o: ../../../test/yield/thread/unit_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/unit_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/unit_concurrent_queue_test.cpp

../../../build/yield/thread/yield_thread_test_main.o: ../../../test/yield/thread/yield_thread_test_main.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/yield_thread_test_main.o -MD $(CXXFLAGS) ../../../test/yield/thread/yield_thread_test_main.cpp
