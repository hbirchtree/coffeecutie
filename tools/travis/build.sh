#!/bin/bash

case "${TRAVIS_OS_NAME}" in
"linux")
cmake -G"Ninja" -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchains/native-linux-generic.toolchain.cmake -Ccmake/Preload/x86_64-linux-generic.cmake -DCMAKE_INSTALL_PREFIX=out -DCOFFEE_GENERATE_FLATPAK=OFF -DCOFFEE_GENERATE_SNAPPY=OFF
;;
"osx")
cmake -G"Ninja" -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchains/native-macintosh-generic.toolchain.cmake -Ccmake/Preload/x86_64-osx-generic.cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DSWIG_DIR=/usr/local/opt/swig304 -DCMAKE_INSTALL_PREFIX=out
;;
esac

cmake --build . --target install
