#!/bin/bash

case "${TRAVIS_OS_NAME}" in
"linux")
    echo " * Travis/CI people, if you are seeing this, some systems require extended binary format support to work :/"
    docker run --rm --privileged multiarch/qemu-user-static:register

    sudo apt-get -qy install python3-requests python3-yaml
;;
"osx")
    which python3 >/dev/null && brew install python3
    brew install sdl2 cmake ninja openssl swig@3.04 jq
    exit 0
;;
esac
