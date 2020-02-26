#
# $Id: Makefile,v 1.2 2006/12/19 08:33:15 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
CC=gcc
CFLAGS=-O
INCDIR=../../include
LIBDIR=../../lib


all: libdebug.so debug.o

install: libdebug.so 
	cp debug.h $(INCDIR)/
	chmod 644 $(INCDIR)/debug.h

	cp libdebug.so $(LIBDIR)/
	chmod 644 $(LIBDIR)/libdebug.so

libdebug.so: debug.o
	$(CC) -shared debug.o -o libdebug.so
# 	$(CC) -shared -Wl,-s oai-gdbm.o -o liboai-gdbm.so

debug.o: debug.c debug.h
	$(CC) $(CFLAGS) -fPIC -I$(INCDIR) debug.c -c -o debug.o

clean:
	@echo Cleaning up
	rm -f *.o *.so
