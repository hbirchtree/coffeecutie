#!/bin/bash

case "${TRAVIS_OS_NAME}" in
"linux")
    echo " * Travis/CI people, if you are seeing this, some systems require extended binary format support to work :/"
    docker run --rm --privileged multiarch/qemu-user-static:register

    sudo apt-get -qy install python-requests python-yaml python3-requests
;;
"osx")
    #brew install python3
    brew install sdl2 cmake ninja openssl
    easy_install pip
    sudo easy_install pip
    pip install pyyaml requests
    exit 0
;;
esac
