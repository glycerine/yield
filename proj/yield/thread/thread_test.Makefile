# SHELL = /bin/bash
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
	CXXFLAGS += /EHsc /GR- /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
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


LDFLAGS += -L../../../lib
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), FreeBSD)
	LIBS += -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++ -lpthread
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++ -lcpc -lkstat
endif
LIBS += -lyield_thread -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/thread_test -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/thread_test/blocking_concurrent_queue_test.o ../../../build/yield/thread_test/condition_variable_test.o ../../../build/yield/thread_test/lightweight_mutex_test.o ../../../build/yield/thread_test/mutex_test.o ../../../build/yield/thread_test/non_blocking_concurrent_queue_test.o ../../../build/yield/thread_test/performance_counter_set_test.o ../../../build/yield/thread_test/pipe_test.o ../../../build/yield/thread_test/processor_set_test.o ../../../build/yield/thread_test/reader_writer_lock_test.o ../../../build/yield/thread_test/sampler_test.o ../../../build/yield/thread_test/semaphore_test.o ../../../build/yield/thread_test/synchronized_queue_test.o ../../../build/yield/thread_test/thread_test.o ../../../build/yield/thread_test/tls_concurrent_queue_test.o ../../../build/yield/thread_test/unit_concurrent_queue_test.o ../../../build/yield/thread_test/yield_thread_test_main.o


../../../bin/yield_thread_test: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../bin 2>/dev/null
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../bin/yield_thread_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/thread_test/blocking_concurrent_queue_test.o: ../../../src/yield/thread/blocking_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/blocking_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/blocking_concurrent_queue_test.cpp

../../../build/yield/thread_test/condition_variable_test.o: ../../../src/yield/thread/condition_variable_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/condition_variable_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/condition_variable_test.cpp

../../../build/yield/thread_test/lightweight_mutex_test.o: ../../../src/yield/thread/lightweight_mutex_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/lightweight_mutex_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/lightweight_mutex_test.cpp

../../../build/yield/thread_test/mutex_test.o: ../../../src/yield/thread/mutex_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/mutex_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/mutex_test.cpp

../../../build/yield/thread_test/non_blocking_concurrent_queue_test.o: ../../../src/yield/thread/non_blocking_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/non_blocking_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/non_blocking_concurrent_queue_test.cpp

../../../build/yield/thread_test/performance_counter_set_test.o: ../../../src/yield/thread/performance_counter_set_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/performance_counter_set_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/performance_counter_set_test.cpp

../../../build/yield/thread_test/pipe_test.o: ../../../src/yield/thread/pipe_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/pipe_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/pipe_test.cpp

../../../build/yield/thread_test/processor_set_test.o: ../../../src/yield/thread/processor_set_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/processor_set_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/processor_set_test.cpp

../../../build/yield/thread_test/reader_writer_lock_test.o: ../../../src/yield/thread/reader_writer_lock_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/reader_writer_lock_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/reader_writer_lock_test.cpp

../../../build/yield/thread_test/sampler_test.o: ../../../src/yield/thread/sampler_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/sampler_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/sampler_test.cpp

../../../build/yield/thread_test/semaphore_test.o: ../../../src/yield/thread/semaphore_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/semaphore_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/semaphore_test.cpp

../../../build/yield/thread_test/synchronized_queue_test.o: ../../../src/yield/thread/synchronized_queue_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/synchronized_queue_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/synchronized_queue_test.cpp

../../../build/yield/thread_test/thread_test.o: ../../../src/yield/thread/thread_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/thread_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/thread_test.cpp

../../../build/yield/thread_test/tls_concurrent_queue_test.o: ../../../src/yield/thread/tls_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/tls_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/tls_concurrent_queue_test.cpp

../../../build/yield/thread_test/unit_concurrent_queue_test.o: ../../../src/yield/thread/unit_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/unit_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../src/yield/thread/unit_concurrent_queue_test.cpp

../../../build/yield/thread_test/yield_thread_test_main.o: ../../../src/yield/thread/yield_thread_test_main.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/yield_thread_test_main.o -MD $(CXXFLAGS) ../../../src/yield/thread/yield_thread_test_main.cpp


