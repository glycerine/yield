# SHELL = /bin/bash
UNAME := $(shell uname)

CXXFLAGS += -I../../include
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


LDFLAGS += -L../../lib/yield
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


DEP_FILE_PATHS := $(shell find ../../build/yield_test -name "*.d")


OBJECT_FILE_PATHS += ../../build/yield_test/atomic_test.o ../../build/yield_test/auto_object_test.o ../../build/yield_test/date_time_test.o ../../build/yield_test/event_test.o ../../build/yield_test/exception_test.o ../../build/yield_test/fixed_buffer_test.o ../../build/yield_test/log_test.o ../../build/yield_test/message_test.o ../../build/yield_test/object_test.o ../../build/yield_test/page_test.o ../../build/yield_test/request_test.o ../../build/yield_test/response_test.o ../../build/yield_test/string_buffer_test.o ../../build/yield_test/time_test.o ../../build/yield_test/yield_test_main.o


../../bin/yield/yield_test: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../bin/yield 2>/dev/null
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../bin/yield/yield_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../build/yield_test/atomic_test.o: ../../test/yield/atomic_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/atomic_test.o -MD $(CXXFLAGS) ../../test/yield/atomic_test.cpp

../../build/yield_test/auto_object_test.o: ../../test/yield/auto_object_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/auto_object_test.o -MD $(CXXFLAGS) ../../test/yield/auto_object_test.cpp

../../build/yield_test/date_time_test.o: ../../test/yield/date_time_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/date_time_test.o -MD $(CXXFLAGS) ../../test/yield/date_time_test.cpp

../../build/yield_test/event_test.o: ../../test/yield/event_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/event_test.o -MD $(CXXFLAGS) ../../test/yield/event_test.cpp

../../build/yield_test/exception_test.o: ../../test/yield/exception_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/exception_test.o -MD $(CXXFLAGS) ../../test/yield/exception_test.cpp

../../build/yield_test/fixed_buffer_test.o: ../../test/yield/fixed_buffer_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/fixed_buffer_test.o -MD $(CXXFLAGS) ../../test/yield/fixed_buffer_test.cpp

../../build/yield_test/log_test.o: ../../test/yield/log_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/log_test.o -MD $(CXXFLAGS) ../../test/yield/log_test.cpp

../../build/yield_test/message_test.o: ../../test/yield/message_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/message_test.o -MD $(CXXFLAGS) ../../test/yield/message_test.cpp

../../build/yield_test/object_test.o: ../../test/yield/object_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/object_test.o -MD $(CXXFLAGS) ../../test/yield/object_test.cpp

../../build/yield_test/page_test.o: ../../test/yield/page_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/page_test.o -MD $(CXXFLAGS) ../../test/yield/page_test.cpp

../../build/yield_test/request_test.o: ../../test/yield/request_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/request_test.o -MD $(CXXFLAGS) ../../test/yield/request_test.cpp

../../build/yield_test/response_test.o: ../../test/yield/response_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/response_test.o -MD $(CXXFLAGS) ../../test/yield/response_test.cpp

../../build/yield_test/string_buffer_test.o: ../../test/yield/string_buffer_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/string_buffer_test.o -MD $(CXXFLAGS) ../../test/yield/string_buffer_test.cpp

../../build/yield_test/time_test.o: ../../test/yield/time_test.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/time_test.o -MD $(CXXFLAGS) ../../test/yield/time_test.cpp

../../build/yield_test/yield_test_main.o: ../../test/yield/yield_test_main.cpp
	-mkdir -p ../../build/yield_test 2>/dev/null
	$(CXX) -c -o ../../build/yield_test/yield_test_main.o -MD $(CXXFLAGS) ../../test/yield/yield_test_main.cpp


