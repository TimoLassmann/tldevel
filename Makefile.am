
SUBDIRS = dev

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST= README.org autogen.sh version.sh VERSION ax_with_tldevel.m4

AM_LDFLAGS = @DEAD_STRIP@
AM_LDFLAGS += -Wno-undef

lib_LTLIBRARIES = libtldevel.la
#noinst_LIBRARIES = libtldevel.a

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
TLSEQIOSOURCES = tlseqio.c

#lib_LTLIBRARIES = libgettext.la
#libgettext_la_SOURCES = gettext.c gettext.h …

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
tlseqio.h 



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
$(TLALPHABETSOURCES)
# !ENABLE_ZLIB actually works!
if ENABLE_ZLIB
libtldevel_la_SOURCES += $(TLSEQIOSOURCES)
endif
