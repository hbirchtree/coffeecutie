#!/bin/bash

# OSX needs this apparently
realpath() {
    [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

build_dir=`realpath "$1"`

cd "$(dirname $0)"

export COFFEE_REPO_DIR="$(realpath ../../ )"

exec ./output-zip.sh "$build_dir" "$2" "$3" "$4"
