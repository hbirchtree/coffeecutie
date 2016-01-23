# Stick with an LTS release, it has the appropriate CMake version
FROM ubuntu:xenial
MAINTAINER H. Birch Tree

ARG ndkver="10e"
ARG sdkver="24.4.1"

# Install dependencies for Android SDK/NDK as well as build tools
RUN dpkg --add-architecture i386
RUN apt-get -qy update
RUN apt-get -qy install openjdk-7-jdk mercurial git cmake swig usbutils openssl ninja-build make nano libc6:i386 libstdc++6:i386 wget

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

# Install some script-sauce to create our dev environment
RUN ["/bin/bash","-c","echo '' >> .bashrc"]
RUN ["/bin/bash","-c","echo 'export ANDROID_HOME=$HOME/android-sdk-linux' >> .bashrc"]
RUN ["/bin/bash","-c","echo 'export ANDROID_SDK=$ANDROID_HOME' >> .bashrc"]
RUN ["/bin/bash","-c","echo 'export ANDROID_NDK=$HOME/android-ndk-linux' >> .bashrc"]
RUN ["/bin/bash","-c","echo 'export PATH=$ANDROID_NDK:$ANDROID_SDK/tools:$ANDROID_SDK/platform-tools:$PATH' >> .bashrc"]
RUN ["/bin/bash","-c","echo 'export ANDROID=$(which android)' >> .bashrc"]

RUN mkdir lib

WORKDIR /home/coffee/lib

RUN git clone https://github.com/apportable/openal-soft.git
RUN git clone https://github.com/taka-no-me/android-cmake.git
RUN hg clone http://hg.libsdl.org/SDL SDL2

# Set the final working directory and user
WORKDIR /home/coffee
USER coffee

VOLUME ["/home/coffee/project","/home/coffee/bridge","/home/coffee/build"]
