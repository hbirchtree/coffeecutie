FROM jenkinsci/jenkins
MAINTAINER H. Birch Tree

RUN apt update && apt install \
	build-essential libsdl2-dev libssl-dev libopenal-dev \
	cmake ninja-build

CMD ["/bin/tini -- /usr/local/bin/jenkins.sh"]
