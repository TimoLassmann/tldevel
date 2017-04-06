
EXTRA_DIST=pwrite_test.sh

noinst_LIBRARIES = libtldevel.a librtree.a libhtsglue.a libthrpool.a libpwrite.a 

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

librtree_a_LIBADD = libtldevel.a

libhtsglue_a_SOURCES =  \
htsglue.h \
htsglue.c

libhtsglue_a_LIBADD = libtldevel.a librtree.a

libthrpool_a_SOURCES = \
thr_pool.h \
thr_pool.c

libpwrite_a_SOURCES =\
pwrite.h \
pwrite.c

libpwrite_a_LIBADD = libtldevel.a

TESTS= libtldevel_ITEST rb_ITEST librtree_ITEST libthrpool_ITEST libhtsglue_ITEST libpwrite_ITEST pwrite_test.sh

check_PROGRAMS =  libtldevel_ITEST   rb_ITEST librtree_ITEST libthrpool_ITEST libhtsglue_ITEST libpwrite_ITEST

libtldevel_ITEST_SOURCES = $(libtldevel_a_SOURCES)
libtldevel_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DITEST
libtldevel_ITEST_LDADD =    -lm

librtree_ITEST_SOURCES = $(librtree_a_SOURCES)
librtree_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DITEST 
librtree_ITEST_LDADD =    $(LIBTLDEVEL) -ltldevel -L.

rb_ITEST_SOURCES = $(libtldevel_a_SOURCES) 
rb_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DRB_TEST
rb_ITEST_LDADD =  $(LIBTLDEVEL)

libthrpool_ITEST_SOURCES = $(libthrpool_a_SOURCES) 
libthrpool_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DITEST
libthrpool_ITEST_LDADD = 



libhtsglue_ITEST_SOURCES = $(libhtsglue_a_SOURCES) 
libhtsglue_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DITEST
libhtsglue_ITEST_LDADD = libtldevel.a librtree.a


libpwrite_ITEST_SOURCES = $(libpwrite_a_SOURCES) 
libpwrite_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DITEST
libpwrite_ITEST_LDADD = libtldevel.a  -lpthread libthrpool.a






build_config.c: ./helper/refset env_settings
	./helper/refset  env_settings > build_config.c


CLEANFILES = \
	build_config.c \
	libhts_test.fa \
	libhts_test.fa.fai \
	pw_write_test.txt \
	testlog.txt \
	pwtest_t*

