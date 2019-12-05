
SUBDIRS = helper

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST=pwrite_test.sh version.sh autogen.sh VERSION

noinst_LIBRARIES = libtldevel.a

TLHASHTABLESOURCES = tlhashtable.h tlhashtable.c
TLMINHASHSOURCES = tlminhash.h tlminhash.c
TLDEVELSOURCES = tldevel.h  tldevel.c
TLRNGSOURCES = tlrng.c tlrng.h
TLLOGSUMSOURCES = tllogsum.h tllogsum.c
TLBITVECSOURCES = tlbitvec.h tlbitvec.c
TLRBTREESOURCES  = tlrbtree.h tlrbtree.c

RTRTREESOURCES = rtr.h      rtr.c
#THRPOOLSOURCES = thr_pool.h thr_pool.c
PWRITESOURCES  = pwrite.h   pwrite.c
#HASHTABLESOURCES = hash_table.h hash_table.c

RBSOURCES = rb.h rb.c

libtldevel_a_SOURCES = $(TLDEVELSOURCES)  $(MINHASHSOURCES)  $(RBTREESOURCES) $(RTRTREESOURCES) $(THRPOOLSOURCES) $(PWRITESOURCES) $(RBSOURCES) $(RNGSOURCES)
# $(HASHTABLESOURCES)
# libhtsglue_a_SOURCES =  \
htsglue.h \
htsglue.c

AM_LDFLAGS = @DEAD_STRIP@
AM_LDFLAGS += -Wno-undef

TESTS = tldevel_test rng_test rbtree_test logsum_test hash_test minhash_test

check_PROGRAMS = tldevel_test  rng_test rbtree_test logsum_test  hash_test minhash_test #libtldevel_ITEST  ht_ITEST rb_ITEST librtree_ITEST libthrpool_ITEST libpwrite_ITEST mem_ITEST libavlrb_ITEST minhash_ITEST rng_ITEST

tldevel_test_SOURCES = $(TLDEVELSOURCES) tldevel_test.c
tldevel_test_CPPFLAGS = $(AM_CPPFLAGS)
tldevel_test_LDADD =

rng_test_SOURCES = $(TLRNGSOURCES) $(TLDEVELSOURCES) tlrng_test.c
rng_test_CPPFLAGS = $(AM_CPPFLAGS)
rng_test_LDADD =


rbtree_test_SOURCES = $(TLRBTREESOURCES) $(TLDEVELSOURCES) tlrbtree_test.c
rbtree_test_CPPFLAGS = $(AM_CPPFLAGS)
rbtree_test_LDADD =

logsum_test_SOURCES = $(TLLOGSUMSOURCES) $(TLDEVELSOURCES) tllogsum_test.c
logsum_test_CPPFLAGS = $(AM_CPPFLAGS)
logsum_test_LDADD =


hash_test_SOURCES = $(TLHASHTABLESOURCES) $(TLDEVELSOURCES) tlhashtable_test.c
hash_test_CPPFLAGS = $(AM_CPPFLAGS)
hash_test_LDADD =

minhash_test_SOURCES = $(TLMINHASHSOURCES) $(TLRNGSOURCES) $(TLBITVECSOURCES) $(TLDEVELSOURCES) tlminhash_test.c
minhash_test_CPPFLAGS = $(AM_CPPFLAGS)
minhash_test_LDADD =


#tllogsum.h tllogsum.c test_tldev.c tlbitvec.h tlbitvec.c tlcheckpoint.h tlcheckpoint.c tlmisc.h tlmisc.c tlrng.h tlrng.c

libavlrb_ITEST_SOURCES = rb_itest.c $(RBSOURCES)
libavlrb_ITESTCPPFLAGS = $(AM_CPPFLAGS)
libavlrb_ITEST_LDADD = libtldevel.a


libtldevel_ITEST_SOURCES = $(TLDEVELSOURCES)
libtldevel_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DITEST
libtldevel_ITEST_LDADD =

mem_ITEST_SOURCES = mem_itest.c
mem_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)
mem_ITEST_LDADD = libtldevel.a

ht_ITEST_SOURCES = $(TLDEVELSOURCES) $(HASHTABLESOURCES)
ht_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DHTITEST
ht_ITEST_LDADD =

librtree_ITEST_SOURCES = $(RTRTREESOURCES) $(TLDEVELSOURCES)
librtree_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DRTRITEST
librtree_ITEST_LDADD =

rb_ITEST_SOURCES = $(RBTREESOURCES) $(TLDEVELSOURCES)
rb_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DRB_TEST
rb_ITEST_LDADD =




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


