FROM alpine:latest
MAINTAINER H. Birch Tree

RUN apk update && apk add \
	build-base cmake ninja git mercurial \
	sdl2-dev openal-soft-dev openssl-dev

RUN adduser builder -D -h /builder

USER builder
RUN mkdir /builder/pkg
