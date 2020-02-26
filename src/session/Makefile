#
# $Id: Makefile,v 1.3 2006/12/19 08:33:18 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
CC=gcc
CFLAGS=-O 
LIBDIR=../../lib
INCDIR=../../include
SBINDIR=../../sbin

all: libsession.so session.o md5.o crtsession tstsession setsession

install: libsession.so crtsession tstsession setsession
	cp *.h $(INCDIR)/
	chmod 644 $(INCDIR)/*.h

	cp *.so $(LIBDIR)/
	chmod 644 $(LIBDIR)/*.so

	if [ ! -d "$(SBINDIR)" ]; then mkdir $(SBINDIR); fi

	cp crtsession $(SBINDIR)/
	chmod 711 $(SBINDIR)/crtsession

	cp setsession $(SBINDIR)/
	chmod 711 $(SBINDIR)/setsession

crtsession: libsession.so session_create.o 
	$(CC) -L. -L$(LIBDIR)  -o crtsession session_create.o  -lsession -ldebug -lhash

session_create.o: session_create.c
	$(CC) $(CFLAGS) -I$(INCDIR) session_create.c -c -o session_create.o

setsession: libsession.so session_set.o 
	$(CC) -L. -L$(LIBDIR)  -o setsession session_set.o  -lsession -ldebug -lhash

session_set.o: session_set.c
	$(CC) $(CFLAGS) -I$(INCDIR) session_set.c -c -o session_set.o

tstsession: libsession.so session_test.o
	$(CC) -g -L. -L$(LIBDIR) -o tstsession session_test.o  -lsession -ldebug -lhash

session_test.o: session_test.c
	$(CC) -g $(CFLAGS) -I$(INCDIR) session_test.c -c -o session_test.o

libsession.so: session.o md5.o
#	$(CC) -shared session.o md5.o -o libsession.so
	$(CC) -shared -nostartfiles -Wl,-s session.o md5.o -o libsession.so 

session.o: session.c session.h
	$(CC) $(CFLAGS) -fPIC -I$(INCDIR) session.c -c -o session.o 

md5.o: md5.c md5.h
	$(CC) $(CFLAGS) -fPIC -I$(INCDIR) md5.c -c -o md5.o

clean:
	@echo Cleaning up
	rm -f *.o *.so crtsession tstsession setsession
