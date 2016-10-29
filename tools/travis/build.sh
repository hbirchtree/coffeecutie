#!/bin/bash

case "${TRAVIS_OS_NAME}" in
"linux")
cmake -G"Ninja" -DCMAKE_INSTALL_PREFIX=out
;;
"osx")
cmake -G"Ninja" -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DCMAKE_INSTALL_PREFIX=out
;;
esac

cmake --build . --target install
