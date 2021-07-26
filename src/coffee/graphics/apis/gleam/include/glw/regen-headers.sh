#!/usr/bin/env bash

set -eu

CLANG_FORMAT=${CLANG_FORMAT:-clang-format}
ROOT_DIR=$(git rev-parse --show-toplevel)

cd $(dirname $0)

$CLANG_FORMAT --version

$ROOT_DIR/tools/code-tools/opengl-wrapper/gl_parse.py

$CLANG_FORMAT -i -style=file *.h versions/*.h enums/*.h extensions/*.h
