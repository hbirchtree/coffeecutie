#!/bin/bash

# Fix for github:travis-ci/travis-ci#4704
python -c "import fcntl; fcntl.fcntl(1, fcntl.F_SETFL, 0)"

case "${TRAVIS_OS_NAME}" in
"linux")
    echo " * Travis/CI people, if you are seeing this, some systems require extended binary format support to work :/"
    docker run --rm --privileged multiarch/qemu-user-static:register

    sudo apt-get -qy install python3-pip python-requests python-yaml python3-requests python3-paho-mqtt python3-setuptools python3-dev
;;
"osx")
    #brew install sdl2 cmake openssl python3 python jq
    #brew upgrade python python3
    brew link python3
    pip3 install pyyaml requests paho-mqtt
;;
*)
    echo " * No OS name was provided, this is an error"
    exit 1
;;
esac

# Common to all platforms
pip3 install oyaml # for ordered yaml creation

