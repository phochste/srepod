#
# $Id: Makefile,v 1.2 2006/12/19 08:33:12 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
CC=gcc
CFLAGS=-O 
LIBDIR=../../lib
INCDIR=../../include

all: libhash.so libcconfig.so cconfig.o

install: libhash.so libcconfig.so 
	cp hash.h $(INCDIR)/
	chmod 644 $(INCDIR)/hash.h

	cp cconfig.h $(INCDIR)/
	chmod 644 $(INCDIR)/cconfig.h

	cp libhash.so $(LIBDIR)/
	chmod 644 $(LIBDIR)/libhash.so

	cp libcconfig.so $(LIBDIR)/
	chmod 644 $(LIBDIR)/libcconfig.so

libhash.so: hash.o 
#	$(CC) $(CFLAGS) -shared hash.o -o libhash.so
	$(CC) -shared -Wl,-s hash.o -o libhash.so

hash.o: hash.c
	$(CC) $(CFLAGS) -fPIC -I$(INCDIR) -I. hash.c -c -o hash.o

hash.c: hash.h

libcconfig.so: cconfig.o 
#	$(CC) $(CFLAGS) -L. -L$(LIBDIR) -shared cconfig.o -o libcconfig.so -lhash
	$(CC) -shared -L. -L$(LIBDIR) -Wl,-s cconfig.o -o libcconfig.so -lhash

cconfig.o: cconfig.c 
	$(CC) $(CFLAGS) -fPIC -I$(INCDIR) -I. cconfig.c -c -o cconfig.o

cconfig.c: cconfig.h

test:
	$(CC) $(CFLAGS) -L. -L$(LIBDIR) -I$(INCDIR) -o cconf_test test.c -lcconfig -ldebug

clean:
	@echo Cleaning up
	rm -f *.o *.so cconf_test hash_test
