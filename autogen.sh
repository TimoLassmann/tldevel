#!/usr/bin/env sh
inside_git_repo=

inside_git_repo="$(git rev-parse --is-inside-work-tree 2>/dev/null)"


test -n "$srcdir" || srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.

if [ "$inside_git_repo" ]; then
    echo "Updating git sub-modules"
    cd "$srcdir"
    git submodule update --init --recursive
fi

autoreconf --force --install --verbose "$srcdir"

test -n "$NOCONFIGURE" || "$srcdir/configure" "$@"
