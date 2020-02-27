AC_INIT([tldevel], [1.2.0])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_CXX

AC_PROG_INSTALL

AM_SILENT_RULES([no])



LT_INIT
AC_CONFIG_MACRO_DIRS([m4])


m4_include([m4/ax_check_zlib.m4])
m4_include([m4/ax_check_compile_flag.m4])
m4_include([m4/ax_lib_hdf5.m4])
m4_include([m4/tldevel_set_compile_flags.m4])

AX_ENABLE_DEBUG

AX_LIB_HDF5([serial])
if test "$with_hdf5" = "yes"; then

dnl CC="$HDF5_CC"
CFLAGS="${CFLAGS} ${HDF5_CFLAGS}"
CPPFLAGS="${CPPFLAGS} ${HDF5_CPPFLAGS}"
LDFLAGS="${LDFLAGS} ${HDF5_LDFLAGS}"
LIBS="${LIBS} ${HDF5_LIBS}"
else
AC_MSG_WARN([

Unable to find HDF5, we need parallel HDF5.
Install on fedora:
sudo dnf install hdf5 hdf5-devel

])
fi

AM_CONDITIONAL([ENABLE_HDF5], [test "$with_hdf5" = "yes"])

AX_CHECK_ZLIB(
[
CPPFLAGS="$CPPFLAGS -I${ZLIB_HOME}/include"
LDFLAGS="$LDFLAGS -L${ZLIB_HOME}/lib"
LIBS="-lz $LIBS"
AC_DEFINE([HAVE_LIBZ], [1],[Define to 1 if you have `z' library (-lz)])
AM_CONDITIONAL([ENABLE_ZLIB], [test "$zlib_cv_zlib_h" = "yes"])
]
,
AC_MSG_WARN([Zlib not found!!!]))


# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [log])
AC_CHECK_LIB([pthread], [pthread_create])
AC_SEARCH_LIBS([clock_gettime],[rt])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([stdint.h])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([float.h])
# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T

AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

AC_TYPE_SSIZE_T
# Checks for library functions.
AC_C_INLINE
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CHECK_FUNCS([clock_gettime memset pow sqrt getpid])
AC_CHECK_FUNCS([strstr])


AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([strtol])


AC_ARG_WITH([htsdir],
  [AS_HELP_STRING([--with-htsdir=DIR],
    [use the HTSlib source tree or installation in DIR]),
    ],
  [], [with_htsdir=no])

AS_IF([test "$with_htsdir" = "no"],[
   AC_CHECK_HEADER([htslib/sam.h],
      [AC_CHECK_LIB(hts, hts_version, [ax_cv_htslib=yes], [ax_cv_htslib=no])],
      [ax_cv_htslib=no], [;])

   AM_CONDITIONAL([ENABLE_HTS], [test "$ax_cv_htslib" = "yes"])

   AS_IF([test "$ax_cv_htslib" = "yes"],[
      LIBS="-lhts $LIBS"
      AC_DEFINE([HAVE_HTS], [1],[Define to 1 if you have `z' library (-lz)])
   ])
],[

  HTSDIR=$with_htsdir
  if test -f "$HTSDIR/hts.c" && test -f "$HTSDIR/htslib/hts.h"; then
    ax_cv_local_htslib=yes
    else
    ax_cv_local_htslib=no
  fi

   AS_IF([test "$ax_cv_local_htslib" = "no"],
      [AC_MSG_ERROR([Local hts library was not found here: $HTSDIR  ])],
   [
   CPPFLAGS="$CPPFLAGS -I${HTSDIR}/htslib"
   LDFLAGS="$LDFLAGS -L${HTSDIR}"
   LIBS="-lhts $LIBS"
   ]
   )

   AM_CONDITIONAL([ENABLE_HTS], [test "$ax_cv_local_htslib" = "yes"])
])


# non-standard stuff


AC_CANONICAL_HOST
case $host_os in
darwin* )
DEAD_STRIP="-Wl,-dead_strip"
;;
*)
DEAD_STRIP="-Wl,--gc-sections -Wl,--as-needed"
;;
esac
AC_SUBST(DEAD_STRIP)



AX_VALGRIND_DFLT([memcheck], [on])
AX_VALGRIND_CHECK

AC_SUBST([DATE]) DATE="`date`"

AC_SUBST([USER]) USER="`whoami`@`hostname`"

AC_SUBST([UNAME_INFO]) UNAME_INFO=`uname -n`

AC_SUBST([COMPILER]) COMPILER=`$CC --version | head -n 1`



dnl ac_configure_args="$ac_configure_args PACKAGE_VERSION=$PACKAGE_VERSION   PACKAGE_NAME=$PACKAGE_NAME"


AC_CONFIG_FILES([
Makefile
dev/Makefile
])

AC_CONFIG_HEADERS([config.h])

AC_OUTPUT


AC_MSG_NOTICE([---------------------------------------------------------])
AC_MSG_NOTICE([Configuration complete - $PACKAGE_NAME-$PACKAGE_VERSION])
AC_MSG_NOTICE([])
AC_MSG_NOTICE([General Information:])
AC_MSG_NOTICE([               Version: $VERSION])
AC_MSG_NOTICE([         Configured on: $DATE])
AC_MSG_NOTICE([         Configured by: $USER])
AC_MSG_NOTICE([           Host system: $host_cpu-$host_vendor-$host_os])
AC_MSG_NOTICE([     Uname information: $UNAME_INFO])
AC_MSG_NOTICE([                CFLAGS: $CFLAGS])
AC_MSG_NOTICE([               CPFLAGS: $CPPFLAGS])
AC_MSG_NOTICE([                  LIBS: $LIBS])
AC_MSG_NOTICE([               LDFLAGS: $LDFLAGS])
AC_MSG_NOTICE([    Installation point: $prefix])
AC_MSG_NOTICE([---------------------------------------------------------])

AS_IF([test "$ax_cv_htslib" = "no"],[
   AC_MSG_NOTICE([])
   AC_MSG_NOTICE([---------------------------------------------------------])
   AC_MSG_WARN([Could not find the hts library!])
   AC_MSG_WARN([The tldevel library will be compiled without:])
   AC_MSG_WARN([- fasta I/O])
   AC_MSG_WARN([- fastq I/O])
   AC_MSG_WARN([- SAM/BAM/CRAM I/O])
   AC_MSG_WARN([download & install the library from here: ])
   AC_MSG_WARN([https://github.com/samtools/htslib])
   AC_MSG_WARN([and re-run configure])
   AC_MSG_NOTICE([---------------------------------------------------------])
   AC_MSG_NOTICE([])
])


AS_IF([test "$with_hdf5" = "no"],[
   AC_MSG_NOTICE([])
   AC_MSG_NOTICE([---------------------------------------------------------])
   AC_MSG_WARN([Could not find the HDF5 library!])
   AC_MSG_WARN([The tldevel library will be compiled without:])
   AC_MSG_WARN([- hdf5 support ])
   AC_MSG_NOTICE([---------------------------------------------------------])
   AC_MSG_NOTICE([])
])

