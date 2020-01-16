
SUBDIRS = dev

ACLOCAL_AMFLAGS = -I m4


EXTRA_DIST= README.org autogen.sh version.sh VERSION ax_with_tldevel.m4

AM_LDFLAGS = @DEAD_STRIP@
AM_LDFLAGS += -Wno-undef

lib_LTLIBRARIES = libtldevel.la
#noinst_LIBRARIES = libtldevel.a

#     HTSLIB_CPPFLAGS     Preprocessor flags for compiling with HTSlib
#     HTSLIB_LDFLAGS      Linker flags for linking with HTSlib


TLHASHTABLESOURCES = tlhashtable.c
TLMINHASHSOURCES = tlminhash.c
TLDEVELSOURCES = tldevel.c
TLRNGSOURCES = tlrng.c
TLLOGSUMSOURCES = tllogsum.c
TLBITVECSOURCES = tlbitvec.c
TLRBTREESOURCES  = tlrbtree.h tlrbtree.c
TLCHECKPOINTSOURCES = tlcheckpoint.c
TLMISCSOURCES = tlmisc.c
TLALPHABETSOURCES = tlalphabet.c
TLSEQIOSOURCES = tlseqio.c tlseqbuffer.c
TLHTSWRAPSOURCES = tlhtswrap.c tlseqbuffer.c
TLHDF5WRAPSOURCES = tlhdf5wrap.c 
#lib_LTLIBRARIES = libgettext.la
#libgettext_la_SOURCES = gettext.c gettext.h …

# From klib

KSTRING_SOURCES =  kstring.c


include_HEADERS = \
tlhashtable.h \
tlminhash.h \
tldevel.h \
tlrng.h \
tllogsum.h \
tlbitvec.h \
tlrbtree.h \
tlcheckpoint.h \
tlmisc.h \
tlalphabet.h \
kstring.h \
khash.h

if ENABLE_HTS
include_HEADERS += tlseqio.h tlseqbuffer.h
else
if ENABLE_ZLIB
include_HEADERS += tlseqio.h tlseqbuffer.h
endif
endif


if ENABLE_HDF5
include_HEADERS += tlhdf5wrap.h tlhdf5wrap_types.h
endif

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
$(KSTRING_SOURCES)

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

