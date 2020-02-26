#
# $Id: Makefile,v 1.6 2006/12/19 08:33:17 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
CC=gcc
CFLAGS=-O
INCDIR=../../include
LIBDIR=../../lib
BINDIR=../../bin
SBINDIR=../../sbin
CGIDIR=../../cgi-bin
#LD_SEARCH_FLAGS=-Xlinker -rpath $(LIBDIR)

all: registry unregister terminator

install: registry unregister terminator
	if [ ! -d "$(CGIDIR)" ]; then mkdir $(CGIDIR); fi

	cp registry $(CGIDIR)
	cp registry.cgi $(CGIDIR)
	cp unregister $(SBINDIR)
	cp unregister.sh $(BINDIR)
	cp terminator $(CGIDIR)
	cp terminator.cgi $(CGIDIR)
	chmod 711 $(CGIDIR)/registry
	chmod 755 $(CGIDIR)/registry.cgi
	chmod 755 $(BINDIR)/unregister.sh
	chmod 711 $(SBINDIR)/unregister
	chmod 711 $(CGIDIR)/terminator
	chmod 755 $(CGIDIR)/terminator.cgi

registry: registry.o md5.o
	$(CC) $(LD_SEARCH_FLAGS) -L$(LIBDIR) registry.o md5.o -o registry -ldebug -lsession -lcconfig -lcgi-util -ltemplate -lhash
	strip registry

registry.o: registry.c 
	$(CC) $(CFLAGS) -I$(INCDIR) registry.c -c -o registry.o

unregister: unregister.o
	$(CC) $(LD_SEARCH_FLAGS) -L$(LIBDIR) unregister.o -o unregister -lcconfig -ldebug -lhash
	strip unregister

unregister.o: unregister.c
	$(CC) $(CFLAGS) -I$(INCDIR) unregister.c -c -o unregister.o

md5.o: md5.c md5.h
	$(CC) $(CFLAGS) -I$(INCDIR) md5.c -c -o md5.o

terminator: terminator.o md5.o
	$(CC) $(LD_SEARCH_FLAGS) -L$(LIBDIR) terminator.o md5.o -o terminator -ldebug -lsession -lcconfig -lcgi-util -ltemplate -lhash
	strip terminator

terminator.o: terminator.c
	$(CC) $(CFLAGS) -I$(INCDIR) terminator.c -c -o terminator.o

clean:
	@echo Cleaning up
	rm -f *.o registry unregister terminator 
