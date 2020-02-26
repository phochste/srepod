# Makefile for cgi-util example

# by Bill Kendrick
# bill@newbreedsoftware.com
# http://www.newbreedsoftware.com/cgi-util/

# June 12, 1999 - August 28, 2000


# Makefile options:

CC=gcc
CFLAGS=-O -Wall
LIBDIR=/usr/local/include
INCDIR=/usr/local/lib


# Makefile "commands":

default:
	@echo
	@echo "cgi-util make options:"
	@echo "----------------------"
	@echo "install    - install everything"
	@echo "install-o  - install only the cgi-util.o object"
	@echo "install-a  - install the libcgi-util.a static library"
	@echo "install-so - install the libcgi-util.so shared library"
	@echo "test-cgis  - compile test.cgi, filetest.cgi (using cgi-util.o)"
	@echo

install:	install-o install-a install-so test-cgis

install-h:	cgi-util.h
	cp cgi-util.h $(INCDIR)/
	chmod 644 $(INCDIR)/cgi-util.h

install-o:	cgi-util.o install-h
	cp cgi-util.o $(LIBDIR)/
	chmod 644 $(LIBDIR)/cgi-util.o

install-a:	libcgi-util.a install-h
	cp libcgi-util.a $(LIBDIR)/
	chmod 644 $(LIBDIR)/libcgi-util.a

install-so:	libcgi-util.so install-h
	cp libcgi-util.so $(LIBDIR)/
	chmod 644 $(LIBDIR)/libcgi-util.so

test-cgis:	test.cgi filetest.cgi
	chmod 755 test.cgi filetest.cgi
	chmod 644 test.html filetest.html

clean:
	-rm cgi-util.o libcgi-util.a libcgi-util.so libcgi-util.o
	-rm test.o test.cgi filetest.o filetest.cgi


# The test CGIs:

test.cgi:	test.o cgi-util.o
	$(CC) $(CFLAGS) test.o cgi-util.o -o test.cgi
	chmod 755 test.cgi

filetest.cgi:	filetest.o cgi-util.o
	$(CC) $(CFLAGS) filetest.o cgi-util.o -o filetest.cgi
	chmod 755 filetest.cgi


# Test CGIs Objects:

test.o:	test.c cgi-util.h

filetest.o:	filetest.c cgi-util.h


# Objects:

cgi-util.o:	cgi-util.c cgi-util.h

libcgi-util.so:
	$(CC) $(CFLAGS) -fPIC cgi-util.c -c -o libcgi-util.o
	$(CC) -shared -Wl,-s libcgi-util.o -o libcgi-util.so

libcgi-util.a:	cgi-util.o
	ar rcs libcgi-util.a cgi-util.o
