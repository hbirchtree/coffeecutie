#
# This Dockerfile will create an environment for building against the Steam Runtime as a means of staying portable on Linux
# Any dependencies needed to build will be installed
# The build process will still be performed manually
#

FROM ubuntu:trusty
MAINTAINER H. Birch Tree

RUN apt-get -qy update && apt-get -qy install \
	git

RUN mkdir -p /build

WORKDIR /build
RUN git clone https://github.com/ValveSoftware/steam-runtime.git

RUN /bin/bash steam-runtime/setup_chroot.sh --amd64 --output-dir /build/steamrt
