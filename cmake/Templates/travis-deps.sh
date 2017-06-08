#!/bin/bash

case "${TRAVIS_OS_NAME}" in
"linux")
    #sudo apt-get update -qq
    #sudo apt-get install -qy libsdl2-dev libopenal-dev cmake ninja-build swig3.0 libunwind8-dev
;;
"osx")
    brew install homebrew/versions/swig304
    brew install sdl2 openal-soft cmake ninja openssl
    exit 0
;;
esac
