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


LDFLAGS += -L../../../lib/yield
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
	LIBS += -lpthread -lrt -lstdc++ -laio
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lnsl -lsocket -luuid -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_aio -lyield_fs -lyield_i18n -lyield_poll -lyield_net -lyield_stage -lyield_thread -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/aio_test -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/aio_test/fs/bio_queue_test.o ../../../build/yield/aio_test/net/sockets/bio_queue_test.o ../../../build/yield/aio_test/net/sockets/nbio_queue_test.o ../../../build/yield/aio_test/yield_aio_test_main.o
ifeq ($(UNAME), Darwin)
	OBJECT_FILE_PATHS += ../../../build/yield/aio_test/fs/posix/aio_queue_test.o
endif
ifeq ($(UNAME), FreeBSD)
	OBJECT_FILE_PATHS += ../../../build/yield/aio_test/fs/posix/aio_queue_test.o
endif
ifeq ($(UNAME), Linux)
	OBJECT_FILE_PATHS += ../../../build/yield/aio_test/fs/posix/aio_queue_test.o
endif
ifeq ($(UNAME), Solaris)
	OBJECT_FILE_PATHS += ../../../build/yield/aio_test/fs/posix/aio_queue_test.o
endif
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../build/yield/aio_test/fs/win32/aio_queue_test.o ../../../build/yield/aio_test/net/sockets/win32/aio_queue_test.o
endif


../../../bin/yield/yield_aio_test: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../bin/yield/yield_aio_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/aio_test/fs/bio_queue_test.o: ../../../src/yield/aio/fs/bio_queue_test.cpp
	-mkdir -p ../../../build/yield/aio_test/fs 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio_test/fs/bio_queue_test.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/bio_queue_test.cpp

../../../build/yield/aio_test/fs/posix/aio_queue_test.o: ../../../src/yield/aio/fs/posix/aio_queue_test.cpp
	-mkdir -p ../../../build/yield/aio_test/fs/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio_test/fs/posix/aio_queue_test.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/posix/aio_queue_test.cpp

../../../build/yield/aio_test/fs/win32/aio_queue_test.o: ../../../src/yield/aio/fs/win32/aio_queue_test.cpp
	-mkdir -p ../../../build/yield/aio_test/fs/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio_test/fs/win32/aio_queue_test.o -MD $(CXXFLAGS) ../../../src/yield/aio/fs/win32/aio_queue_test.cpp

../../../build/yield/aio_test/net/sockets/bio_queue_test.o: ../../../src/yield/aio/net/sockets/bio_queue_test.cpp
	-mkdir -p ../../../build/yield/aio_test/net/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio_test/net/sockets/bio_queue_test.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/bio_queue_test.cpp

../../../build/yield/aio_test/net/sockets/nbio_queue_test.o: ../../../src/yield/aio/net/sockets/nbio_queue_test.cpp
	-mkdir -p ../../../build/yield/aio_test/net/sockets 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio_test/net/sockets/nbio_queue_test.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/nbio_queue_test.cpp

../../../build/yield/aio_test/net/sockets/win32/aio_queue_test.o: ../../../src/yield/aio/net/sockets/win32/aio_queue_test.cpp
	-mkdir -p ../../../build/yield/aio_test/net/sockets/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio_test/net/sockets/win32/aio_queue_test.o -MD $(CXXFLAGS) ../../../src/yield/aio/net/sockets/win32/aio_queue_test.cpp

../../../build/yield/aio_test/yield_aio_test_main.o: ../../../src/yield/aio/yield_aio_test_main.cpp
	-mkdir -p ../../../build/yield/aio_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/aio_test/yield_aio_test_main.o -MD $(CXXFLAGS) ../../../src/yield/aio/yield_aio_test_main.cpp


