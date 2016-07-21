#
# Uses the following project: https://github.com/waneck/linux-ios-toolchain
# DOES NOT WORK
#

FROM ubuntu:xenial
MAINTAINER H. Birch Tree

RUN apt-get -qy update && apt-get -qy install git gcc g++ clang libclang-dev uuid-dev libssl-dev libpng12-dev libicu-dev bison flex libsqlite3-dev libtool llvm-dev libxml2-dev automake pkg-config

RUN mkdir /build

WORKDIR /build

RUN git clone --recursive https://github.com/waneck/linux-ios-toolchain.git
WORKDIR /build/linux-ios-toolchain
