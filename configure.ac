
AC_INIT([libtldevel], [m4_esyscmd_s([git describe --always])]) 




AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

#AM_PROG_AR
AC_PROG_RANLIB

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [log])
AC_CHECK_LIB([pthread], [pthread_create])


htslib_support=ok
AC_CHECK_LIB([hts], [fai_load],[], [htslib_support=missing])


if test $htslib_support != ok; then
  AC_MSG_ERROR([htslib not found

This packages uses the htslib library. Please install and try again!


https://github.com/samtools/htslib


FAILED.  This error must be resolved in order to build this package successfully.])
fi

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


# non-standard stuff

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
        CFLAGS="-pg -ggdb -Wall -m64 -std=gnu11"
else
        CFLAGS="-O2 -funroll-loops -Wall -std=gnu11"
        DEBUG=0
fi

AC_ARG_ENABLE(valgrind-tests,
    [  --enable-valgrind-tests run self tests under valgrind],
    opt_valgrind_tests="$enableval", opt_valgrind_tests="no")
    

OPTS="-q --trace-children=no --error-exitcode=1 --leak-check=yes"
  # Run self-tests under valgrind?
if test "$opt_valgrind_tests" = "yes"; then
        AC_DEFINE(DEBUG,1,[Defines debugging .])   
        CFLAGS="-pg -ggdb -pedantic -Wall -m64 -std=gnu11"
	AC_CHECK_PROGS(VALGRIND, valgrind)
	VALGRIND="$VALGRIND $OPTS"
else
	VALGRIND=
fi
AC_MSG_CHECKING([whether self tests are run under valgrind])
AC_MSG_RESULT($opt_valgrind_tests)



AC_CANONICAL_HOST

AC_SUBST([DATE]) DATE="`date`"

AC_SUBST([USER]) USER="`whoami`@`hostname`"

AC_SUBST([UNAME_INFO]) UNAME_INFO=`uname -n`

AC_SUBST([COMPILER]) COMPILER=`$CC --version | head -n 1`

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile
		 env_settings
                 helper/Makefile])
AC_OUTPUT



