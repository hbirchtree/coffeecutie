#
# Untested, has issues with Alpine's STL (does not compile)
#
#

FROM alpine:latest
MAINTAINER H. Birch Tree

RUN apk update && apk add \
	build-base cmake ninja git mercurial \
	sdl2-dev openal-soft-dev openssl-dev

RUN adduser builder -D -h /builder

USER builder
RUN mkdir /builder/pkg
