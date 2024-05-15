FROM ubuntu:18.04

#COPY package.json /package.json

RUN apt-get update && \
    apt-get install -y bc python make \
    u-boot-tools libncurses-dev \
    device-tree-compiler gparted git \
    build-essential libssl1.0-dev wget \
    cpio unzip rsync

#COPY src/ /usr/src/app

CMD ["bash"]