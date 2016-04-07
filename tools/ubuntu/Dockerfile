#
# This Dockerfile will create a stock compilation environment on Ubuntu LTS
# The listed dependencies might aid in finding dependencies for other distributions
#

FROM ubuntu:xenial
MAINTAINER H. Birch Tree

RUN apt-get -qy update

# We use a tier-based installation based. If one step fails, it will still save some of the others.
# It will also be less bad if you cancel one of the installations mid-way

# Base tools
RUN apt-get -qy install \
	git nano wget \
	cmake ninja-build build-essential \
	clang-3.8 \
	doxygen swig3.0 \
        libsdl2-dev libopenal-dev libssl-dev libunwind-dev \
        libupower-glib-dev

# FFMPEG for media playback/capture
RUN apt-get -qy install \
	libavcodec-dev libavdevice-dev \
	libavformat-dev libswresample-dev \
	libswscale-dev

# Qt libraries
RUN apt-get -qy install qt5-default

# PCL for point cloud data processing
#RUN apt-get -qy install libpcl-dev

#Create user directory + group
RUN groupadd coffeeusers -g 1000
RUN useradd coffee -m -u 1000

WORKDIR /home/coffee
USER coffee

RUN mkdir -p project bridge build/bin build/lib

VOLUME ["/home/coffee/project","/home/coffee/bridge","/home/coffee/build/bin","/home/coffee/build/lib"]
