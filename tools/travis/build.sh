#!/bin/bash

case "${TRAVIS_OS_NAME}" in
"linux")
cmake -G"Ninja" -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchains/linux-generic_linux.toolchain.cmake -Ccmake/Preload/linux-generic.cmake -DCMAKE_INSTALL_PREFIX=out

cmake -DCOFFEE_GENERATE_FLATPAK=OFF -DCOFFEE_GENERATE_SNAPPY=OFF
;;
"osx")
cmake -G"Ninja" -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchains/osx-generic_osx.toolchain.cmake -Ccmake/Preload/osx-generic.cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DSWIG_DIR=/usr/local/opt/swig304 -DCMAKE_INSTALL_PREFIX=out
;;
esac

cmake --build . --target install

tar -zcf binaries_${TRAVIS_OS_NAME}_${CC}.tar.gz out/*
