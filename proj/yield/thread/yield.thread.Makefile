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
	LIBS += -lrt -lstdc++ -lpthread
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++ -lcpc -lkstat
endif
LIBS += -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/thread -name "*.d")


O_FILE_PATHS += ../../../build/yield/thread/condition_variable.o ../../../build/yield/thread/fiber.o ../../../build/yield/thread/lightweight_mutex.o ../../../build/yield/thread/mutex.o ../../../build/yield/thread/performance_counter_set.o ../../../build/yield/thread/processor_set.o ../../../build/yield/thread/reader_writer_lock.o ../../../build/yield/thread/semaphore.o ../../../build/yield/thread/thread.o
ifeq ($(UNAME), Darwin)
	O_FILE_PATHS += ../../../build/yield/thread/darwin/semaphore.o ../../../build/yield/thread/darwin/system_configuration.o ../../../build/yield/thread/darwin/thread.o
	O_FILE_PATHS += ../../../build/yield/thread/posix/condition_variable.o ../../../build/yield/thread/posix/mutex.o ../../../build/yield/thread/posix/pipe.o ../../../build/yield/thread/posix/reader_writer_lock.o ../../../build/yield/thread/posix/semaphore.o ../../../build/yield/thread/posix/thread.o
	O_FILE_PATHS += ../../../build/yield/thread/unix/fiber.o ../../../build/yield/thread/unix/papi_performance_counter_set.o
endif
ifeq ($(UNAME), FreeBSD)
	O_FILE_PATHS += ../../../build/yield/thread/posix/condition_variable.o ../../../build/yield/thread/posix/mutex.o ../../../build/yield/thread/posix/pipe.o ../../../build/yield/thread/posix/reader_writer_lock.o ../../../build/yield/thread/posix/semaphore.o ../../../build/yield/thread/posix/thread.o
	O_FILE_PATHS += ../../../build/yield/thread/unix/fiber.o ../../../build/yield/thread/unix/papi_performance_counter_set.o
endif
ifeq ($(UNAME), Linux)
	O_FILE_PATHS += ../../../build/yield/thread/linux/processor_set.o ../../../build/yield/thread/linux/system_configuration.o ../../../build/yield/thread/linux/thread.o
	O_FILE_PATHS += ../../../build/yield/thread/posix/condition_variable.o ../../../build/yield/thread/posix/mutex.o ../../../build/yield/thread/posix/pipe.o ../../../build/yield/thread/posix/reader_writer_lock.o ../../../build/yield/thread/posix/semaphore.o ../../../build/yield/thread/posix/thread.o
	O_FILE_PATHS += ../../../build/yield/thread/unix/fiber.o ../../../build/yield/thread/unix/papi_performance_counter_set.o
endif
ifeq ($(UNAME), Solaris)
	O_FILE_PATHS += ../../../build/yield/thread/posix/condition_variable.o ../../../build/yield/thread/posix/mutex.o ../../../build/yield/thread/posix/pipe.o ../../../build/yield/thread/posix/reader_writer_lock.o ../../../build/yield/thread/posix/semaphore.o ../../../build/yield/thread/posix/thread.o
	O_FILE_PATHS += ../../../build/yield/thread/sunos/cpc_performance_counter_set.o ../../../build/yield/thread/sunos/processor_set.o ../../../build/yield/thread/sunos/system_configuration.o ../../../build/yield/thread/sunos/thread.o
	O_FILE_PATHS += ../../../build/yield/thread/unix/fiber.o ../../../build/yield/thread/unix/papi_performance_counter_set.o
endif
ifeq ($(UNAME), MINGW32)
	O_FILE_PATHS += ../../../build/yield/thread/win32/condition_variable.o ../../../build/yield/thread/win32/fiber.o ../../../build/yield/thread/win32/lightweight_mutex.o ../../../build/yield/thread/win32/mutex.o ../../../build/yield/thread/win32/pipe.o ../../../build/yield/thread/win32/processor_set.o ../../../build/yield/thread/win32/reader_writer_lock.o ../../../build/yield/thread/win32/semaphore.o ../../../build/yield/thread/win32/system_configuration.o ../../../build/yield/thread/win32/thread.o
endif


all: ../../../lib/yield/libyield_thread.a

clean:
	$(RM) ../../../lib/yield/libyield_thread.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../lib/yield/libyield.a:
	$(MAKE) ../yield.Makefile


../../../lib/yield/libyield_thread.a: $(O_FILE_PATHS) ../../../lib/yield/libyield.a
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../build/yield/thread/condition_variable.o: ../../../src/yield/thread/condition_variable.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/condition_variable.o -MD $(CXXFLAGS) ../../../src/yield/thread/condition_variable.cpp

../../../build/yield/thread/darwin/semaphore.o: ../../../src/yield/thread/darwin/semaphore.cpp
	-mkdir -p ../../../build/yield/thread/darwin 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/darwin/semaphore.o -MD $(CXXFLAGS) ../../../src/yield/thread/darwin/semaphore.cpp

../../../build/yield/thread/darwin/system_configuration.o: ../../../src/yield/thread/darwin/system_configuration.cpp
	-mkdir -p ../../../build/yield/thread/darwin 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/darwin/system_configuration.o -MD $(CXXFLAGS) ../../../src/yield/thread/darwin/system_configuration.cpp

../../../build/yield/thread/darwin/thread.o: ../../../src/yield/thread/darwin/thread.cpp
	-mkdir -p ../../../build/yield/thread/darwin 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/darwin/thread.o -MD $(CXXFLAGS) ../../../src/yield/thread/darwin/thread.cpp

../../../build/yield/thread/fiber.o: ../../../src/yield/thread/fiber.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/fiber.o -MD $(CXXFLAGS) ../../../src/yield/thread/fiber.cpp

../../../build/yield/thread/lightweight_mutex.o: ../../../src/yield/thread/lightweight_mutex.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/lightweight_mutex.o -MD $(CXXFLAGS) ../../../src/yield/thread/lightweight_mutex.cpp

../../../build/yield/thread/linux/processor_set.o: ../../../src/yield/thread/linux/processor_set.cpp
	-mkdir -p ../../../build/yield/thread/linux 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/linux/processor_set.o -MD $(CXXFLAGS) ../../../src/yield/thread/linux/processor_set.cpp

../../../build/yield/thread/linux/system_configuration.o: ../../../src/yield/thread/linux/system_configuration.cpp
	-mkdir -p ../../../build/yield/thread/linux 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/linux/system_configuration.o -MD $(CXXFLAGS) ../../../src/yield/thread/linux/system_configuration.cpp

../../../build/yield/thread/linux/thread.o: ../../../src/yield/thread/linux/thread.cpp
	-mkdir -p ../../../build/yield/thread/linux 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/linux/thread.o -MD $(CXXFLAGS) ../../../src/yield/thread/linux/thread.cpp

../../../build/yield/thread/mutex.o: ../../../src/yield/thread/mutex.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/mutex.o -MD $(CXXFLAGS) ../../../src/yield/thread/mutex.cpp

../../../build/yield/thread/performance_counter_set.o: ../../../src/yield/thread/performance_counter_set.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/performance_counter_set.o -MD $(CXXFLAGS) ../../../src/yield/thread/performance_counter_set.cpp

../../../build/yield/thread/posix/condition_variable.o: ../../../src/yield/thread/posix/condition_variable.cpp
	-mkdir -p ../../../build/yield/thread/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/posix/condition_variable.o -MD $(CXXFLAGS) ../../../src/yield/thread/posix/condition_variable.cpp

../../../build/yield/thread/posix/mutex.o: ../../../src/yield/thread/posix/mutex.cpp
	-mkdir -p ../../../build/yield/thread/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/posix/mutex.o -MD $(CXXFLAGS) ../../../src/yield/thread/posix/mutex.cpp

../../../build/yield/thread/posix/pipe.o: ../../../src/yield/thread/posix/pipe.cpp
	-mkdir -p ../../../build/yield/thread/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/posix/pipe.o -MD $(CXXFLAGS) ../../../src/yield/thread/posix/pipe.cpp

../../../build/yield/thread/posix/reader_writer_lock.o: ../../../src/yield/thread/posix/reader_writer_lock.cpp
	-mkdir -p ../../../build/yield/thread/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/posix/reader_writer_lock.o -MD $(CXXFLAGS) ../../../src/yield/thread/posix/reader_writer_lock.cpp

../../../build/yield/thread/posix/semaphore.o: ../../../src/yield/thread/posix/semaphore.cpp
	-mkdir -p ../../../build/yield/thread/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/posix/semaphore.o -MD $(CXXFLAGS) ../../../src/yield/thread/posix/semaphore.cpp

../../../build/yield/thread/posix/thread.o: ../../../src/yield/thread/posix/thread.cpp
	-mkdir -p ../../../build/yield/thread/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/posix/thread.o -MD $(CXXFLAGS) ../../../src/yield/thread/posix/thread.cpp

../../../build/yield/thread/processor_set.o: ../../../src/yield/thread/processor_set.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/processor_set.o -MD $(CXXFLAGS) ../../../src/yield/thread/processor_set.cpp

../../../build/yield/thread/reader_writer_lock.o: ../../../src/yield/thread/reader_writer_lock.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/reader_writer_lock.o -MD $(CXXFLAGS) ../../../src/yield/thread/reader_writer_lock.cpp

../../../build/yield/thread/semaphore.o: ../../../src/yield/thread/semaphore.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/semaphore.o -MD $(CXXFLAGS) ../../../src/yield/thread/semaphore.cpp

../../../build/yield/thread/sunos/cpc_performance_counter_set.o: ../../../src/yield/thread/sunos/cpc_performance_counter_set.cpp
	-mkdir -p ../../../build/yield/thread/sunos 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/sunos/cpc_performance_counter_set.o -MD $(CXXFLAGS) ../../../src/yield/thread/sunos/cpc_performance_counter_set.cpp

../../../build/yield/thread/sunos/processor_set.o: ../../../src/yield/thread/sunos/processor_set.cpp
	-mkdir -p ../../../build/yield/thread/sunos 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/sunos/processor_set.o -MD $(CXXFLAGS) ../../../src/yield/thread/sunos/processor_set.cpp

../../../build/yield/thread/sunos/system_configuration.o: ../../../src/yield/thread/sunos/system_configuration.cpp
	-mkdir -p ../../../build/yield/thread/sunos 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/sunos/system_configuration.o -MD $(CXXFLAGS) ../../../src/yield/thread/sunos/system_configuration.cpp

../../../build/yield/thread/sunos/thread.o: ../../../src/yield/thread/sunos/thread.cpp
	-mkdir -p ../../../build/yield/thread/sunos 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/sunos/thread.o -MD $(CXXFLAGS) ../../../src/yield/thread/sunos/thread.cpp

../../../build/yield/thread/thread.o: ../../../src/yield/thread/thread.cpp
	-mkdir -p ../../../build/yield/thread 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/thread.o -MD $(CXXFLAGS) ../../../src/yield/thread/thread.cpp

../../../build/yield/thread/unix/fiber.o: ../../../src/yield/thread/unix/fiber.cpp
	-mkdir -p ../../../build/yield/thread/unix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/unix/fiber.o -MD $(CXXFLAGS) ../../../src/yield/thread/unix/fiber.cpp

../../../build/yield/thread/unix/papi_performance_counter_set.o: ../../../src/yield/thread/unix/papi_performance_counter_set.cpp
	-mkdir -p ../../../build/yield/thread/unix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/unix/papi_performance_counter_set.o -MD $(CXXFLAGS) ../../../src/yield/thread/unix/papi_performance_counter_set.cpp

../../../build/yield/thread/win32/condition_variable.o: ../../../src/yield/thread/win32/condition_variable.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/condition_variable.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/condition_variable.cpp

../../../build/yield/thread/win32/fiber.o: ../../../src/yield/thread/win32/fiber.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/fiber.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/fiber.cpp

../../../build/yield/thread/win32/lightweight_mutex.o: ../../../src/yield/thread/win32/lightweight_mutex.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/lightweight_mutex.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/lightweight_mutex.cpp

../../../build/yield/thread/win32/mutex.o: ../../../src/yield/thread/win32/mutex.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/mutex.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/mutex.cpp

../../../build/yield/thread/win32/pipe.o: ../../../src/yield/thread/win32/pipe.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/pipe.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/pipe.cpp

../../../build/yield/thread/win32/processor_set.o: ../../../src/yield/thread/win32/processor_set.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/processor_set.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/processor_set.cpp

../../../build/yield/thread/win32/reader_writer_lock.o: ../../../src/yield/thread/win32/reader_writer_lock.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/reader_writer_lock.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/reader_writer_lock.cpp

../../../build/yield/thread/win32/semaphore.o: ../../../src/yield/thread/win32/semaphore.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/semaphore.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/semaphore.cpp

../../../build/yield/thread/win32/system_configuration.o: ../../../src/yield/thread/win32/system_configuration.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/system_configuration.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/system_configuration.cpp

../../../build/yield/thread/win32/thread.o: ../../../src/yield/thread/win32/thread.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/thread.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/thread.cpp
