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
	LIBS += -laio -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lnsl -lsocket -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_aio -lyield_fs -lyield_i18n -lyield_poll -lyield_net -lyield_stage -lyield_thread -lyield_uri -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/http -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/http/access_log.o ../../../build/yield/http/file_access_log.o ../../../build/yield/http/http_client.o ../../../build/yield/http/http_message.o ../../../build/yield/http/http_message_parser.o ../../../build/yield/http/http_request.o ../../../build/yield/http/http_request_handler.o ../../../build/yield/http/http_request_parser.o ../../../build/yield/http/http_response.o ../../../build/yield/http/http_response_parser.o ../../../build/yield/http/http_server.o ../../../build/yield/http/socket_client.o ../../../build/yield/http/socket_peer.o ../../../build/yield/http/socket_server.o ../../../build/yield/http/stream_socket_client.o ../../../build/yield/http/stream_socket_peer.o ../../../build/yield/http/stream_socket_server.o


../../../lib/yield/libyield_http.a: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../lib/yield 2>/dev/null
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/yield/libyield_http.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/http/access_log.o: ../../../src/yield/http/access_log.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/access_log.o -MD $(CXXFLAGS) ../../../src/yield/http/access_log.cpp

../../../build/yield/http/file_access_log.o: ../../../src/yield/http/file_access_log.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/file_access_log.o -MD $(CXXFLAGS) ../../../src/yield/http/file_access_log.cpp

../../../build/yield/http/http_client.o: ../../../src/yield/http/http_client.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_client.o -MD $(CXXFLAGS) ../../../src/yield/http/http_client.cpp

../../../build/yield/http/http_message.o: ../../../src/yield/http/http_message.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_message.o -MD $(CXXFLAGS) ../../../src/yield/http/http_message.cpp

../../../build/yield/http/http_message_parser.o: ../../../src/yield/http/http_message_parser.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_message_parser.o -MD $(CXXFLAGS) ../../../src/yield/http/http_message_parser.cpp

../../../build/yield/http/http_request.o: ../../../src/yield/http/http_request.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_request.o -MD $(CXXFLAGS) ../../../src/yield/http/http_request.cpp

../../../build/yield/http/http_request_handler.o: ../../../src/yield/http/http_request_handler.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_request_handler.o -MD $(CXXFLAGS) ../../../src/yield/http/http_request_handler.cpp

../../../build/yield/http/http_request_parser.o: ../../../src/yield/http/http_request_parser.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_request_parser.o -MD $(CXXFLAGS) ../../../src/yield/http/http_request_parser.cpp

../../../build/yield/http/http_response.o: ../../../src/yield/http/http_response.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_response.o -MD $(CXXFLAGS) ../../../src/yield/http/http_response.cpp

../../../build/yield/http/http_response_parser.o: ../../../src/yield/http/http_response_parser.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_response_parser.o -MD $(CXXFLAGS) ../../../src/yield/http/http_response_parser.cpp

../../../build/yield/http/http_server.o: ../../../src/yield/http/http_server.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/http_server.o -MD $(CXXFLAGS) ../../../src/yield/http/http_server.cpp

../../../build/yield/http/socket_client.o: ../../../src/yield/http/socket_client.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/socket_client.o -MD $(CXXFLAGS) ../../../src/yield/http/socket_client.cpp

../../../build/yield/http/socket_peer.o: ../../../src/yield/http/socket_peer.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/socket_peer.o -MD $(CXXFLAGS) ../../../src/yield/http/socket_peer.cpp

../../../build/yield/http/socket_server.o: ../../../src/yield/http/socket_server.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/socket_server.o -MD $(CXXFLAGS) ../../../src/yield/http/socket_server.cpp

../../../build/yield/http/stream_socket_client.o: ../../../src/yield/http/stream_socket_client.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/stream_socket_client.o -MD $(CXXFLAGS) ../../../src/yield/http/stream_socket_client.cpp

../../../build/yield/http/stream_socket_peer.o: ../../../src/yield/http/stream_socket_peer.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/stream_socket_peer.o -MD $(CXXFLAGS) ../../../src/yield/http/stream_socket_peer.cpp

../../../build/yield/http/stream_socket_server.o: ../../../src/yield/http/stream_socket_server.cpp
	-mkdir -p ../../../build/yield/http 2>/dev/null
	$(CXX) -c -o ../../../build/yield/http/stream_socket_server.o -MD $(CXXFLAGS) ../../../src/yield/http/stream_socket_server.cpp


