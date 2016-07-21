#
# This Dockerfile will create an environment for building against the Steam Runtime as a means of staying portable on Linux
# Any dependencies needed to build will be installed
# The build process will still be performed manually
#

FROM ubuntu:trusty
MAINTAINER H. Birch Tree

# We need newer CMake, 3.1+
RUN apt-get -qy update && apt-get -qy install software-properties-common
RUN add-apt-repository ppa:george-edison55/cmake-3.x

# Steam beta repository
#RUN echo "deb http://repo.steampowered.com/steamrt/ scout_beta main" >> /etc/apt/sources.list
# Steam non-beta repository
RUN echo "deb http://repo.steampowered.com/steamrt/ scout main" >> /etc/apt/sources.list

ADD key.gpg /key.gpg

RUN cat /key.gpg | apt-key add -
RUN rm /key.gpg

RUN apt-get -qy update && apt-get -qy install \
	git nano wget \
	cmake ninja-build build-essential gdb \
	steamrt-dev

# From here, do "cmake -GNinja ${SOURCE_DIR}; ninja"
# This will hopefully produce SteamRT-compatible binaries
# Can also be used as a Jenkins slave
