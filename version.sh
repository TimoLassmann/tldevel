#!/bin/bash


if [[ ! $RRIDTT_VERSION ]]; then
    if git rev-parse --git-dir > /dev/null 2>&1; then
        RRIDTT_VERSION=`git describe --always > /dev/null`
    fi
fi

if [[ ! $RRIDTT_VERSION ]]; then
    RRIDTT_VERSION=`cat VERSION`
fi


if [[ ! $RRIDTT_VERSION ]]; then
    RRIDTT_VERSION="[na]"
fi
echo $RRIDTT_VERSION

