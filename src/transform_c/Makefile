#
# $Id: Makefile,v 1.3 2005/07/26 08:39:59 hochstenbach Exp $
#
#---------------------------------------------------#
#           Generic Makefile                        #
#        GNU Compiler Settings by Henry N Jerez     #
#---------------------------------------------------#
CC=gcc
CFLAGS=-O2
LIBDIR=../../lib
INCDIR=../../include
BINDIR=../../bin

all: transform 

install: transform
	if [ ! -d "$(BINDIR)" ]; then mkdir $(BINDIR); fi

	cp transform $(BINDIR)
	chmod 711 $(BINDIR)/transform

transform: stackli.o parser.o
	$(CC) $(CFLAGS) -L$(LIBDIR) -o transform -lexpat -lgdbm stackli.o parser.o
	strip transform

parser.o: stackli.h parser.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c parser.c

stackli.o: stackli.h stackli.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c stackli.c

clean:
	rm -f *.o
	rm -f transform
	rm -f core*
