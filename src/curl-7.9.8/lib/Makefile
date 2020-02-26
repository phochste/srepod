# Makefile.in generated automatically by automake 1.5 from Makefile.am.

# Copyright 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001
# Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.



#
# $Id: Makefile,v 1.12 2005/06/28 11:14:25 hochstenbach Exp $
#

SHELL = /bin/sh

srcdir = .
top_srcdir = ..

prefix = /opt/srepod-2-1-4
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include
pkgdatadir = $(datadir)/curl
pkglibdir = $(libdir)/curl
pkgincludedir = $(includedir)/curl
top_builddir = ..

ACLOCAL = ${SHELL} /home/hochsten/srepod/src/curl-7.9.8/missing --run aclocal
AUTOCONF = ${SHELL} /home/hochsten/srepod/src/curl-7.9.8/missing --run autoconf
AUTOMAKE = ${SHELL} /home/hochsten/srepod/src/curl-7.9.8/missing --run automake
AUTOHEADER = ${SHELL} /home/hochsten/srepod/src/curl-7.9.8/missing --run autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}
INSTALL_HEADER = $(INSTALL_DATA)
transform = s,x,x,
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = 
host_triplet = i686-pc-linux-gnu
AMTAR = ${SHELL} /home/hochsten/srepod/src/curl-7.9.8/missing --run tar
AS = as
AWK = gawk
CC = gcc
CURL_DISABLE_DICT = 
CURL_DISABLE_FILE = 
CURL_DISABLE_FTP = 
CURL_DISABLE_GOPHER = 
CURL_DISABLE_HTTP = 
CURL_DISABLE_LDAP = 
CURL_DISABLE_TELNET = 
DEPDIR = .deps
DLLTOOL = dlltool
ECHO = echo
EXEEXT = 
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
IPV6_ENABLED = 
KRB4_ENABLED = 
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
NROFF = /usr/bin/gnroff
OBJDUMP = objdump
OBJEXT = o
OPENSSL_ENABLED = 
PACKAGE = curl
PERL = /usr/bin/perl
PKGADD_NAME = cURL - a client that groks URLs
PKGADD_PKG = HAXXcurl
PKGADD_VENDOR = curl.haxx.se
RANDOM_FILE = /dev/urandom
RANLIB = ranlib
STRIP = strip
VERSION = 7.9.8
VERSIONNUM = 070908
YACC = bison -y
am__include = include
am__quote = 
install_sh = /home/hochsten/srepod/src/curl-7.9.8/install-sh

AUTOMAKE_OPTIONS = foreign nostdinc

EXTRA_DIST = getdate.y Makefile.b32 Makefile.b32.resp Makefile.m32	    \
       Makefile.vc6 Makefile.riscos libcurl.def dllinit.c curllib.dsp	    \
       curllib.dsw config-vms.h config-win32.h config-riscos.h config-mac.h \
       config.h.in


lib_LTLIBRARIES = libcurl.la

# we use srcdir/include for the static global include files
# we use builddir/lib for the generated lib/config.h file to get found
# we use srcdir/lib for the lib-private header files
INCLUDES = -I$(top_srcdir)/include -I$(top_builddir)/lib -I$(top_srcdir)/lib

libcurl_la_LDFLAGS = -no-undefined -version-info 2:2:0

# This flag accepts an argument of the form current[:revision[:age]]. So,
# passing -version-info 3:12:1 sets current to 3, revision to 12, and age to
# 1.
#
# If either revision or age are omitted, they default to 0. Also note that age
# must be less than or equal to the current interface number.
#
# Here are a set of rules to help you update your library version information: 
#
# 1.Start with version information of 0:0:0 for each libtool library.
#
# 2.Update the version information only immediately before a public release of
# your software. More frequent updates are unnecessary, and only guarantee
# that the current interface number gets larger faster.
#
# 3.If the library source code has changed at all since the last update, then
# increment revision (c:r:a becomes c:r+1:a).
#
# 4.If any interfaces have been added, removed, or changed since the last
# update, increment current, and set revision to 0.
#
# 5.If any interfaces have been added since the last public release, then
# increment age.
#
# 6.If any interfaces have been removed since the last public release, then
# set age to 0.
#
libcurl_la_SOURCES = \
arpa_telnet.h  file.c         getpass.h      netrc.h        timeval.c \
base64.c       file.h         hostip.c       progress.c     timeval.h \
base64.h       formdata.c     hostip.h       progress.h     \
cookie.c       formdata.h     http.c         sendf.c        \
cookie.h       ftp.c          http.h         sendf.h        url.c \
dict.c         ftp.h          if2ip.c        speedcheck.c   url.h \
dict.h         getdate.c      if2ip.h        speedcheck.h   urldata.h \
getdate.h      ldap.c         ssluse.c       version.c \
getenv.c       ldap.h         ssluse.h       \
escape.c       mprintf.c      telnet.c       \
escape.h       getpass.c      netrc.c        telnet.h       \
getinfo.c getinfo.h transfer.c strequal.c strequal.h easy.c \
security.h security.c krb4.c krb4.h memdebug.c memdebug.h inet_ntoa_r.h \
http_chunks.c http_chunks.h strtok.c strtok.h connect.c connect.h \
llist.c llist.h hash.c hash.h multi.c


noinst_HEADERS = setup.h transfer.h
subdir = lib
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = config.h $(top_builddir)/src/config.h \
	$(top_builddir)/tests/server/config.h
CONFIG_CLEAN_FILES =
LTLIBRARIES = $(lib_LTLIBRARIES)

libcurl_la_LIBADD =
am_libcurl_la_OBJECTS = file.lo timeval.lo base64.lo hostip.lo \
	progress.lo formdata.lo cookie.lo http.lo sendf.lo ftp.lo \
	url.lo dict.lo if2ip.lo speedcheck.lo getdate.lo ldap.lo \
	ssluse.lo version.lo getenv.lo escape.lo mprintf.lo telnet.lo \
	getpass.lo netrc.lo getinfo.lo transfer.lo strequal.lo easy.lo \
	security.lo krb4.lo memdebug.lo http_chunks.lo strtok.lo \
	connect.lo llist.lo hash.lo multi.lo
libcurl_la_OBJECTS = $(am_libcurl_la_OBJECTS)

DEFS = -DHAVE_CONFIG_H
DEFAULT_INCLUDES = 
CPPFLAGS = 
LDFLAGS = 
LIBS = -ldl 
depcomp = $(SHELL) $(top_srcdir)/depcomp
DEP_FILES = $(DEPDIR)/base64.Plo $(DEPDIR)/connect.Plo \
	$(DEPDIR)/cookie.Plo $(DEPDIR)/dict.Plo \
	$(DEPDIR)/easy.Plo $(DEPDIR)/escape.Plo \
	$(DEPDIR)/file.Plo $(DEPDIR)/formdata.Plo \
	$(DEPDIR)/ftp.Plo $(DEPDIR)/getdate.Plo \
	$(DEPDIR)/getenv.Plo $(DEPDIR)/getinfo.Plo \
	$(DEPDIR)/getpass.Plo $(DEPDIR)/hash.Plo \
	$(DEPDIR)/hostip.Plo $(DEPDIR)/http.Plo \
	$(DEPDIR)/http_chunks.Plo $(DEPDIR)/if2ip.Plo \
	$(DEPDIR)/krb4.Plo $(DEPDIR)/ldap.Plo \
	$(DEPDIR)/llist.Plo $(DEPDIR)/memdebug.Plo \
	$(DEPDIR)/mprintf.Plo $(DEPDIR)/multi.Plo \
	$(DEPDIR)/netrc.Plo $(DEPDIR)/progress.Plo \
	$(DEPDIR)/security.Plo $(DEPDIR)/sendf.Plo \
	$(DEPDIR)/speedcheck.Plo $(DEPDIR)/ssluse.Plo \
	$(DEPDIR)/strequal.Plo $(DEPDIR)/strtok.Plo \
	$(DEPDIR)/telnet.Plo $(DEPDIR)/timeval.Plo \
	$(DEPDIR)/transfer.Plo $(DEPDIR)/url.Plo \
	$(DEPDIR)/version.Plo
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) \
	$(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) \
	$(AM_LDFLAGS) $(LDFLAGS) -o $@
CFLAGS = -g -O2
DIST_SOURCES = $(libcurl_la_SOURCES)
HEADERS = $(noinst_HEADERS)

DIST_COMMON = $(noinst_HEADERS) ./stamp-h1.in Makefile.am Makefile.in \
	config.h.in
SOURCES = $(libcurl_la_SOURCES)

all: config.h
	$(MAKE) $(AM_MAKEFLAGS) all-am

.SUFFIXES:
.SUFFIXES: .c .lo .o .obj

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:
	-rm -f libtool
$(srcdir)/Makefile.in:  Makefile.am  $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && \
	  $(AUTOMAKE) --foreign  lib/Makefile
Makefile:  $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) && \
	  CONFIG_HEADERS= CONFIG_LINKS= \
	  CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status
config.h: stamp-h1
	@if test ! -f $@; then \
		rm -f stamp-h1; \
		$(MAKE) stamp-h1; \
	else :; fi
stamp-h1: $(srcdir)/config.h.in $(top_builddir)/config.status
	@rm -f stamp-h1 stamp-h1T
	@echo timestamp > stamp-h1T 2> /dev/null
	cd $(top_builddir) \
	  && CONFIG_FILES= CONFIG_HEADERS=lib/config.h \
	     $(SHELL) ./config.status
	@mv stamp-h1T stamp-h1
$(srcdir)/config.h.in:  $(srcdir)/./stamp-h1.in
	@if test ! -f $@; then \
		rm -f $(srcdir)/./stamp-h1.in; \
		$(MAKE) $(srcdir)/./stamp-h1.in; \
	else :; fi
$(srcdir)/./stamp-h1.in: $(top_srcdir)/configure.in $(ACLOCAL_M4) $(top_srcdir)/acconfig.h
	@rm -f $(srcdir)/./stamp-h1.in $(srcdir)/./stamp-h1.inT
	@echo timestamp > $(srcdir)/./stamp-h1.inT 2> /dev/null
	cd $(top_srcdir) && $(AUTOHEADER)
	@mv $(srcdir)/./stamp-h1.inT $(srcdir)/./stamp-h1.in

distclean-hdr:
	-rm -f config.h
install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(LIBTOOL) --mode=install $(INSTALL) $(INSTALL_STRIP_FLAG) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(LIBTOOL) --mode=install $(INSTALL) $(INSTALL_STRIP_FLAG) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  echo " $(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p"; \
	  $(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p; \
	done

clean-libLTLIBRARIES:
	-test -z "$(lib_LTLIBRARIES)" || rm -f $(lib_LTLIBRARIES)
libcurl.la: $(libcurl_la_OBJECTS) $(libcurl_la_DEPENDENCIES) 
	$(LINK) -rpath $(libdir) $(libcurl_la_LDFLAGS) $(libcurl_la_OBJECTS) $(libcurl_la_LIBADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT) core *.core

distclean-compile:
	-rm -f *.tab.c

include $(DEPDIR)/base64.Plo
include $(DEPDIR)/connect.Plo
include $(DEPDIR)/cookie.Plo
include $(DEPDIR)/dict.Plo
include $(DEPDIR)/easy.Plo
include $(DEPDIR)/escape.Plo
include $(DEPDIR)/file.Plo
include $(DEPDIR)/formdata.Plo
include $(DEPDIR)/ftp.Plo
include $(DEPDIR)/getdate.Plo
include $(DEPDIR)/getenv.Plo
include $(DEPDIR)/getinfo.Plo
include $(DEPDIR)/getpass.Plo
include $(DEPDIR)/hash.Plo
include $(DEPDIR)/hostip.Plo
include $(DEPDIR)/http.Plo
include $(DEPDIR)/http_chunks.Plo
include $(DEPDIR)/if2ip.Plo
include $(DEPDIR)/krb4.Plo
include $(DEPDIR)/ldap.Plo
include $(DEPDIR)/llist.Plo
include $(DEPDIR)/memdebug.Plo
include $(DEPDIR)/mprintf.Plo
include $(DEPDIR)/multi.Plo
include $(DEPDIR)/netrc.Plo
include $(DEPDIR)/progress.Plo
include $(DEPDIR)/security.Plo
include $(DEPDIR)/sendf.Plo
include $(DEPDIR)/speedcheck.Plo
include $(DEPDIR)/ssluse.Plo
include $(DEPDIR)/strequal.Plo
include $(DEPDIR)/strtok.Plo
include $(DEPDIR)/telnet.Plo
include $(DEPDIR)/timeval.Plo
include $(DEPDIR)/transfer.Plo
include $(DEPDIR)/url.Plo
include $(DEPDIR)/version.Plo

distclean-depend:
	-rm -rf $(DEPDIR)

.c.o:
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(COMPILE) -c `test -f $< || echo '$(srcdir)/'`$<

.c.obj:
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(COMPILE) -c `cygpath -w $<`

.c.lo:
	source='$<' object='$@' libtool=yes \
	depfile='$(DEPDIR)/$*.Plo' tmpdepfile='$(DEPDIR)/$*.TPlo' \
	$(CCDEPMODE) $(depcomp) \
	$(LTCOMPILE) -c -o $@ `test -f $< || echo '$(srcdir)/'`$<
CCDEPMODE = depmode=gcc3
uninstall-info-am:

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES) config.h.in $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)config.h.in$$unique$(LISP)$$tags" \
	  || etags $(ETAGS_ARGS) $$tags config.h.in $$unique $(LISP)

GTAGS:
	here=`CDPATH=: && cd $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) $$here

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH

DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

top_distdir = ..
distdir = $(top_distdir)/$(PACKAGE)-$(VERSION)

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  if test -f $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    $(mkinstalldirs) "$(distdir)/$$dir"; \
	  fi; \
	  if test -d $$d/$$file; then \
	    cp -pR $$d/$$file $(distdir) \
	    || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
check: check-am
all-am: Makefile $(LTLIBRARIES) $(HEADERS) config.h

installdirs:
	$(mkinstalldirs) $(DESTDIR)$(libdir)

install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES) stamp-h stamp-h[0-9]*

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
clean: clean-am

clean-am: clean-generic clean-libLTLIBRARIES clean-libtool \
	mostlyclean-am

distclean: distclean-am

distclean-am: clean-am distclean-compile distclean-depend \
	distclean-generic distclean-hdr distclean-libtool \
	distclean-tags

dvi: dvi-am

dvi-am:

info: info-am

info-am:

install-data-am:

install-exec-am: install-libLTLIBRARIES

install-info: install-info-am

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-am

maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic \
	mostlyclean-libtool

uninstall-am: uninstall-info-am uninstall-libLTLIBRARIES

.PHONY: GTAGS all all-am check check-am clean clean-generic \
	clean-libLTLIBRARIES clean-libtool distclean distclean-compile \
	distclean-depend distclean-generic distclean-hdr \
	distclean-libtool distclean-tags distdir dvi dvi-am info \
	info-am install install-am install-data install-data-am \
	install-exec install-exec-am install-info install-info-am \
	install-libLTLIBRARIES install-man install-strip installcheck \
	installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic mostlyclean-libtool tags uninstall \
	uninstall-am uninstall-info-am uninstall-libLTLIBRARIES


# Say $(srcdir), so GNU make does not report an ambiguity with the .y.c rule.
$(srcdir)/getdate.c: getdate.y
	cd $(srcdir) && \
	  $(YACC) $(YFLAGS) getdate.y; \
	  mv -f y.tab.c getdate.c
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
