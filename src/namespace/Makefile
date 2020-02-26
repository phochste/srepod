#
# $Id: Makefile,v 1.4 2006/12/19 08:33:16 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
CC=gcc
LDFLAGS=
LIBDIR=../../lib
INCDIR=../../include
BINDIR=../../bin
LIBS=-L$(LIBDIR) -lexpat -lhash 
CFLAGS=-I$(INCDIR) -O2 -DXSI_TYPE_HACK

all:	xmlapp

install: xmlapp 
	
	cp namespace $(BINDIR)
	chmod 711 $(BINDIR)/namespace

xmlapp: scan1.o scan2.o stack.o main.o 
	$(CC) $(LDFLAGS) -o namespace main.o scan1.o scan2.o stack.o $(LIBS)
#	strip namespace
clean:
	rm -f *.o core namespace
