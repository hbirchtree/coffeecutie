#!/bin/bash

case "${TRAVIS_OS_NAME}" in
"linux")
    echo " * Travis/CI people, if you are seeing this, some systems require extended binary format support to work :/"
    docker run --rm --privileged multiarch/qemu-user-static:register
;;
"osx")
    brew install homebrew/versions/swig304
    brew install sdl2 openal-soft cmake ninja openssl
    exit 0
;;
esac
