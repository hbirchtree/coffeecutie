#!/usr/bin/env bash

set -eu

ROOT_DIR=$(git rev-parse --show-toplevel)
CLANG_FORMAT=${CLANG_FORMAT:-${ROOT_DIR}/multi_build/compilers/bin/clang-format}

cd $(dirname $0)

$CLANG_FORMAT --version

$ROOT_DIR/tools/code-tools/opengl-wrapper/gl_parse.py

$CLANG_FORMAT -i -style=file *.h versions/*.h enums/*.h extensions/*.h
