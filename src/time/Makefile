#
# $Id: Makefile,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
CC=gcc
CCFLAGS=-O2 
LIBDIR=../../lib
INCDIR=../../include


all: libtf.so timefunc.o

install: libtf.so 
	cp timefunc.h $(INCDIR)/
	chmod 644 $(INCDIR)/timefunc.h

	cp libtf.so $(LIBDIR)/
	chmod 644 $(LIBDIR)/libtf.so

libtf.so: timefunc.o
	$(CC) -shared -Wl,-s timefunc.o -o libtf.so

timefunc.o: timefunc.c timefunc.h
	$(CC) $(CFLAGS) -fPIC -I../include timefunc.c -c -o timefunc.o

clean:
	@echo Cleaning up
	rm -f *.o *.so
