#!/bin/bash

case "${TRAVIS_OS_NAME}" in
"linux")
    echo " * Travis/CI people, if you are seeing this, some systems require extended binary format support to work :/"
    docker run --rm --privileged multiarch/qemu-user-static:register
;;
"osx")
    # Qt5 package is for github-cli
    brew install sdl2 cmake ninja openssl qt5 swig@3.04
    exit 0
;;
esac
