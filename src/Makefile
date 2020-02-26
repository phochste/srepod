#
# $Id: Makefile,v 1.13 2006/12/19 08:33:11 hochstenbach Exp $
#
# Author(s): Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
#
PROG=gateway
CC=gcc
CFLAGS=-O -I include -I lib
INSTDIR=/opt/srepod-2-1-6
INCDIR=$(INSTDIR)/include
LIBDIR=$(INSTDIR)/lib
BINDIR=$(INSTDIR)/bin
SBINDIR=$(INSTDIR)/sbin
SPOOLDIR=$(INSTDIR)/spool
HTMLDIR=$(INSTDIR)/htdocs
DBDIR=$(INSTDIR)/db
CGIDIR=$(INSTDIR)/cgi-bin
#LD_SEARCH_FLAGS=-Xlinker -rpath $(LIBDIR)

all: libgdbm libexpat libcurl libcgi libtf libdebug libcconfig libsession libtemplate liboai server adm reg val namesp trans dbinst prog 

gw: libcgi libtf libdebug libcconfig libsession libtemplate liboai server adm reg val trans dbinst prog 


libgdbm:
	@echo Making gdbm
	cd gdbm-1.8.3; \
	./configure --prefix=$(INSTDIR); \
	make install

libexpat:
	@echo Making expat
	cd expat-1.95.6; \
	./configure --prefix=$(INSTDIR); \
	make install

libcurl:
	@echo Making curl
	cd curl-7.9.8; \
	./configure --prefix=$(INSTDIR) --disable-debug  --disable-ipv6  --disable-telnet --disable-dict  --disable-ldap   --disable-file  --disable-gopher --disable-ftp  --without-ssl; \
	make install

libcgi:
	@echo Making libcgi-util
	make -C cgi-util-2.1.7 install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR)

libtf:
	@echo Making libtf
	make -C time install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR)

libcconfig:
	@echo Making libcconfig
	make -C cconfig install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR)

libdebug:
	@echo Making libdebug
	make -C debug install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR)

liboai: 		
	@echo Making liboai-gdbm
	make -C oai-gdbm install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR)

libsession:
	@echo Making libsession
	make -C session install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR) SBINDIR=$(SBINDIR)

libtemplate:
	@echo Making libtemplate
	make -C template install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR)

server:
	@echo Making daemon
	make -C daemon install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR) BINDIR=$(BINDIR)

reg:
	@echo Making registry
	make -C registry install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR) CGIDIR=$(CGIDIR) BINDIR=$(BINDIR) SBINDIR=$(SBINDIR)

adm:
	@echo Making adm
	make -C admin install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR) SBINDIR=$(SBINDIR)

trans:
	@echo Making transform_c
	make -C transform_c install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR) BINDIR=$(BINDIR)

namesp:
	@echo Making namespace 
	make -C namespace install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR) BINDIR=$(BINDIR)

val:
	@echo Making validate 
	make -C validate install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR) BINDIR=$(BINDIR)

dbinst:
	@echo Making dbinstall
	make -C dbinstall install INCDIR=$(INCDIR) LIBDIR=$(LIBDIR) BINDIR=$(BINDIR)

prog: gateway.o 
	$(CC) -o $(PROG) gateway.o $(LD_SEARCH_FLAGS) -L$(LIBDIR) -loai-gdbm -ltf -lgdbm -lcgi-util -lcconfig -ldebug -lcurl -lsession -ltemplate -lhash
	strip $(PROG)

gateway.o: gateway.c gateway.h
	$(CC) $(CFLAGS) -I$(INCDIR) gateway.c -c -o gateway.o 

install:
	cp $(PROG) $(CGIDIR)
	cp $(PROG).cgi $(CGIDIR)
	cp $(PROG).conf $(CGIDIR)
	cp friends.conf $(CGIDIR)
	cp desc.conf $(CGIDIR)
	cp README* $(CGIDIR)
	cp environ $(CGIDIR)

	cp ../LICENSE  $(INSTDIR)
	cp ../chmod.sh $(INSTDIR)
	cp ../INSTALL  $(INSTDIR)

	if [ ! -d "$(SPOOLDIR)" ]; then mkdir $(SPOOLDIR); fi
	if [ ! -d "$(HTMLDIR)" ]; then mkdir $(HTMLDIR); fi
	if [ ! -d "$(DBDIR)" ]; then mkdir $(DBDIR); fi

	cp ../htdocs/*ml $(HTMLDIR)
	cp ../htdocs/*gif $(HTMLDIR)
	cp ../htdocs/*png $(HTMLDIR)
	cp ../htdocs/*jpg $(HTMLDIR)

	chmod 711 $(CGIDIR)/$(PROG)
	chmod 644 $(CGIDIR)/$(PROG).conf
	chmod 644 $(CGIDIR)/friends.conf
	chmod 644 $(CGIDIR)/desc.conf
	chmod 644 $(CGIDIR)/README*
	chmod 755 $(CGIDIR)/$(PROG).cgi

clean:
	if [ -f "gdbm-1.8.3/Makefile" ]; then make -C gdbm-1.8.3 distclean; fi
	if [ -f "expat-1.95.6/Makefile" ]; then make -C expat-1.95.6 distclean; fi
	if [ -f "curl-7.9.8/Makefile" ]; then make -C curl-7.9.8 distclean; fi
	make -C oai-gdbm clean
	make -C cgi-util-2.1.7 clean
	make -C time clean
	make -C cconfig clean
	make -C debug clean
	make -C daemon clean
	make -C registry clean
	make -C transform_c clean
	make -C namespace clean
	make -C validate clean
	make -C admin clean
	make -C dbinstall clean
	make -C session clean
	make -C template clean
	rm -f *.o $(PROG)
