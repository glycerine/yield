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


LDFLAGS += -L../../../lib/yield
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++
endif
LIBS += -lyield_poll -lyield


DEP_FILE_PATHS := $(shell find ../../../build/yield/poll_test -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/poll_test/fd_event_queue_test.o ../../../build/yield/poll_test/yield_poll_test_main.o


../../../bin/yield/yield_poll_test: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../bin/yield/yield_poll_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/poll_test/fd_event_queue_test.o: ../../../test/yield/poll/fd_event_queue_test.cpp
	-mkdir -p ../../../build/yield/poll_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll_test/fd_event_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/poll/fd_event_queue_test.cpp

../../../build/yield/poll_test/yield_poll_test_main.o: ../../../test/yield/poll/yield_poll_test_main.cpp
	-mkdir -p ../../../build/yield/poll_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/poll_test/yield_poll_test_main.o -MD $(CXXFLAGS) ../../../test/yield/poll/yield_poll_test_main.cpp


