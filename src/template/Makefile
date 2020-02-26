#
# $Id: Makefile,v 1.2 2006/12/19 08:33:18 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
CC=gcc
CFLAGS=-O
INCDIR=../../include
LIBDIR=../../lib


all: libtemplate.so template.o example

install: libtemplate.so 
	cp template.h $(INCDIR)/
	chmod 644 $(INCDIR)/template.h

	cp libtemplate.so $(LIBDIR)/
	chmod 644 $(LIBDIR)/libtemplate.so

libtemplate.so: template.o
# 	$(CC) -shared template.o -o libtemplate.so
	$(CC) -shared -Wl,-s template.o -o libtemplate.so

template.o: template.c template.h
	$(CC) $(CFLAGS) -fPIC -I$(INCDIR) template.c -c -o template.o

example: libtemplate.so
	$(CC) $(CFLAGS) -L. -I. -L$(LIBDIR) -I$(INCDIR) test.c -o test -ltemplate -ldebug

clean:
	@echo Cleaning up
	rm -f *.o *.so test 
