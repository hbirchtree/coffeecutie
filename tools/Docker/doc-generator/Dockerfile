#
# This Dockerfile will create a stock compilation environment on Ubuntu LTS
# The listed dependencies might aid in finding dependencies for other distributions
#

FROM ubuntu:xenial
MAINTAINER H. Birch Tree

RUN apt-get -qy update && apt-get -qy install \
        cmake build-essential \
	doxygen graphviz

#Create user directory + group
RUN groupadd coffeeusers -g 1000
RUN useradd coffee -m -u 1000

WORKDIR /home/coffee
USER coffee

RUN mkdir -p project bridge build/bin build/lib build/out

VOLUME ["/home/coffee/project","/home/coffee/build","/home/coffee/build/out"]

WORKDIR /home/coffee/build

CMD ["/bin/bash","-c","cmake -C /home/coffee/project/cmake/Preload/none_docs-none-none.cmake /home/coffee/project && cmake --build . --target install"]
