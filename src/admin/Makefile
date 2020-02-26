#
# $Id: Makefile,v 1.3 2006/12/19 08:33:11 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
CC=gcc
CFLAGS=-O
INCDIR=../../include
LIBDIR=../../lib
SBINDIR=../../sbin

all: dbexport dbimport

install: dbexport dbimport
	if [ ! -d "$(SBINDIR)" ]; then mkdir $(SBINDIR); fi

	cp dbexport $(SBINDIR)
	cp dbimport $(SBINDIR)

dbexport: gdbm_export.c 
	$(CC) $(CFLAGS) -L$(LIBDIR) -I$(INCDIR) -o dbexport gdbm_export.c -lgdbm

dbimport: gdbm_import.c 
	$(CC) $(CFLAGS) -L$(LIBDIR) -I$(INCDIR) -o dbimport gdbm_import.c -lgdbm
	
clean:
	@echo Cleaning up
	rm -f *.o dbexport dbimport
