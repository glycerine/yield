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


LDFLAGS += -L../../../lib
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++
endif
LIBS += -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/common_test -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/common_test/atomic_test.o ../../../build/yield/common_test/auto_object_test.o ../../../build/yield/common_test/date_time_test.o ../../../build/yield/common_test/event_test.o ../../../build/yield/common_test/exception_test.o ../../../build/yield/common_test/fixed_buffer_test.o ../../../build/yield/common_test/log_test.o ../../../build/yield/common_test/message_test.o ../../../build/yield/common_test/object_test.o ../../../build/yield/common_test/page_test.o ../../../build/yield/common_test/request_test.o ../../../build/yield/common_test/response_test.o ../../../build/yield/common_test/string_buffer_test.o ../../../build/yield/common_test/time_test.o ../../../build/yield/common_test/yield_common_test_main.o


../../../bin/yield_common_test: $(OBJECT_FILE_PATHS)
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../bin/yield_common_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/common_test/atomic_test.o: ../../../src/yield/atomic_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/atomic_test.o -MD $(CXXFLAGS) ../../../src/yield/atomic_test.cpp

../../../build/yield/common_test/auto_object_test.o: ../../../src/yield/auto_object_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/auto_object_test.o -MD $(CXXFLAGS) ../../../src/yield/auto_object_test.cpp

../../../build/yield/common_test/date_time_test.o: ../../../src/yield/date_time_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/date_time_test.o -MD $(CXXFLAGS) ../../../src/yield/date_time_test.cpp

../../../build/yield/common_test/event_test.o: ../../../src/yield/event_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/event_test.o -MD $(CXXFLAGS) ../../../src/yield/event_test.cpp

../../../build/yield/common_test/exception_test.o: ../../../src/yield/exception_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/exception_test.o -MD $(CXXFLAGS) ../../../src/yield/exception_test.cpp

../../../build/yield/common_test/fixed_buffer_test.o: ../../../src/yield/fixed_buffer_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/fixed_buffer_test.o -MD $(CXXFLAGS) ../../../src/yield/fixed_buffer_test.cpp

../../../build/yield/common_test/log_test.o: ../../../src/yield/log_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/log_test.o -MD $(CXXFLAGS) ../../../src/yield/log_test.cpp

../../../build/yield/common_test/message_test.o: ../../../src/yield/message_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/message_test.o -MD $(CXXFLAGS) ../../../src/yield/message_test.cpp

../../../build/yield/common_test/object_test.o: ../../../src/yield/object_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/object_test.o -MD $(CXXFLAGS) ../../../src/yield/object_test.cpp

../../../build/yield/common_test/page_test.o: ../../../src/yield/page_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/page_test.o -MD $(CXXFLAGS) ../../../src/yield/page_test.cpp

../../../build/yield/common_test/request_test.o: ../../../src/yield/request_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/request_test.o -MD $(CXXFLAGS) ../../../src/yield/request_test.cpp

../../../build/yield/common_test/response_test.o: ../../../src/yield/response_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/response_test.o -MD $(CXXFLAGS) ../../../src/yield/response_test.cpp

../../../build/yield/common_test/string_buffer_test.o: ../../../src/yield/string_buffer_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/string_buffer_test.o -MD $(CXXFLAGS) ../../../src/yield/string_buffer_test.cpp

../../../build/yield/common_test/time_test.o: ../../../src/yield/time_test.cpp
	$(CXX) -c -o ../../../build/yield/common_test/time_test.o -MD $(CXXFLAGS) ../../../src/yield/time_test.cpp

../../../build/yield/common_test/yield_common_test_main.o: ../../../src/yield/yield_common_test_main.cpp
	$(CXX) -c -o ../../../build/yield/common_test/yield_common_test_main.o -MD $(CXXFLAGS) ../../../src/yield/yield_common_test_main.cpp


