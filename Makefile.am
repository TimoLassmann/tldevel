
noinst_LIBRARIES = libtldevel.a librtree.a libhtsglue.a

AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = helper

libtldevel_a_SOURCES = \
tldevel.h \
tldevel.c \
rbtree.h \
rbtree.c \
build_config.c

librtree_a_SOURCES = libtldevel.a \
rtr.c \
rtr.h


AM_CPPFLAGS = \
-I$(top_builddir)/libinc/htslib-1.2.1 \
-I$top_srcdir/libinc/htslib-1.2.1 


libhtsglue_a_SOURCES = libtldevel.a librtree.a  \
htsglue.h \
htsglue.c




TESTS= libtldevel_ITEST rb_ITEST librtree_ITEST

check_PROGRAMS =  libtldevel_ITEST   rb_ITEST librtree_ITEST

libtldevel_ITEST_SOURCES = $(libtldevel_a_SOURCES)
libtldevel_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DITEST
libtldevel_ITEST_LDADD =    -lm

librtree_ITEST_SOURCES = $(librtree_a_SOURCES)
librtree_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DITEST 
librtree_ITEST_LDADD =    $(LIBTLDEVEL) -ltldevel -L.

rb_ITEST_SOURCES = $(libtldevel_a_SOURCES) 
rb_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DRB_TEST
rb_ITEST_LDADD =  $(LIBTLDEVEL)



build_config.c: ./helper/refset env_settings
	./helper/refset  env_settings > build_config.c


CLEANFILES = build_config.c testlog.txt


