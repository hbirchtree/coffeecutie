#
# This Dockerfile will create a stock compilation environment on Ubuntu LTS
# The listed dependencies might aid in finding dependencies for other distributions
#

FROM ubuntu:xenial
MAINTAINER H. Birch Tree

RUN dpkg --add-architecture i386
RUN apt-get -qy update

RUN apt-get -y install software-properties-common

RUN bash -c "add-apt-repository \"deb http://archive.ubuntu.com/ubuntu $(lsb_release -sc) universe multiverse\""

RUN apt-get -qy update && apt-get -qy install \
        git nano wget \
        cmake ninja-build build-essential \
        clang-3.8 \
        doxygen swig3.0:i386 \
        libsdl2-dev libopenal-dev:i386 libssl-dev:i386 libunwind-dev:i386 \
        libavcodec-dev:i386 libavdevice-dev:i386 \
        libavformat-dev:i386 libswresample-dev:i386 \
        libswscale-dev:i386 \
	qt5-default:i386

# PCL for point cloud data processing
#RUN apt-get -qy install libpcl-dev:i386

#Create user directory + group
RUN groupadd coffeeusers -g 1000
RUN useradd coffee -m -u 1000

WORKDIR /home/coffee
USER coffee

RUN mkdir -p project bridge build/bin build/lib

VOLUME ["/home/coffee/project","/home/coffee/bridge","/home/coffee/build/bin","/home/coffee/build/lib"]
