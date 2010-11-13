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
	LIBS += -liconv -lnsl -lsocket -luuid -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_http -lyield_aio -lyield_fs -lyield_i18n -lyield_poll -lyield_net -lyield_stage -lyield_thread -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/http_test -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/http_test/http_client_test.o ../../../build/yield/http_test/http_request_parser_test.o ../../../build/yield/http_test/http_response_parser_test.o ../../../build/yield/http_test/http_server_test.o ../../../build/yield/http_test/yield_http_test_main.o


../../../bin/yield_http_test: $(OBJECT_FILE_PATHS)
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../bin/yield_http_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/http_test/http_client_test.o: ../../../src/yield/http/http_client_test.cpp
	$(CXX) -c -o ../../../build/yield/http_test/http_client_test.o -MD $(CXXFLAGS) ../../../src/yield/http/http_client_test.cpp

../../../build/yield/http_test/http_request_parser_test.o: ../../../src/yield/http/http_request_parser_test.cpp
	$(CXX) -c -o ../../../build/yield/http_test/http_request_parser_test.o -MD $(CXXFLAGS) ../../../src/yield/http/http_request_parser_test.cpp

../../../build/yield/http_test/http_response_parser_test.o: ../../../src/yield/http/http_response_parser_test.cpp
	$(CXX) -c -o ../../../build/yield/http_test/http_response_parser_test.o -MD $(CXXFLAGS) ../../../src/yield/http/http_response_parser_test.cpp

../../../build/yield/http_test/http_server_test.o: ../../../src/yield/http/http_server_test.cpp
	$(CXX) -c -o ../../../build/yield/http_test/http_server_test.o -MD $(CXXFLAGS) ../../../src/yield/http/http_server_test.cpp

../../../build/yield/http_test/yield_http_test_main.o: ../../../src/yield/http/yield_http_test_main.cpp
	$(CXX) -c -o ../../../build/yield/http_test/yield_http_test_main.o -MD $(CXXFLAGS) ../../../src/yield/http/yield_http_test_main.cpp


