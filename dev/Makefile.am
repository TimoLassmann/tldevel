ACLOCAL_AMFLAGS = -I m4
AM_LDFLAGS = @DEAD_STRIP@
AM_LDFLAGS += -Wno-undef

@VALGRIND_CHECK_RULES@

AM_CFLAGS = -I$(top_srcdir)/ -I${top_builddir}

TESTLIST = \
tldevel_test \
tlmisc_test \
rng_test \
checkpoint_test \
rbtree_test \
logsum_test \
hash_test \
alphabet_test \
seqio_test \
demo \
kstring_test \
khash_test \
ksa_test \
esl_stopwatch_test \
tl_alloc_test

# minhash_test

if ENABLE_HDF5
TESTLIST+= hdf5_test
endif


TESTS = $(TESTLIST)

check_PROGRAMS = $(TESTLIST)

demo_SOURCES= demo.c

esl_stopwatch_test_SOURCES = eslStopWatchTest.c
esl_stopwatch_test_CPPFLAGS = $(AM_CPPFLAGS)
esl_stopwatch_test_LDADD = ../libtldevel.la

tlmisc_test_SOURCES = tlmisc_test.c
tlmisc_test_CPPFLAGS = $(AM_CPPFLAGS)
tlmisc_test_LDADD = ../libtldevel.la


hdf5_test_SOURCES = tlhdf5wrap_test.c
hdf5_test_CPPFLAGS = $(AM_CPPFLAGS)
hdf5_test_LDADD = ../libtldevel.la

ksa_test_SOURCES = ksa_test.c
ksa_test_CPPFLAGS = $(AM_CPPFLAGS)
ksa_test_LDADD = ../libtldevel.la


khash_test_SOURCES = khash_test.c
khash_test_CPPFLAGS = $(AM_CPPFLAGS)
khash_test_LDADD = ../libtldevel.la


kstring_test_SOURCES = kstring_test.c
kstring_test_CPPFLAGS = $(AM_CPPFLAGS)
kstring_test_LDADD = ../libtldevel.la


tldevel_test_SOURCES = tldevel_test.c
tldevel_test_CPPFLAGS = $(AM_CPPFLAGS)
tldevel_test_LDADD = ../libtldevel.la

rng_test_SOURCES = tlrng_test.c
rng_test_CPPFLAGS = $(AM_CPPFLAGS)
rng_test_LDADD = ../libtldevel.la

alphabet_test_SOURCES = tlalphabet_test.c
alphabet_test_CPPFLAGS = $(AM_CPPFLAGS)
alphabet_test_LDADD = ../libtldevel.la

seqio_test_SOURCES = tlseqio_test.c
seqio_test_CPPFLAGS = $(AM_CPPFLAGS)
seqio_test_LDADD = ../libtldevel.la


checkpoint_test_SOURCES = tlcheckpoint_test.c
checkpoint_test_CPPFLAGS = $(AM_CPPFLAGS)
checkpoint_test_LDADD = ../libtldevel.la


rbtree_test_SOURCES = tlrbtree_test.c
rbtree_test_CPPFLAGS = $(AM_CPPFLAGS)
rbtree_test_LDADD = ../libtldevel.la

logsum_test_SOURCES = tllogsum_test.c
logsum_test_CPPFLAGS = $(AM_CPPFLAGS)
logsum_test_LDADD = ../libtldevel.la


hash_test_SOURCES = tlhashtable_test.c
hash_test_CPPFLAGS = $(AM_CPPFLAGS)
hash_test_LDADD = ../libtldevel.la

# minhash_test_SOURCES = tlminhash_test.c
# minhash_test_CPPFLAGS = $(AM_CPPFLAGS)
# minhash_test_LDADD = ../libtldevel.la


tl_alloc_test_SOURCES = \
tlalloc_test.c
tl_alloc_test_CPPFLAGS = $(AM_CPPFLAGS)
tl_alloc_test_LDADD = ../libtldevel.la

CLEANFILES = \
	MAIN_0.chk \
	hdf5testfile.h5 \
	tlrng_hdf5.h5
