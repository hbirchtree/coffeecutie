#!/bin/bash

# OSX needs this apparently
realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

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
elif [[ "`uname -s`" == *"CYGWIN"* ]]; then
    cmake_path=`cygpath -w "$1"`
    echo "$cmake_path"
    cmake --build "$cmake_path" --target install
else
    cmake --build "$1" --target install
fi

cd "$1"

bdata="file.zip"

if [[ "$(uname -s)" == "Linux" ]]; then
    zip -q -9 -r - "out" > "$bdata"
else
    zip -q -9 -r - "out" > "$bdata"
fi

bdata=`realpath "$bdata"`

cd "$COFFEE_REPO_DIR"

server_ip="$2"
server_port="$3"
data_arch="$4"
data_bid="$(git rev-parse HEAD)"
data_host="$(hostname)"

cd "$1"

echo curl -v -F file=@`basename "$bdata"` "http://$server_ip:$server_port/bin/upload/data/$data_arch/$data_host/$data_bid"

curl -v -0 -F file=@`basename "$bdata"` "http://$server_ip:$server_port/bin/upload/data/$data_arch/$data_host/$data_bid"
