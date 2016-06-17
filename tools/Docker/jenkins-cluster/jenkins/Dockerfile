FROM jenkins
MAINTAINER H. Birch Tree

USER root
RUN apt-get update && apt-get -y install \
	build-essential libsdl2-dev libssl-dev libopenal-dev \
	ninja-build wget

RUN echo "jenkins ALL=NOPASSWD: ALL" >> /etc/sudoers

RUN mkdir -p /build
RUN git clone https://cmake.org/cmake.git /build/cmake
WORKDIR /build/cmake
RUN /build/cmake/configure
RUN make install

COPY plugins.txt /usr/share/jenkins/plugins.txt
RUN /usr/local/bin/plugins.sh /usr/share/jenkins/plugins.txt

COPY *.groovy /usr/share/jenkins/ref/init.groovy.d/
COPY *.properties $JENKINS_HOME/

USER jenkins
WORKDIR /
CMD ["/bin/tini","--","/usr/local/bin/jenkins.sh"]
