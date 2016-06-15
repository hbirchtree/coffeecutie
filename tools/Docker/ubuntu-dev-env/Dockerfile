#
# This Dockerfile will create a stock compilation environment on Ubuntu LTS
# The listed dependencies might aid in finding dependencies for other distributions
#

FROM ubuntu:xenial
MAINTAINER H. Birch Tree

RUN apt-get -qy update && apt-get -qy install \
        git nano wget \
        cmake ninja-build build-essential \
        clang-3.8 \
        doxygen swig3.0 \
        libsdl2-dev libopenal-dev libssl-dev libunwind-dev \
        libavcodec-dev libavdevice-dev \
        libavformat-dev libswresample-dev \
        libswscale-dev \
	qt5-default \
	qtcreator numix-gtk-theme \
	libxcb-keysyms1-dev libxcb-image0-dev \
	libxcb-shm0-dev libxcb-icccm4-dev libxcb-sync0-dev libxcb-xfixes0-dev \
	libxcb-shape0-dev libxcb-randr0-dev libxcb-render-util0-dev \
	libfontconfig1-dev libfreetype6-dev libx11-dev libxext-dev libxfixes-dev \
	libxi-dev libxrender-dev libxcb1-dev libx11-xcb-dev libxcb-glx0-dev x11vnc \
	xauth build-essential mesa-common-dev libglu1-mesa-dev libxkbcommon-dev \
	libxcb-xkb-dev libxslt1-dev libgstreamer-plugins-base0.10-dev \
	mesa-utils

#RUN gsettings set org.gnome.desktop.interface gtk-theme "Numix"

RUN mkdir -p /etc/sudoers.d/

#Create user directory + group, make it compatible with 
RUN export uid=1000 gid=1000 && \
    mkdir -p /home/coffee && \
    echo "coffee:x:${uid}:${gid}:CoffeeDev,,,:/home/coffee:/bin/bash" >> /etc/passwd && \
    echo "coffee:x:${uid}:" >> /etc/group && \
    echo "coffee ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/coffee && \
    chmod 0440 /etc/sudoers.d/coffee && \
    chown ${uid}:${gid} -R /home/coffee

WORKDIR /home/coffee
USER coffee

ENV HOME /home/coffee

RUN mkdir -p project bridge build/bin build/lib

VOLUME ["/home/coffee/project","/home/coffee/bridge","/home/coffee/output"]
