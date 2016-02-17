# Base it on good ol' trusty
FROM ubuntu:trusty
MAINTAINER hbirch

# List of system dependencies, rest come from git
RUN apt-get -y install software-properties-common

RUN bash -c "add-apt-repository \"deb http://archive.ubuntu.com/ubuntu $(lsb_release -sc) universe multiverse\""

RUN apt-get update

RUN apt-get -y install \
	git \
	build-essential \
	cmake \
	libopencv-dev \
	libboost-dev \
	libboost-thread-dev \
	libboost-system-dev \
	libboost-date-time-dev \
	libboost-chrono-dev \
	libboost-program-options-dev \
	libboost-filesystem-dev \
	libusb-1.0-0-dev

RUN useradd -m bob

USER bob
WORKDIR /home/bob

# Cloning repos
RUN git clone --recursive https://github.com/OSVR/OSVR-Core.git
RUN git clone --recursive https://github.com/OSVR/libfunctionality.git
RUN git clone --recursive https://github.com/VRPN/jsoncpp

USER root
RUN add-apt-repository "ppa:george-edison55/cmake-3.x"

RUN apt-get update
RUN apt-get -y install cmake

USER bob

# Building JSONCPP
RUN mkdir /home/bob/json-build
WORKDIR /home/bob/json-build
RUN cmake ../jsoncpp/ -DJSONCPP_WITH_CMAKE_PACKAGE=ON -DJSONCPP_LIB_BUILD_SHARED=OFF -DCMAKE_CXX_FLAGS=-fPIC
RUN make
# Deploying JSONCPP to /usr/local
USER root
RUN make install

# Building libfunctionality
USER bob
RUN mkdir /home/bob/libfun-build
WORKDIR /home/bob/libfun-build
RUN cmake ../libfunctionality/
RUN make
# Deploying libfunctionality to /usr/local
RUN cmake ../libfunctionality/ -DCMAKE_INSTALL_PREFIX=/home/bob/osvr-bin/
RUN make install
USER root
RUN cmake ../libfunctionality/
RUN make install

# Extra pass... Just a secret!
USER bob

# At last, building OSVR-Core
USER bob
RUN mkdir /home/bob/osvr-build
WORKDIR /home/bob/osvr-build
RUN cmake ../OSVR-Core -DCMAKE_INSTALL_PREFIX=/home/bob/osvr-bin/
RUN make install
