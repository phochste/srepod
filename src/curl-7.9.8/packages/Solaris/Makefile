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
top_srcdir = ../..

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
top_builddir = ../..

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
RANDOM_FILE = /dev/urandom
RANLIB = ranlib
STRIP = strip
VERSION = 7.9.8
VERSIONNUM = 070908
YACC = bison -y
am__include = include
am__quote = 
install_sh = /home/hochsten/srepod/src/curl-7.9.8/install-sh
PKGADD_NAME = "curl-7.9.8 - cURL - a client that groks URLs"

PKGADD_PKG = "HAXXcurl"
PKGADD_VENDOR = "curl.haxx.se"
PKGADD_ARCH = "i686"
PKGADD_DESC = "curl-7.9.8 - cURL - a client that groks URLs"
PKGADD_CLASSES = none
PKGADD_BASEDIR = /

PKGADD_FILE = ${PKGADD_PKG}-curl-7.9.8-i686-pc-linux-gnu.pkg
subdir = packages/Solaris
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = $(top_builddir)/lib/config.h \
	$(top_builddir)/src/config.h \
	$(top_builddir)/tests/server/config.h
CONFIG_CLEAN_FILES =
DIST_SOURCES =
DIST_COMMON = Makefile.am Makefile.in
all: all-am

.SUFFIXES:

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:
	-rm -f libtool
$(srcdir)/Makefile.in:  Makefile.am  $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && \
	  $(AUTOMAKE) --gnu  packages/Solaris/Makefile
Makefile:  $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) && \
	  CONFIG_HEADERS= CONFIG_LINKS= \
	  CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status
uninstall-info-am:
tags: TAGS
TAGS:


DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

top_distdir = ../..
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
all-am: Makefile

installdirs:

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

clean-am: clean-generic clean-libtool mostlyclean-am

distclean: distclean-am

distclean-am: clean-am distclean-generic distclean-libtool

dvi: dvi-am

dvi-am:

info: info-am

info-am:

install-data-am:

install-exec-am:

install-info: install-info-am

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-am

maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-generic mostlyclean-libtool

uninstall-am: uninstall-info-am

.PHONY: all all-am check check-am clean clean-generic clean-libtool \
	distclean distclean-generic distclean-libtool distdir dvi \
	dvi-am info info-am install install-am install-data \
	install-data-am install-exec install-exec-am install-info \
	install-info-am install-man install-strip installcheck \
	installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-generic \
	mostlyclean-libtool uninstall uninstall-am uninstall-info-am


package:
	if [ ! -f release ]; then echo 0 > release; fi
	if [ `find root -newer release -print|wc -l` -gt 0 ]; then expr `cat release` + 1 > release; fi
	echo PKG=${PKGADD_PKG} > pkginfo
	echo NAME=${PKGADD_NAME} >> pkginfo
	echo VENDOR=${PKGADD_VENDOR} >> pkginfo
	echo ARCH=${PKGADD_ARCH} >> pkginfo
	echo DESC=${PKGADD_DESC} >> pkginfo
	echo CLASSES=${PKGADD_CLASSES} >> pkginfo
	echo BASEDIR=${PKGADD_BASEDIR} >> pkginfo
	echo CATEGORY=application >> pkginfo
	echo VERSION=`cat ./release` >> pkginfo
	echo > prototype
	for subdir in opt var etc usr; \
	do if [ -d root/$$subdir ]; then \
		pkgproto root/$$subdir=$$subdir >> prototype;\
	fi; done
	for file in pkginfo preinstall postinstall copyright; \
	do if [ -f $$file ]; then \
		echo "i $$file=$$file" >> prototype; \
	fi; done
	rm -rf ./tmp
	mkdir ./tmp
	pkgmk -o -d ./tmp
	pkgtrans -so ./tmp ${top_srcdir}/../${PKGADD_FILE} ${PKGADD_PKG}
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
