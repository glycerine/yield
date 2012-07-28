TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
UNAME := $(shell uname)


CXXFLAGS += -I../../../include
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



LDFLAGS += -L../../../lib/yield
ifeq ($(UNAME), FreeBSD)
	LDFLAGS += -L/usr/local/lib
endif
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


LIBS += -lyield
ifeq ($(UNAME), FreeBSD)
	LIBS += -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lkstat -lm -lrt -lstdc++
endif


D_FILE_PATHS := $(shell find ../../../build/yield/thread -name "*.d")


ifeq ($(UNAME), Darwin)
	O_FILE_PATHS += ../../../build/yield/thread/darwin/processor_set.o ../../../build/yield/thread/darwin/semaphore.o ../../../build/yield/thread/darwin/thread.o
	O_FILE_PATHS += ../../../build/yield/thread/posix/condition_variable.o ../../../build/yield/thread/posix/mutex.o ../../../build/yield/thread/posix/reader_writer_lock.o ../../../build/yield/thread/posix/semaphore.o ../../../build/yield/thread/posix/thread.o
endif
ifeq ($(UNAME), FreeBSD)
	O_FILE_PATHS += ../../../build/yield/thread/posix/condition_variable.o ../../../build/yield/thread/posix/mutex.o ../../../build/yield/thread/posix/reader_writer_lock.o ../../../build/yield/thread/posix/semaphore.o ../../../build/yield/thread/posix/thread.o
endif
ifeq ($(UNAME), Linux)
	O_FILE_PATHS += ../../../build/yield/thread/linux/processor_set.o ../../../build/yield/thread/linux/thread.o
	O_FILE_PATHS += ../../../build/yield/thread/posix/condition_variable.o ../../../build/yield/thread/posix/mutex.o ../../../build/yield/thread/posix/reader_writer_lock.o ../../../build/yield/thread/posix/semaphore.o ../../../build/yield/thread/posix/thread.o
endif
ifeq ($(UNAME), Solaris)
	O_FILE_PATHS += ../../../build/yield/thread/posix/condition_variable.o ../../../build/yield/thread/posix/mutex.o ../../../build/yield/thread/posix/reader_writer_lock.o ../../../build/yield/thread/posix/semaphore.o ../../../build/yield/thread/posix/thread.o
	O_FILE_PATHS += ../../../build/yield/thread/sunos/processor_set.o ../../../build/yield/thread/sunos/thread.o
endif
ifeq ($(UNAME), MINGW32)
	O_FILE_PATHS += ../../../build/yield/thread/win32/condition_variable.o ../../../build/yield/thread/win32/lightweight_mutex.o ../../../build/yield/thread/win32/mutex.o ../../../build/yield/thread/win32/processor_set.o ../../../build/yield/thread/win32/reader_writer_lock.o ../../../build/yield/thread/win32/semaphore.o ../../../build/yield/thread/win32/thread.o
endif


all: ../../../lib/yield/libyield_thread.a

clean:
	$(RM) ../../../lib/yield/libyield_thread.a $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)


../../../lib/yield/libyield.a:
	$(MAKE) -C .. yield.Makefile


../../../lib/yield/libyield_thread.a: $(O_FILE_PATHS) ../../../lib/yield/libyield.a
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(O_FILE_PATHS)

../../../build/yield/thread/darwin/processor_set.o: ../../../src/yield/thread/darwin/processor_set.cpp
	-mkdir -p ../../../build/yield/thread/darwin 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/darwin/processor_set.o -MD $(CXXFLAGS) ../../../src/yield/thread/darwin/processor_set.cpp

../../../build/yield/thread/darwin/semaphore.o: ../../../src/yield/thread/darwin/semaphore.cpp
	-mkdir -p ../../../build/yield/thread/darwin 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/darwin/semaphore.o -MD $(CXXFLAGS) ../../../src/yield/thread/darwin/semaphore.cpp

../../../build/yield/thread/darwin/thread.o: ../../../src/yield/thread/darwin/thread.cpp
	-mkdir -p ../../../build/yield/thread/darwin 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/darwin/thread.o -MD $(CXXFLAGS) ../../../src/yield/thread/darwin/thread.cpp

../../../build/yield/thread/linux/processor_set.o: ../../../src/yield/thread/linux/processor_set.cpp
	-mkdir -p ../../../build/yield/thread/linux 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/linux/processor_set.o -MD $(CXXFLAGS) ../../../src/yield/thread/linux/processor_set.cpp

../../../build/yield/thread/linux/thread.o: ../../../src/yield/thread/linux/thread.cpp
	-mkdir -p ../../../build/yield/thread/linux 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/linux/thread.o -MD $(CXXFLAGS) ../../../src/yield/thread/linux/thread.cpp

../../../build/yield/thread/posix/condition_variable.o: ../../../src/yield/thread/posix/condition_variable.cpp
	-mkdir -p ../../../build/yield/thread/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/posix/condition_variable.o -MD $(CXXFLAGS) ../../../src/yield/thread/posix/condition_variable.cpp

../../../build/yield/thread/posix/mutex.o: ../../../src/yield/thread/posix/mutex.cpp
	-mkdir -p ../../../build/yield/thread/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/posix/mutex.o -MD $(CXXFLAGS) ../../../src/yield/thread/posix/mutex.cpp

../../../build/yield/thread/posix/reader_writer_lock.o: ../../../src/yield/thread/posix/reader_writer_lock.cpp
	-mkdir -p ../../../build/yield/thread/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/posix/reader_writer_lock.o -MD $(CXXFLAGS) ../../../src/yield/thread/posix/reader_writer_lock.cpp

../../../build/yield/thread/posix/semaphore.o: ../../../src/yield/thread/posix/semaphore.cpp
	-mkdir -p ../../../build/yield/thread/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/posix/semaphore.o -MD $(CXXFLAGS) ../../../src/yield/thread/posix/semaphore.cpp

../../../build/yield/thread/posix/thread.o: ../../../src/yield/thread/posix/thread.cpp
	-mkdir -p ../../../build/yield/thread/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/posix/thread.o -MD $(CXXFLAGS) ../../../src/yield/thread/posix/thread.cpp

../../../build/yield/thread/sunos/processor_set.o: ../../../src/yield/thread/sunos/processor_set.cpp
	-mkdir -p ../../../build/yield/thread/sunos 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/sunos/processor_set.o -MD $(CXXFLAGS) ../../../src/yield/thread/sunos/processor_set.cpp

../../../build/yield/thread/sunos/thread.o: ../../../src/yield/thread/sunos/thread.cpp
	-mkdir -p ../../../build/yield/thread/sunos 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/sunos/thread.o -MD $(CXXFLAGS) ../../../src/yield/thread/sunos/thread.cpp

../../../build/yield/thread/win32/condition_variable.o: ../../../src/yield/thread/win32/condition_variable.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/condition_variable.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/condition_variable.cpp

../../../build/yield/thread/win32/lightweight_mutex.o: ../../../src/yield/thread/win32/lightweight_mutex.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/lightweight_mutex.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/lightweight_mutex.cpp

../../../build/yield/thread/win32/mutex.o: ../../../src/yield/thread/win32/mutex.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/mutex.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/mutex.cpp

../../../build/yield/thread/win32/processor_set.o: ../../../src/yield/thread/win32/processor_set.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/processor_set.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/processor_set.cpp

../../../build/yield/thread/win32/reader_writer_lock.o: ../../../src/yield/thread/win32/reader_writer_lock.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/reader_writer_lock.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/reader_writer_lock.cpp

../../../build/yield/thread/win32/semaphore.o: ../../../src/yield/thread/win32/semaphore.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/semaphore.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/semaphore.cpp

../../../build/yield/thread/win32/thread.o: ../../../src/yield/thread/win32/thread.cpp
	-mkdir -p ../../../build/yield/thread/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread/win32/thread.o -MD $(CXXFLAGS) ../../../src/yield/thread/win32/thread.cpp
