all: yield yield.aio yield.aio_test yield.fs yield.fs.aio yield.fs.aio_test yield.fs_test yield.getopt yield.getopt_test yield.http yield.http.client yield.http.client_test yield.http.server yield.http.server_test yield.http_test yield.i18n yield.i18n_test yield.marshal yield.marshal.json yield.marshal.json_test yield.marshal.xdr yield.marshal.xdr_test yield.marshal.xml yield.marshal.xml_test yield.marshal_test yield.poll yield.poll_test yield.process yield.process_test yield.sockets yield.sockets.aio yield.sockets.aio_test yield.sockets.client yield.sockets.client_test yield.sockets.poll yield.sockets.poll_test yield.sockets.server yield.sockets.server_test yield.sockets.ssl yield.sockets.ssl_test yield.sockets_test yield.stage yield.stage_test yield.thread yield.thread_test yield.uri yield.uri_test yield.uuid yield.uuid_test yield_test

clean:
	$(MAKE) -C proj/yield -f yield.Makefile clean
	$(MAKE) -C proj/yield -f yield_test.Makefile clean
	$(MAKE) -C proj/yield/aio -f yield.aio.Makefile clean
	$(MAKE) -C proj/yield/aio -f yield.aio_test.Makefile clean
	$(MAKE) -C proj/yield/fs -f yield.fs.Makefile clean
	$(MAKE) -C proj/yield/fs -f yield.fs_test.Makefile clean
	$(MAKE) -C proj/yield/fs/aio -f yield.fs.aio.Makefile clean
	$(MAKE) -C proj/yield/fs/aio -f yield.fs.aio_test.Makefile clean
	$(MAKE) -C proj/yield/getopt -f yield.getopt.Makefile clean
	$(MAKE) -C proj/yield/getopt -f yield.getopt_test.Makefile clean
	$(MAKE) -C proj/yield/http -f yield.http.Makefile clean
	$(MAKE) -C proj/yield/http -f yield.http_test.Makefile clean
	$(MAKE) -C proj/yield/http/client -f yield.http.client.Makefile clean
	$(MAKE) -C proj/yield/http/client -f yield.http.client_test.Makefile clean
	$(MAKE) -C proj/yield/http/server -f yield.http.server.Makefile clean
	$(MAKE) -C proj/yield/http/server -f yield.http.server_test.Makefile clean
	$(MAKE) -C proj/yield/i18n -f yield.i18n.Makefile clean
	$(MAKE) -C proj/yield/i18n -f yield.i18n_test.Makefile clean
	$(MAKE) -C proj/yield/marshal -f yield.marshal.Makefile clean
	$(MAKE) -C proj/yield/marshal -f yield.marshal_test.Makefile clean
	$(MAKE) -C proj/yield/marshal/json -f yield.marshal.json.Makefile clean
	$(MAKE) -C proj/yield/marshal/json -f yield.marshal.json_test.Makefile clean
	$(MAKE) -C proj/yield/marshal/xdr -f yield.marshal.xdr.Makefile clean
	$(MAKE) -C proj/yield/marshal/xdr -f yield.marshal.xdr_test.Makefile clean
	$(MAKE) -C proj/yield/marshal/xml -f yield.marshal.xml.Makefile clean
	$(MAKE) -C proj/yield/marshal/xml -f yield.marshal.xml_test.Makefile clean
	$(MAKE) -C proj/yield/poll -f yield.poll.Makefile clean
	$(MAKE) -C proj/yield/poll -f yield.poll_test.Makefile clean
	$(MAKE) -C proj/yield/process -f yield.process.Makefile clean
	$(MAKE) -C proj/yield/process -f yield.process_test.Makefile clean
	$(MAKE) -C proj/yield/sockets -f yield.sockets.Makefile clean
	$(MAKE) -C proj/yield/sockets -f yield.sockets_test.Makefile clean
	$(MAKE) -C proj/yield/sockets/aio -f yield.sockets.aio.Makefile clean
	$(MAKE) -C proj/yield/sockets/aio -f yield.sockets.aio_test.Makefile clean
	$(MAKE) -C proj/yield/sockets/client -f yield.sockets.client.Makefile clean
	$(MAKE) -C proj/yield/sockets/client -f yield.sockets.client_test.Makefile clean
	$(MAKE) -C proj/yield/sockets/poll -f yield.sockets.poll.Makefile clean
	$(MAKE) -C proj/yield/sockets/poll -f yield.sockets.poll_test.Makefile clean
	$(MAKE) -C proj/yield/sockets/server -f yield.sockets.server.Makefile clean
	$(MAKE) -C proj/yield/sockets/server -f yield.sockets.server_test.Makefile clean
	$(MAKE) -C proj/yield/sockets/ssl -f yield.sockets.ssl.Makefile clean
	$(MAKE) -C proj/yield/sockets/ssl -f yield.sockets.ssl_test.Makefile clean
	$(MAKE) -C proj/yield/stage -f yield.stage.Makefile clean
	$(MAKE) -C proj/yield/stage -f yield.stage_test.Makefile clean
	$(MAKE) -C proj/yield/thread -f yield.thread.Makefile clean
	$(MAKE) -C proj/yield/thread -f yield.thread_test.Makefile clean
	$(MAKE) -C proj/yield/uri -f yield.uri.Makefile clean
	$(MAKE) -C proj/yield/uri -f yield.uri_test.Makefile clean
	$(MAKE) -C proj/yield/uuid -f yield.uuid.Makefile clean
	$(MAKE) -C proj/yield/uuid -f yield.uuid_test.Makefile clean

depclean:
	$(MAKE) -C proj/yield -f yield.Makefile depclean
	$(MAKE) -C proj/yield -f yield_test.Makefile depclean
	$(MAKE) -C proj/yield/aio -f yield.aio.Makefile depclean
	$(MAKE) -C proj/yield/aio -f yield.aio_test.Makefile depclean
	$(MAKE) -C proj/yield/fs -f yield.fs.Makefile depclean
	$(MAKE) -C proj/yield/fs -f yield.fs_test.Makefile depclean
	$(MAKE) -C proj/yield/fs/aio -f yield.fs.aio.Makefile depclean
	$(MAKE) -C proj/yield/fs/aio -f yield.fs.aio_test.Makefile depclean
	$(MAKE) -C proj/yield/getopt -f yield.getopt.Makefile depclean
	$(MAKE) -C proj/yield/getopt -f yield.getopt_test.Makefile depclean
	$(MAKE) -C proj/yield/http -f yield.http.Makefile depclean
	$(MAKE) -C proj/yield/http -f yield.http_test.Makefile depclean
	$(MAKE) -C proj/yield/http/client -f yield.http.client.Makefile depclean
	$(MAKE) -C proj/yield/http/client -f yield.http.client_test.Makefile depclean
	$(MAKE) -C proj/yield/http/server -f yield.http.server.Makefile depclean
	$(MAKE) -C proj/yield/http/server -f yield.http.server_test.Makefile depclean
	$(MAKE) -C proj/yield/i18n -f yield.i18n.Makefile depclean
	$(MAKE) -C proj/yield/i18n -f yield.i18n_test.Makefile depclean
	$(MAKE) -C proj/yield/marshal -f yield.marshal.Makefile depclean
	$(MAKE) -C proj/yield/marshal -f yield.marshal_test.Makefile depclean
	$(MAKE) -C proj/yield/marshal/json -f yield.marshal.json.Makefile depclean
	$(MAKE) -C proj/yield/marshal/json -f yield.marshal.json_test.Makefile depclean
	$(MAKE) -C proj/yield/marshal/xdr -f yield.marshal.xdr.Makefile depclean
	$(MAKE) -C proj/yield/marshal/xdr -f yield.marshal.xdr_test.Makefile depclean
	$(MAKE) -C proj/yield/marshal/xml -f yield.marshal.xml.Makefile depclean
	$(MAKE) -C proj/yield/marshal/xml -f yield.marshal.xml_test.Makefile depclean
	$(MAKE) -C proj/yield/poll -f yield.poll.Makefile depclean
	$(MAKE) -C proj/yield/poll -f yield.poll_test.Makefile depclean
	$(MAKE) -C proj/yield/process -f yield.process.Makefile depclean
	$(MAKE) -C proj/yield/process -f yield.process_test.Makefile depclean
	$(MAKE) -C proj/yield/sockets -f yield.sockets.Makefile depclean
	$(MAKE) -C proj/yield/sockets -f yield.sockets_test.Makefile depclean
	$(MAKE) -C proj/yield/sockets/aio -f yield.sockets.aio.Makefile depclean
	$(MAKE) -C proj/yield/sockets/aio -f yield.sockets.aio_test.Makefile depclean
	$(MAKE) -C proj/yield/sockets/client -f yield.sockets.client.Makefile depclean
	$(MAKE) -C proj/yield/sockets/client -f yield.sockets.client_test.Makefile depclean
	$(MAKE) -C proj/yield/sockets/poll -f yield.sockets.poll.Makefile depclean
	$(MAKE) -C proj/yield/sockets/poll -f yield.sockets.poll_test.Makefile depclean
	$(MAKE) -C proj/yield/sockets/server -f yield.sockets.server.Makefile depclean
	$(MAKE) -C proj/yield/sockets/server -f yield.sockets.server_test.Makefile depclean
	$(MAKE) -C proj/yield/sockets/ssl -f yield.sockets.ssl.Makefile depclean
	$(MAKE) -C proj/yield/sockets/ssl -f yield.sockets.ssl_test.Makefile depclean
	$(MAKE) -C proj/yield/stage -f yield.stage.Makefile depclean
	$(MAKE) -C proj/yield/stage -f yield.stage_test.Makefile depclean
	$(MAKE) -C proj/yield/thread -f yield.thread.Makefile depclean
	$(MAKE) -C proj/yield/thread -f yield.thread_test.Makefile depclean
	$(MAKE) -C proj/yield/uri -f yield.uri.Makefile depclean
	$(MAKE) -C proj/yield/uri -f yield.uri_test.Makefile depclean
	$(MAKE) -C proj/yield/uuid -f yield.uuid.Makefile depclean
	$(MAKE) -C proj/yield/uuid -f yield.uuid_test.Makefile depclean


yield.aio: yield
	$(MAKE) -C proj/yield/aio -f yield.aio.Makefile

yield.aio_test: yield.aio
	$(MAKE) -C proj/yield/aio -f yield.aio_test.Makefile

yield.fs.aio: yield.aio yield.fs yield.thread
	$(MAKE) -C proj/yield/fs/aio -f yield.fs.aio.Makefile

yield.fs.aio_test: yield.fs.aio
	$(MAKE) -C proj/yield/fs/aio -f yield.fs.aio_test.Makefile

yield.fs: yield.i18n
	$(MAKE) -C proj/yield/fs -f yield.fs.Makefile

yield.fs_test: yield.fs
	$(MAKE) -C proj/yield/fs -f yield.fs_test.Makefile

yield.getopt: yield
	$(MAKE) -C proj/yield/getopt -f yield.getopt.Makefile

yield.getopt_test: yield.getopt
	$(MAKE) -C proj/yield/getopt -f yield.getopt_test.Makefile

yield.http.client: yield.http yield.sockets.client
	$(MAKE) -C proj/yield/http/client -f yield.http.client.Makefile

yield.http.client_test: yield.http.client yield.http.server
	$(MAKE) -C proj/yield/http/client -f yield.http.client_test.Makefile

yield.http.server: yield.fs yield.http yield.sockets.server
	$(MAKE) -C proj/yield/http/server -f yield.http.server.Makefile

yield.http.server_test: yield.http.server
	$(MAKE) -C proj/yield/http/server -f yield.http.server_test.Makefile

yield.http: yield.uri
	$(MAKE) -C proj/yield/http -f yield.http.Makefile

yield.http_test: yield.http
	$(MAKE) -C proj/yield/http -f yield.http_test.Makefile

yield.i18n: yield
	$(MAKE) -C proj/yield/i18n -f yield.i18n.Makefile

yield.i18n_test: yield.i18n
	$(MAKE) -C proj/yield/i18n -f yield.i18n_test.Makefile

yield.marshal.json: yield.marshal
	$(MAKE) -C proj/yield/marshal/json -f yield.marshal.json.Makefile

yield.marshal.json_test: yield.marshal.json
	$(MAKE) -C proj/yield/marshal/json -f yield.marshal.json_test.Makefile

yield.marshal.xdr: yield.marshal
	$(MAKE) -C proj/yield/marshal/xdr -f yield.marshal.xdr.Makefile

yield.marshal.xdr_test: yield.marshal.xdr
	$(MAKE) -C proj/yield/marshal/xdr -f yield.marshal.xdr_test.Makefile

yield.marshal.xml: yield.marshal
	$(MAKE) -C proj/yield/marshal/xml -f yield.marshal.xml.Makefile

yield.marshal.xml_test: yield.marshal.xml
	$(MAKE) -C proj/yield/marshal/xml -f yield.marshal.xml_test.Makefile

yield.marshal: yield
	$(MAKE) -C proj/yield/marshal -f yield.marshal.Makefile

yield.marshal_test: yield.marshal
	$(MAKE) -C proj/yield/marshal -f yield.marshal_test.Makefile

yield.poll: yield
	$(MAKE) -C proj/yield/poll -f yield.poll.Makefile

yield.poll_test: yield.poll
	$(MAKE) -C proj/yield/poll -f yield.poll_test.Makefile

yield.process: yield.fs
	$(MAKE) -C proj/yield/process -f yield.process.Makefile

yield.process_test: yield.process
	$(MAKE) -C proj/yield/process -f yield.process_test.Makefile

yield.sockets.aio: yield.aio yield.sockets.poll
	$(MAKE) -C proj/yield/sockets/aio -f yield.sockets.aio.Makefile

yield.sockets.aio_test: yield.sockets.aio
	$(MAKE) -C proj/yield/sockets/aio -f yield.sockets.aio_test.Makefile

yield.sockets.client: yield.sockets.aio yield.stage
	$(MAKE) -C proj/yield/sockets/client -f yield.sockets.client.Makefile

yield.sockets.client_test: yield.sockets.client
	$(MAKE) -C proj/yield/sockets/client -f yield.sockets.client_test.Makefile

yield.sockets.poll: yield.poll yield.sockets
	$(MAKE) -C proj/yield/sockets/poll -f yield.sockets.poll.Makefile

yield.sockets.poll_test: yield.sockets.poll
	$(MAKE) -C proj/yield/sockets/poll -f yield.sockets.poll_test.Makefile

yield.sockets.server: yield.sockets.aio yield.stage
	$(MAKE) -C proj/yield/sockets/server -f yield.sockets.server.Makefile

yield.sockets.server_test: yield.sockets.server
	$(MAKE) -C proj/yield/sockets/server -f yield.sockets.server_test.Makefile

yield.sockets.ssl: yield.sockets
	$(MAKE) -C proj/yield/sockets/ssl -f yield.sockets.ssl.Makefile

yield.sockets.ssl_test: yield.sockets.ssl
	$(MAKE) -C proj/yield/sockets/ssl -f yield.sockets.ssl_test.Makefile

yield.sockets: yield
	$(MAKE) -C proj/yield/sockets -f yield.sockets.Makefile

yield.sockets_test: yield.sockets
	$(MAKE) -C proj/yield/sockets -f yield.sockets_test.Makefile

yield.stage: yield.thread
	$(MAKE) -C proj/yield/stage -f yield.stage.Makefile

yield.stage_test: yield.stage
	$(MAKE) -C proj/yield/stage -f yield.stage_test.Makefile

yield.thread: yield
	$(MAKE) -C proj/yield/thread -f yield.thread.Makefile

yield.thread_test: yield.thread
	$(MAKE) -C proj/yield/thread -f yield.thread_test.Makefile

yield.uri: yield
	$(MAKE) -C proj/yield/uri -f yield.uri.Makefile

yield.uri_test: yield.uri
	$(MAKE) -C proj/yield/uri -f yield.uri_test.Makefile

yield.uuid: yield
	$(MAKE) -C proj/yield/uuid -f yield.uuid.Makefile

yield.uuid_test: yield.uuid
	$(MAKE) -C proj/yield/uuid -f yield.uuid_test.Makefile

yield: 
	$(MAKE) -C proj/yield -f yield.Makefile

yield_test: yield
	$(MAKE) -C proj/yield -f yield_test.Makefile

