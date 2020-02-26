#
# $Id: Makefile,v 1.7 2006/12/19 08:33:19 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
CC=gcc
CFLAGS=-O -I$(INCDIR) -L$(LIBDIR)
INCDIR=../../include
LIBDIR=../../lib
BINDIR=../../bin

all: hstrip hasout validurl 

install: hstrip hasout validurl
	cp hasout $(BINDIR)/
	chmod 711 $(BINDIR)/hasout

	cp hstrip $(BINDIR)/
	chmod 711 $(BINDIR)/hstrip

	cp validurl $(BINDIR)/
	chmod 711 $(BINDIR)/validurl

hstrip: hstrip.c 
	$(CC) $(CFLAGS) hstrip.c -o hstrip

hasout: hasout.c 
	$(CC) $(CFLAGS) hasout.c -o hasout

validurl: validurl.o
	$(CC) $(CFLAGS) -L$(LIBDIR) -I$(INCDIR) validurl.o -o validurl -lexpat

clean:
	@echo Cleaning up
	rm -f *.o *.so hstrip hasout validurl
