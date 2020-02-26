#
# $Id: Makefile,v 1.2 2006/12/19 08:33:14 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
CC=gcc
CCFLAGS=-O 
LIBDIR=../../lib
INCDIR=../../include
BINDIR=../../bin


all: dbinstall dblock dbupdate

install: dbinstall dblock dbupdate
	cp dbupdate $(BINDIR)/
	cp dbinstall $(BINDIR)/
	cp dblock $(BINDIR)/
	chmod 711 $(BINDIR)/dbupdate
	chmod 711 $(BINDIR)/dbinstall
	chmod 711 $(BINDIR)/dblock
	cd $(BINDIR) ; ln -s -f dblock dbunlock

dbupdate: dbupdate.c
	$(CC) $(CFLAGS) -I$(INCDIR) -L$(LIBDIR) dbupdate.c -o dbupdate -lgdbm

dbinstall: dbinstall.c
	$(CC) $(CFLAGS) -I$(INCDIR) -L$(LIBDIR) dbinstall.c -o dbinstall -lgdbm

dblock: dblock.c
	$(CC) $(CFLAGS) -I$(INCDIR) -L$(LIBDIR) dblock.c -o dblock -lgdbm

clean:
	@echo Cleaning up
	rm -f *.o *.so dbinstall dblock dbunlock dbupdate
