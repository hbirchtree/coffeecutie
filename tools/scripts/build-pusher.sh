#!/bin/bash

# Inputs:
# 1: build directory containing out/ directory
# 2: server address
# 3: server port
# 4: system string

# OSX needs this apparently
realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

build_dir=`realpath "$1"`

cd "$(dirname $0)"

export COFFEE_REPO_DIR="$(realpath ../../ )"

exec ./output-zip.sh "$build_dir" "$2" "$3" "$4"
