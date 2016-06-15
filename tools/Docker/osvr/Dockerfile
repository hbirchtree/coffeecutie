# Base it on good ol' trusty
FROM ubuntu:xenial
MAINTAINER hbirch

ADD vrpn-patch.p /vrpn-patch.patch

# List of system dependencies, rest come from git
RUN apt-get -y install software-properties-common

RUN bash -c "add-apt-repository \"deb http://archive.ubuntu.com/ubuntu $(lsb_release -sc) universe multiverse\""

RUN apt-get update && apt-get -y install \
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
	libusb-1.0-0-dev \
        libudev-dev \
        libgl1-mesa-dev \
        ninja-build \
        wget

RUN useradd -m bob

USER bob
WORKDIR /home/bob

# Download Oculus Rift SDK
RUN wget https://static.oculus.com/sdk-downloads/ovr_sdk_linux_0.5.0.1.tar.xz
RUN tar xvf ovr*.xz
WORKDIR /home/bob/ovr_sdk_linux_0.5.0.1
RUN sh ConfigureDebian.sh
RUN make
USER root
RUN make install

RUN ln -s /home/bob/ovr_sdk_linux_0.5.0.1/LibOVRKernel/Src/Kernel /usr/include/

USER bob
WORKDIR /home/bob

# Cloning repos
RUN git clone --recursive https://github.com/OSVR/OSVR-Core.git
RUN git clone --recursive https://github.com/OSVR/libfunctionality.git
RUN git clone --recursive https://github.com/VRPN/jsoncpp
RUN git clone --recursive https://github.com/OSVR/OSVR-Oculus-Rift.git
RUN git clone --recursive https://github.com/vrpn/vrpn.git

WORKDIR /home/bob/vrpn
RUN git checkout feature/oculus-rift

RUN bash -c "patch -p1 < /vrpn-patch.patch"

WORKDIR /home/bob
USER root

# Building JSONCPP
RUN mkdir /home/bob/json-build
WORKDIR /home/bob/json-build
RUN cmake ../jsoncpp/ -DJSONCPP_WITH_CMAKE_PACKAGE=ON -DJSONCPP_LIB_BUILD_SHARED=OFF -DCMAKE_CXX_FLAGS=-fPIC
RUN make
# Deploying JSONCPP to /usr/local
RUN make install

# Building libfunctionality
RUN mkdir /home/bob/libfun-build
WORKDIR /home/bob/libfun-build
RUN cmake ../libfunctionality/ -DBUILD_SHARED_LIBS=OFF -DCMAKE_CXX_FLAGS=-fPIC
RUN make
# Deploying libfunctionality to /usr/local
RUN make install

# At last, building OSVR-Core
RUN mkdir /home/bob/osvr-build
WORKDIR /home/bob/osvr-build
RUN cmake ../OSVR-Core -DCMAKE_CXX_FLAGS=-fPIC
RUN make
RUN make install
RUN cmake ../OSVR-Core -DCMAKE_INSTALL_PREFIX=/home/bob/osvr-bin/
RUN make install

# For some reason, VRPN must be built twice
RUN mkdir /home/bob/vrpn-build
WORKDIR /home/bob/vrpn-build
RUN cmake ../vrpn -DVRPN_USE_OVR=1
RUN make install

# Build Oculus Rift support
RUN mkdir /home/bob/osvr-ovr-build
WORKDIR /home/bob/osvr-ovr-build
RUN cmake ../OSVR-Oculus-Rift -DCMAKE_INSTALL_PREFIX=/home/bob/osvr-bin/ -DVRPN_USE_OVR=1
RUN make
RUN make install
