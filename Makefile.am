
noinst_LIBRARIES = libtldevel.a

AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = helper

libtldevel_a_SOURCES = \
tldevel.h \
tldevel.c \
rbtree.h \
rbtree.c \
build_config.c

check_PROGRAMS =  libtldevel_ITEST rb_ITEST

libtldevel_ITEST_SOURCES = $(libtldevel_a_SOURCES)
libtldevel_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DITEST
libtldevel_ITEST_LDADD =    -lm

build_config.c: ./helper/refset env_settings
	./helper/refset  env_settings > build_config.c


CLEANFILES = build_config.c testlog.txt

TESTS= libtldevel_ITEST rb_ITEST

rb_ITEST_SOURCES = $(libtldevel_a_SOURCES)
rb_ITEST_CPPFLAGS  = $(AM_CPPFLAGS)  -DRB_TEST
rb_ITEST_LDADD =  $(LIBTLDEVEL)
