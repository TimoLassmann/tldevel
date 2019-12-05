
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
TLCHECKPOINTSOURCES = tlcheckpoint.h tlcheckpoint.c
TLMISCSOURCES = tlmisc.h tlmisc.c

#RTRTREESOURCES = rtr.h      rtr.c
#THRPOOLSOURCES = thr_pool.h thr_pool.c
#PWRITESOURCES  = pwrite.h   pwrite.c
#HASHTABLESOURCES = hash_table.h hash_table.c

#RBSOURCES = rb.h rb.c

libtldevel_a_SOURCES = \
$(TLHASHTABLESOURCES) \
$(TLMINHASHSOURCES) \
$(TLDEVELSOURCES) \
$(TLRNGSOURCES) \
$(TLLOGSUMSOURCES) \
$(TLBITVECSOURCES) \
$(TLRBTREESOURCES) \
$(TLCHECKPOINTSOURCES) \
$(TLMISCSOURCES)


# libhtsglue_a_SOURCES =  \
htsglue.h \
htsglue.c

AM_LDFLAGS = @DEAD_STRIP@
AM_LDFLAGS += -Wno-undef

TESTLIST = \
tldevel_test \
rng_test \
checkpoint_test \
rbtree_test \
logsum_test \
hash_test \
minhash_test

TESTS = $(TESTLIST)

check_PROGRAMS = $(TESTLIST)

tldevel_test_SOURCES = $(TLDEVELSOURCES) tldevel_test.c
tldevel_test_CPPFLAGS = $(AM_CPPFLAGS)
tldevel_test_LDADD =

rng_test_SOURCES = $(TLRNGSOURCES) $(TLDEVELSOURCES) tlrng_test.c
rng_test_CPPFLAGS = $(AM_CPPFLAGS)
rng_test_LDADD =

checkpoint_test_SOURCES = $(TLCHECKPOINTSOURCES) $(TLDEVELSOURCES) $(TLMISCSOURCES) tlcheckpoint_test.c
checkpoint_test_CPPFLAGS = $(AM_CPPFLAGS)
checkpoint_test_LDADD =


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

build_config.c: ./helper/refset env_settings
	./helper/refset  env_settings > build_config.c

CLEANFILES = \
	MAIN_0.chk


