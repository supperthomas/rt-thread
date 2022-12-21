FROM ubuntu:20.04

ARG DEST_DIR=/root

WORKDIR ${DEST_DIR}

#system
RUN apt-get update

RUN apt-get upgrade -y

RUN apt-get install -y vim

RUN apt-get install -y git

RUN rm /etc/localtime

RUN cp /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

RUN ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

RUN apt-get install -y qemu-system-arm

#env
RUN mkdir /root/.env

RUN mkdir /root/.env/tools

RUN mkdir /root/.env/packages

RUN mkdir /root/.env/packages/packages

RUN mkdir /root/.env/tools/scripts

RUN touch /root/.env/packages/Kconfig

Run echo "source $PKGS_DIR/packages/Kconfig" > /root/.env/packages/Kconfig

RUN git clone https://github.com/RT-Thread/env.git /root/.env/tools/scripts/

RUN git clone https://github.com/RT-Thread/packages.git /root/.env/packages/packages/

ENV PATH="/root/.env/tools/scripts:$PATH"

RUN apt-get install -y wget

RUN apt-get install -y python3 python-is-python3 

RUN apt-get install -y pip
#arm-none-eabi
RUN apt-get install -y gcc-arm-none-eabi
#rtthread
RUN apt-get install -y scons

RUN apt-get install -y libncurses5-dev

RUN pip install requests -qq

ENV RTT_EXEC_PATH=/usr/bin

RUN apt-get clean

RUN rm -rf /var/lib/apt/lists/*

ENV RTT_CC=gcc

WORKDIR /root



