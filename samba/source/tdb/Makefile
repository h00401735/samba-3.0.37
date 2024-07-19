#!gmake
#
# Makefile for tdb directory
#

CC = gcc
prefix = /usr/local
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
includedir = ${prefix}/include
libdir = ${exec_prefix}/lib
VPATH = ../lib/replace
srcdir = .
builddir = .
CPPFLAGS =  -I$(srcdir)/include -Iinclude -I../include -I../lib/replace
CFLAGS = $(CPPFLAGS) -g -O2
LDFLAGS = 
EXEEXT = 

.PHONY: test

PROGS = bin/tdbtool$(EXEEXT)
PROGS_NOINSTALL = bin/tdbtest$(EXEEXT) bin/tdbdump$(EXEEXT) bin/tdbbackup$(EXEEXT)
ALL_PROGS = $(PROGS) $(PROGS_NOINSTALL)

TDB_OBJ = common/tdb.o common/dump.o common/transaction.o common/error.o common/traverse.o common/freelist.o common/freelistcheck.o common/io.o common/lock.o common/open.o

DIRS = bin common tools

all: showflags dirs $(PROGS)

showflags:
	@echo 'tdb will be compiled with flags:'
	@echo '  CFLAGS = $(CFLAGS)'
	@echo '  CPPFLAGS = $(CPPFLAGS)'
	@echo '  LDFLAGS = $(LDFLAGS)'
	@echo '  LIBS = $(LIBS)'

.c.o:
	@echo Compiling $*.c
	@mkdir -p `dirname $@`
	@$(CC) $(CFLAGS) -c $< -o $@

dirs:
	@mkdir -p $(DIRS)

install: all
	mkdir -p $(bindir)
	mkdir -p $(includedir)
	mkdir -p $(libdir) 
	mkdir -p $(libdir)/pkgconfig
	cp $(PROGS) $(bindir)
	cp $(srcdir)/include/tdb.h $(includedir)
	cp tdb.pc $(libdir)/pkgconfig

libtdb.a: $(TDB_OBJ)
	ar -rv libtdb.a $(TDB_OBJ)

bin/tdbtest$(EXEEXT): tools/tdbtest.o libtdb.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o bin/tdbtest tools/tdbtest.o -L. -ltdb -lgdbm

bin/tdbtool$(EXEEXT): tools/tdbtool.o libtdb.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o bin/tdbtool tools/tdbtool.o -L. -ltdb

bin/tdbdump$(EXEEXT): tools/tdbdump.o libtdb.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o bin/tdbdump tools/tdbdump.o -L. -ltdb

bin/tdbbackup$(EXEEXT): tools/tdbbackup.o libtdb.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o bin/tdbbackup tools/tdbbackup.o -L. -ltdb

installcheck: test install

clean:
	rm -f $(ALL_PROGS) *.o *.a common/*.o tools/*.o tdb.pc
	rm -f test.db test.tdb torture.tdb test.gdbm

distclean: clean
	rm -f *~ */*~
	rm -f config.log config.status include/config.h config.cache
	rm -f Makefile

realdistclean: distclean
	rm -f configure include/config.h.in
