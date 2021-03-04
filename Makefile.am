SUBDIRS = dev

ACLOCAL_AMFLAGS = -I m4


EXTRA_DIST= README.org autogen.sh version.sh VERSION ax_with_tldevel.m4

AM_LDFLAGS = @DEAD_STRIP@
AM_LDFLAGS += -Wno-undef

noinst_LTLIBRARIES =  libtldevel.la
#noinst_LIBRARIES = libtldevel.a

#     HTSLIB_CPPFLAGS     Preprocessor flags for compiling with HTSlib
#     HTSLIB_LDFLAGS      Linker flags for linking with HTSlib


TLHASHTABLESOURCES = tlhashtable.h tlhashtable.c
TLMINHASHSOURCES = tlminhash.h tlminhash.c
TLDEVELSOURCES = tldevel.h tldevel.c
TLRNGSOURCES = tlrng.h tlrng.c
TLLOGSUMSOURCES = tllogsum.h tllogsum.c
TLBITVECSOURCES = tlbitvec.h tlbitvec.c
TLRBTREESOURCES  = tlrbtree.h tlrbtree.c
TLCHECKPOINTSOURCES = tlcheckpoint.h tlcheckpoint.c
TLMISCSOURCES = tlmisc.h tlmisc.c
TLALPHABETSOURCES = tlalphabet.h tlalphabet.c
TLSEQIOSOURCES = tlseqio.h tlseqbuffer.h tlseqio.c tlseqbuffer.c
TLHTSWRAPSOURCES = tlseqio.h tlseqbuffer.h tlhtswrap.c tlseqbuffer.c
TLHDF5WRAPSOURCES = tlhdf5wrap.h tlhdf5wrap_types.h tlhdf5wrap.c

ALNVECALLOCSOURCES = alignedvec_alloc.h alignedvec_alloc.c

KSTRING_SOURCES =  kstring.h kstring.c
KHASH_SOURCES = khash.h
KSA_SOURCES = ksa.c ksa.h

ESL_TIMER_SOURCES = esl_stopwatch.h esl_stopwatch.c

libtldevel_la_SOURCES = \
$(TLHASHTABLESOURCES) \
$(TLMINHASHSOURCES) \
$(TLDEVELSOURCES) \
$(TLRNGSOURCES) \
$(TLLOGSUMSOURCES) \
$(TLBITVECSOURCES) \
$(TLRBTREESOURCES) \
$(TLCHECKPOINTSOURCES) \
$(TLMISCSOURCES) \
$(TLALPHABETSOURCES) \
$(KSTRING_SOURCES) \
$(KHASH_SOURCES) \
$(KSA_SOURCES) \
$(ESL_TIMER_SOURCES) \
$(ALNVECALLOCSOURCES)
if ENABLE_HTS
libtldevel_la_SOURCES += $(TLHTSWRAPSOURCES)
else
if ENABLE_ZLIB
libtldevel_la_SOURCES += $(TLSEQIOSOURCES)
endif
endif

if ENABLE_HDF5
libtldevel_la_SOURCES += $(TLHDF5WRAPSOURCES)
endif


libtldevel_la_LIBADD = $(LIBS)
