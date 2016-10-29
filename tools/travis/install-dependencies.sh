#!/bin/bash

case "${TRAVIS_OS_NAME}" in
"linux")
sudo apt-get update -qq
sudo apt-get install -qy libsdl2-dev libopenal-dev cmake ninja-build
;;
"osx")
brew install sdl2 openal-soft cmake ninja
;;
esac
