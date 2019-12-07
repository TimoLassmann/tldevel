#!/usr/bin/env bash
DIR=`pwd`
TARGETDIR=


function usage()
{

    printf "This script will copy tldevel into a C project.\n\n" ;
    printf "usage: $0 -p <path to project >>\n\n" ;
    exit 1;
}

function next()
{
    printf "Add this to configure.ac:\n\n" ;

    printf "   AC_CONFIG_FILES([\n" ;
    printf "      lib_tldevel/Makefile\n" ;
    printf "      lib_tldevel/dev/Makefile\n" ;
    printf "      Makefile\n" ;
    printf "    ])\n" ;
    printf "    AC_CONFIG_HEADERS([config.h])\n\n" ;


    printf "Add this to your top level Makefile.am:\n\n" ;
    printf "    SUBDIRS = lib_tldevel lib_tldevel/dev\n\n" ;


    
    printf "Add this to your src/Makefile.am.\n\n" ;

    printf "   LIBS = ../libtldevel.a -lm\n";
    printf "   AM_CPPFLAGS = -I$(top_srcdir)/lib_tldevel\n";
    printf "   XXXXX_LDADD = ../libtldevel.a\n";

    exit 1;
}





while getopts p:  opt
do
    case ${opt} in
        p) TARGETDIR=${OPTARG};;
        ,*) usage;;
    esac
done
if [ "${TARGETDIR}" == "" ]; then usage; fi


rsync -av --progress  --exclude "*.o" lib_tldevel $TARGETDIR


next;

