#! /bin/sh
etags    configure.ac
etags -a INSTALL
etags -a COPYING

# Recursively add all .c, .h, .pl, *.tex, *.man
find . -name "*.c"   -print -or -name "*.h"  -print | xargs etags -a
find . -name "*.sh"  -print                         | xargs etags -a
find . -name "*.in"  -print                         | xargs etags -a
find . -name "*README"    -print                    | xargs etags -a
