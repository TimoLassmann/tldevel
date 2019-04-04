AC_INIT([libtldevel], [m4_esyscmd_s([./version.sh])])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB
AM_SILENT_RULES([no])
AC_CONFIG_MACRO_DIR([m4])

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [log])
AC_CHECK_LIB([pthread], [pthread_create])

#AC_CHECK_LIB([hts],[fai_build])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h])

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

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime memset pow sqrt])
AC_CHECK_FUNCS([arc4random arc4random_uniform])


AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([strtol])

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

AC_ARG_ENABLE(debugging,
[AS_HELP_STRING([--enable-debugging],[include debugging code])
AS_HELP_STRING([--enable-debugging=x],[also set diagnostics verbosity level to <x> (1-3)])],
enable_debugging=$enableval, enable_debugging="no")

case $enable_debugging in
yes)  AC_DEFINE(DEBUGLEVEL, 0,[No debugging. ]);;
1)  AC_DEFINE(DEBUGLEVEL, 1,[Defines debugging level 1.]);;
2)  AC_DEFINE(DEBUGLEVEL, 2,[Defines debugging level 2.]);;
3)  AC_DEFINE(DEBUGLEVEL, 3,[Defines debugging level 3.]);;
no)  AC_DEFINE(DEBUGLEVEL, 0,[No debugging.]);;
*)  AC_MSG_ERROR([Unknown argument to --enable-debugging: $enable_debugging]);;
esac



if test "$enable_debugging" != "no"; then
        AC_DEFINE(DEBUG,1,[Defines debugging .])
        CFLAGS="-ggdb -Wall -m64 -std=gnu11 -fdata-sections -ffunction-sections"
else
        CFLAGS="-O3 -funroll-loops -Wall -std=gnu11 -fdata-sections -ffunction-sections"
        DEBUG=0
fi


AX_VALGRIND_DFLT([memcheck], [on])
AX_VALGRIND_CHECK


AC_CANONICAL_HOST

AC_SUBST([DATE]) DATE="`date`"

AC_SUBST([USER]) USER="`whoami`@`hostname`"

AC_SUBST([UNAME_INFO]) UNAME_INFO=`uname -n`

AC_SUBST([COMPILER]) COMPILER=`$CC --version | head -n 1`



ac_configure_args="$ac_configure_args PACKAGE_VERSION=$PACKAGE_VERSION   PACKAGE_NAME=$PACKAGE_NAME"


AC_CONFIG_FILES([Makefile
		 env_settings
                 helper/Makefile])
AC_OUTPUT



