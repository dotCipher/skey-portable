# Makefile - S/KEY v1.1b
#
# Neil M. Haller <nmh@thumper.bellcore.com>
# Philip R. Karn <karn@chicago.qualcomm.com>
# John S. Walden <jsw@thumper.bellcore.com>
# Scott Chasin <chasin@crimelab.com>
#
# Modifications:
# Cody Moore <cjmoore@cs.sunysb.edu>

CC = /usr/bin/gcc

DESTDIR = /root/Temp/

MANDIR = $(DESTDIR)/man
BINDIR = $(DESTDIR)/bin
HEADERDIR = $(DESTDIR)/include
LIBDIR = $(DESTDIR)/lib

CFLAGS	= -Wall -Werror -DUSE_ECHO -lcrypt

LIB     =  libskey.a
LIBOBJS =  skeylogin.o skeysubr.o md4.o put.o debug.o
LIBSRCS =  skeylogin.c skeysubr.c md4.c put.c debug.c

all: $(LIB) key keyinit keysh done

setuid:
	chmod u+s keyinit keysh

install: all
	chmod u+s keyinit keysh
	mkdir -p $(BINDIR)
	mkdir -p $(MANDIR)
	mkdir -p $(HEADERDIR)
	mkdir -p $(LIBDIR)
	install key $(BINDIR)
	install keyinit $(BINDIR)
	install keysh $(BINDIR)
	install ../man/*.1 $(MANDIR)
	install *.h $(HEADERDIR)
	install $(LIB) $(LIBDIR)

key: skey.c
	$(CC) -o $@ $(CFLAGS) skey.c $(LIB) $(SYSLIBS)

keyinit: skeyinit.c
	$(CC) -o $@ $(CFLAGS) skeyinit.c $(LIB) $(SYSLIBS)

keysh: skeysh.c
	$(CC) -o $@ $(CFLAGS) skeysh.c $(LIB) $(SYSLIBS)

$(LIB): $(LIBOBJS)
	ar rv $(LIB) $(LIBOBJS)

done:
	@echo "Make completed."

clean:
	rm -f key keyinit keysh *.o libskey.a *.bak *.BAK *.s tags
