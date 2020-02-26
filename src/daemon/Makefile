#
# $Id: Makefile,v 1.2 2006/12/19 08:33:14 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
CC=gcc
CFLAGS=-O 
INCDIR=../../include
LIBDIR=../../lib
BINDIR=../../bin
#LD_SEARCH_FLAGS=-Xlinker -rpath $(LIBDIR)

all: srepod get

install: srepod get
	cp daemonctl $(BINDIR)
	cp *.sh $(BINDIR)
	cp srepod $(BINDIR)
	cp srepod.conf $(BINDIR)
	cp get	$(BINDIR)
	cp README* $(BINDIR)
	chmod 755 $(BINDIR)/daemonctl
	chmod 755 $(BINDIR)/*.sh
	chmod 711 $(BINDIR)/get
	

srepod: child.o main.o
	$(CC) $(LD_SEARCH_FLAGS) -L$(LIBDIR) child.o main.o -o srepod -ldebug -lsession -lcconfig -lhash
	strip srepod

main.o: main.c defs.h
	$(CC) $(CFLAGS) -I$(INCDIR) main.c -c -o main.o

child.o: child.c defs.h
	$(CC) $(CFLAGS) -I$(INCDIR) child.c -c -o child.o

get: get.c
	$(CC) $(CFLAGS) -L$(LIBDIR) -I$(INCDIR) -o get get.c -lcurl
clean:
	@echo Cleaning up
	rm -f *.o srepod get
