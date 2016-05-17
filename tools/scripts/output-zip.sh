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

bdata="file.zip"

if [[ "$(uname -s)" == "Linux" ]]; then
    zip -9 -r - "out" > "$bdata"
else
    zip -9 -r - "out" > "$bdata"
fi

server_ip="$2"
server_port="$3"
data_arch="$4"
data_bid="$5"
data_host="$6"

curl -v -F file=@"$bdata" "http://$server_ip:$server_port/bin/upload/data/$data_arch/$data_host/$data_bid"
