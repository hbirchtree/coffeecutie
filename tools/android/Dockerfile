#
# This Dockerfile will construct an Android build environment fit to support building CoffeeCutie.
# The Android SDK, NDK and its dependencies will be installed and usable with the attached android-native-build.sh script
#

# Stick with an LTS release, it has the appropriate CMake version
FROM ubuntu:xenial
MAINTAINER H. Birch Tree

ARG ndkver="10e"
ARG sdkver="24.4.1"
ARG sdktarget="19"

# Install dependencies for Android SDK/NDK as well as build tools
RUN dpkg --add-architecture i386
RUN apt-get -qy update
RUN apt-get -qy install openjdk-8-jdk-headless mercurial git cmake swig usbutils openssl ninja-build make nano libc6:i386 libstdc++6:i386 wget ant

# Create users for dev
RUN groupadd coffeeusers -g 1000
RUN useradd coffee -m -u 1000

# Set up the Android NDK and SDK
WORKDIR /home/coffee
USER coffee

# Download and extract the Android NDK, that's all.
RUN wget http://dl.google.com/android/ndk/android-ndk-r${ndkver}-linux-x86_64.bin -O android-ndk-linux-x86_64.bin
RUN chmod +x android-ndk-linux-x86_64.bin
RUN ./android-ndk-linux-x86_64.bin
RUN mv android-ndk-r${ndkver} android-ndk-linux
RUN rm android-ndk-linux-x86_64.bin

# Download and update the Android SDK
RUN wget http://dl.google.com/android/android-sdk_r${sdkver}-linux.tgz -O android-sdk.tgz
RUN tar xvf android-sdk.tgz
RUN ["/bin/bash","-c","( sleep 5 && while [ 1 ]; do sleep 1; echo y; done ) | android-sdk-linux/tools/android update sdk -s -u --filter 'tools,build-tools-23.0.2,platform-tools,android-19'"]
RUN rm android-sdk.tgz

# Install repositories for builds
RUN mkdir /home/coffee/lib
WORKDIR /home/coffee/lib

RUN git clone https://github.com/apportable/openal-soft.git
RUN git clone https://github.com/taka-no-me/android-cmake.git
RUN hg clone http://hg.libsdl.org/SDL SDL2

# Install some script-sauce to create our dev environment
WORKDIR /home/coffee

RUN ["/bin/bash","-c","echo '' >> .bashrc"]
RUN ["/bin/bash","-c","echo 'export ANDROID_HOME=$HOME/android-sdk-linux' >> .bashrc"]
RUN ["/bin/bash","-c","echo 'export ANDROID_SDK=$ANDROID_HOME' >> .bashrc"]
RUN ["/bin/bash","-c","echo 'export ANDROID_NDK=$HOME/android-ndk-linux' >> .bashrc"]
RUN ["/bin/bash","-c","echo 'export PATH=$ANDROID_NDK:$ANDROID_SDK/tools:$ANDROID_SDK/platform-tools:$PATH' >> .bashrc"]
RUN ["/bin/bash","-c","echo 'export ANDROID=$(which android)' >> .bashrc"]

ADD android-sdk-update.sh /home/coffee/android-sdk-update.sh
ADD build-settings.rc /home/coffee/build-settings.rc
# Instructions from: https://wiki.libsdl.org/Android
ADD sdl-build.sh /home/coffee/sdl-build.sh
# Instructions from: https://github.com/apportable/openal-soft
ADD openal-build.sh /home/coffee/openal-build.sh
ADD coffee-build.sh /home/coffee/coffee-build.sh
ADD install-libs.sh /home/coffee/install-libs.sh
ADD generate-toolchain.sh /home/coffee/generate-toolchain.sh

# Manual modifications, likely to break!
# Also, static linking of OpenAL is not advisible (licenses)
ADD openal-soft-static-patch.patch /home/coffee/openal-soft-static-patch.patch

USER root
RUN chmod +x *.sh
USER coffee

# Set the final working directory and user
WORKDIR /home/coffee
USER coffee

VOLUME ["/home/coffee/project","/home/coffee/bridge","/home/coffee/build"]

USER root
RUN chown coffee:coffee /home/coffee/build
USER coffee
