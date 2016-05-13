#!/bin/bash

function check_input()
{
    if [[ -z "$1" ]]; then
        printf "Build directory not specified!\n" 1>&2;
        exit 1;
    fi
    if [[ ! -d "$1" ]]; then
        printf "Valid build directory not specified!\n" 1>&2;
        exit 1;
    fi
}

check_input "$1";

if [[ -f "$1/build.ninja" ]]; then
    ninja -C "$1" install 1>&2
elif [[ -f "$1/Makefile" ]]; then
    make -C "$1" install 1>&2
fi

cd "$1"

if [[ "$(uname -s)" == "Linux" ]]; then
    zip -9 -r - "out" | base64 -w 0
else
    zip -9 -r - "out" | base64
fi
