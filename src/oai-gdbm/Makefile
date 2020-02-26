#
# $Id: Makefile,v 1.2 2006/12/19 08:33:16 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
CC=gcc
CFLAGS=-O 
INCDIR=../../include
LIBDIR=../../lib


all: liboai-gdbm.so http_head.o oai-gdbm.o http_head_debug.o http_test

install: liboai-gdbm.so 
	cp oai-interface.h $(INCDIR)/
	cp oai-gdbm.h $(INCDIR)/
	chmod 644 $(INCDIR)/oai-gdbm.h

	cp liboai-gdbm.so $(LIBDIR)/
	chmod 644 $(LIBDIR)/liboai-gdbm.so

liboai-gdbm.so: oai-gdbm.o http_head.o
#	$(CC) -shared http_head.o oai-gdbm.o -o liboai-gdbm.so
	$(CC) -shared -Wl,-s oai-gdbm.o http_head.o -o liboai-gdbm.so

http_head.o: http_head.c http_head.h
	$(CC) $(CFLAGS) -fPIC -I$(INCDIR) http_head.c -c -o http_head.o

http_head_debug.o: http_head.c http_head.h
	$(CC) $(CFLAGS)-DCURLDEBUG -I$(INCDIR) http_head.c -c -o http_head_debug.o


oai-gdbm.o: oai-gdbm.c oai-gdbm.h oai-interface.h
	$(CC) $(CFLAGS) -fPIC -I. -I$(INCDIR) oai-gdbm.c -c -o oai-gdbm.o

http_test: http_test.o http_head.o
	$(CC) $(CFLAGS) -L$(LIBDIR) -o http_test http_head_debug.o http_test.o -lcurl -ldebug

http_test.o: http_test.c
	$(CC) $(CFLAGS) -I$(INCDIR) http_test.c -c -o http_test.o

clean:
	@echo Cleaning up
	rm -f *.o *.so http_test
