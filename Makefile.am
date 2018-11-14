
SUBDIRS = helper


EXTRA_DIST=pwrite_test.sh

noinst_LIBRARIES = libtldevel.a


# HTSDIR              Directory containing HTSlib source tree
#     HTSLIB_CPPFLAGS     Preprocessor flags for compiling with HTSlib
#     HTSLIB_LDFLAGS


TLDEVELSOURCES = tldevel.h  tldevel.c build_config.c
RBTREESOURCES  = rbtree.h   rbtree.c
RTRTREESOURCES = rtr.h      rtr.c
THRPOOLSOURCES = thr_pool.h thr_pool.c
PWRITESOURCES  = pwrite.h   pwrite.c

libtldevel_a_SOURCES = $(TLDEVELSOURCES) $(RBTREESOURCES) $(RTRTREESOURCES) $(THRPOOLSOURCES) $(PWRITESOURCES)


# libhtsglue_a_SOURCES =  \
htsglue.h \
htsglue.c

AM_LDFLAGS = @DEAD_STRIP@
AM_LDFLAGS += -Wno-undef


TESTS= libtldevel_ITEST mem_ITEST rb_ITEST librtree_ITEST libthrpool_ITEST  libpwrite_ITEST pwrite_test.sh

TESTS_ENVIRONMENT = \
	$(VALGRIND)


check_PROGRAMS =  libtldevel_ITEST   rb_ITEST librtree_ITEST libthrpool_ITEST libpwrite_ITEST mem_ITEST

libtldevel_ITEST_SOURCES = $(TLDEVELSOURCES)
libtldevel_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DITEST
libtldevel_ITEST_LDADD =

mem_ITEST_SOURCES = mem_itest.c
mem_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)
mem_ITEST_LDADD = libtldevel.a


librtree_ITEST_SOURCES = $(RTRTREESOURCES) $(TLDEVELSOURCES)
librtree_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DRTRITEST
librtree_ITEST_LDADD =

rb_ITEST_SOURCES = $(RBTREESOURCES) $(TLDEVELSOURCES)
rb_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DRB_TEST
rb_ITEST_LDADD =

libthrpool_ITEST_SOURCES = $(THRPOOLSOURCES)
libthrpool_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DITEST
libthrpool_ITEST_LDADD =



# libhtsglue_ITEST_SOURCES = $(libhtsglue_a_SOURCES) $(TLDEVELSOURCES)  $(RBTREESOURCES)
# libhtsglue_ITEST_CPPFLAGS  = $(AM_CPPFLAGS) $(HTSLIB_CPPFLAGS)      -DHTSITEST
# libhtsglue_ITEST_LDADD = $(HTSLIB_LDFLAGS)  libhts.a


libpwrite_ITEST_SOURCES = $(PWRITESOURCES)
libpwrite_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DITEST
libpwrite_ITEST_LDADD = libtldevel.a  -lpthread

build_config.c: ./helper/refset env_settings
	./helper/refset  env_settings > build_config.c


CLEANFILES = \
	MAIN_0.chk \
	build_config.c \
	libhts_test.fa \
	libhts_test.fa.fai \
	pw_write_test.txt \
	testlog.txt \
	pwtest_t*

