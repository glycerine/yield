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


LDFLAGS += -L../../../../lib/yield
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lnsl -lsocket -lm -lrt -lstdc++
endif
LIBS += -lyield_sockets_aio -lyield_aio -lyield_sockets_poll -lyield_poll -lyield_sockets -lyield


DEP_FILE_PATHS := $(shell find ../../../../build/yield/sockets/aio_test -name "*.d")


OBJECT_FILE_PATHS += ../../../../build/yield/sockets/aio_test/nbio_queue_test.o ../../../../build/yield/sockets/aio_test/yield_sockets_aio_test_main.o
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../../build/yield/sockets/aio_test/win32/aio_queue_test.o
endif


../../../../bin/yield/yield_sockets_aio_test: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../../bin/yield/yield_sockets_aio_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../../build/yield/sockets/aio_test/nbio_queue_test.o: ../../../../test/yield/sockets/aio/nbio_queue_test.cpp
	-mkdir -p ../../../../build/yield/sockets/aio_test 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio_test/nbio_queue_test.o -MD $(CXXFLAGS) ../../../../test/yield/sockets/aio/nbio_queue_test.cpp

../../../../build/yield/sockets/aio_test/win32/aio_queue_test.o: ../../../../test/yield/sockets/aio/win32/aio_queue_test.cpp
	-mkdir -p ../../../../build/yield/sockets/aio_test/win32 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio_test/win32/aio_queue_test.o -MD $(CXXFLAGS) ../../../../test/yield/sockets/aio/win32/aio_queue_test.cpp

../../../../build/yield/sockets/aio_test/yield_sockets_aio_test_main.o: ../../../../test/yield/sockets/aio/yield_sockets_aio_test_main.cpp
	-mkdir -p ../../../../build/yield/sockets/aio_test 2>/dev/null
	$(CXX) -c -o ../../../../build/yield/sockets/aio_test/yield_sockets_aio_test_main.o -MD $(CXXFLAGS) ../../../../test/yield/sockets/aio/yield_sockets_aio_test_main.cpp


