# SHELL = /bin/bash
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
	LIBS += -laio -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_fs_aio -lyield_aio -lyield_fs -lyield_i18n -lyield_thread -lyield


DEP_FILE_PATHS := $(shell find ../../../../build/yield/fs/aio_test -name "*.d")


OBJECT_FILE_PATHS += ../../../../build/yield/fs/aio_test/bio_queue_test.o ../../../../build/yield/fs/aio_test/yield_fs_aio_test_main.o
ifeq ($(UNAME), Darwin)
	OBJECT_FILE_PATHS += ../../../../build/yield/fs/aio_test/posix/aio_queue_test.o
endif
ifeq ($(UNAME), FreeBSD)
	OBJECT_FILE_PATHS += ../../../../build/yield/fs/aio_test/posix/aio_queue_test.o
endif
ifeq ($(UNAME), Linux)
	OBJECT_FILE_PATHS += ../../../../build/yield/fs/aio_test/posix/aio_queue_test.o
endif
ifeq ($(UNAME), Solaris)
	OBJECT_FILE_PATHS += ../../../../build/yield/fs/aio_test/posix/aio_queue_test.o
endif
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../../build/yield/fs/aio_test/win32/aio_queue_test.o
endif


../../../../bin/yield/yield_fs_aio_test: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../../bin/yield/yield_fs_aio_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../../build/yield/fs/aio_test/bio_queue_test.o: ../../../../test/yield/fs/aio/bio_queue_test.cpp
	-mkdir -p ../../../../build/yield/fs/aio_test 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio_test/bio_queue_test.o -MD $(CXXFLAGS) ../../../../test/yield/fs/aio/bio_queue_test.cpp

../../../../build/yield/fs/aio_test/posix/aio_queue_test.o: ../../../../test/yield/fs/aio/posix/aio_queue_test.cpp
	-mkdir -p ../../../../build/yield/fs/aio_test/posix 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio_test/posix/aio_queue_test.o -MD $(CXXFLAGS) ../../../../test/yield/fs/aio/posix/aio_queue_test.cpp

../../../../build/yield/fs/aio_test/win32/aio_queue_test.o: ../../../../test/yield/fs/aio/win32/aio_queue_test.cpp
	-mkdir -p ../../../../build/yield/fs/aio_test/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio_test/win32/aio_queue_test.o -MD $(CXXFLAGS) ../../../../test/yield/fs/aio/win32/aio_queue_test.cpp

../../../../build/yield/fs/aio_test/yield_fs_aio_test_main.o: ../../../../test/yield/fs/aio/yield_fs_aio_test_main.cpp
	-mkdir -p ../../../../build/yield/fs/aio_test 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/fs/aio_test/yield_fs_aio_test_main.o -MD $(CXXFLAGS) ../../../../test/yield/fs/aio/yield_fs_aio_test_main.cpp


