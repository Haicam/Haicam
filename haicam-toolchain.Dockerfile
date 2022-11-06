#docker build -f haicam-toolchain.Dockerfile -t haicam/haicam-toolchain:latest --force-rm .
#docker run -it --rm --user $(id -u):$(id -g) -v ~/toolchain:/home/haicam/toolchain haicam/haicam-toolchain:latest
#docker push haicam/haicam-toolchain:latest

FROM ubuntu:20.04

MAINTAINER Haicam <support@haicam.tech>

RUN apt-get update
RUN apt-get install -y busybox
RUN mkdir /usr/local/busybox_bin
RUN busybox --install -s /usr/local/busybox_bin
ENV PATH=$PATH:/usr/local/busybox_bin

RUN dpkg --add-architecture i386
RUN apt-get update
RUN apt-get install -y libc6:i386 libstdc++6:i386 zlib1g:i386

RUN mkdir -p /usr/src/arm-linux-3.3
ADD ./toolchain/toolchain_gnueabi-4.4.0_ARMv5TE.tgz /usr/src/arm-linux-3.3/

ADD ./toolchain/go1.14.15.linux-amd64.tar.gz /usr/local
ENV PATH=$PATH:/usr/local/go/bin

RUN apt-get install -y gcc-mingw-w64-i686 gcc-mingw-w64-x86-64

RUN update-alternatives --set i686-w64-mingw32-gcc /usr/bin/i686-w64-mingw32-gcc-posix
RUN update-alternatives --set x86_64-w64-mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc-posix

RUN echo "deb http://dk.archive.ubuntu.com/ubuntu/ xenial main" >> /etc/apt/sources.list
RUN echo "deb http://dk.archive.ubuntu.com/ubuntu/ xenial universe" >> /etc/apt/sources.list

RUN apt update
RUN apt-get install -y --no-install-recommends gcc-4.7 gcc-4.7-multilib g++-4.7 g++-4.7-multilib
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.7 20
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.7 20

RUN apt-get install -y --no-install-recommends make cmake

RUN groupadd -g 1000 haicam
RUN useradd -m -u 1000 -g 1000 haicam

RUN apt-get install -y --no-install-recommends clang patch git ca-certificates python3 libxml2-dev libssl-dev lzma liblzma-dev zlib1g-dev

WORKDIR /tmp
RUN git clone --depth=1 https://github.com/ezhomelabs/xar.git

WORKDIR /tmp/xar/xar
RUN ./configure
RUN make
RUN make install

WORKDIR /tmp
RUN git clone --depth=1 https://github.com/ezhomelabs/pbzx.git

WORKDIR /tmp/pbzx
RUN clang -llzma -lxar -Wl,-rpath,/usr/local/lib -I /usr/local/include pbzx.c -o /usr/local/bin/pbzx

WORKDIR /
RUN rm -rf /tmp/xar; rm -rf /tmp/pbzx

RUN mkdir /osx-toolchain; mkdir /home/haicam/workspace

RUN ln -s /home/haicam/toolchain/MacOSX12.3 /osx-toolchain/MacOSX12.3
RUN ln -s /home/haicam/toolchain/iPhoneOS15.4 /osx-toolchain/iPhoneOS15.4

RUN apt-get install -y libgmp-dev:i386 libc6-dev:i386 linux-libc-dev:i386

RUN apt-get install -y g++-mingw-w64-i686 g++-mingw-w64-x86-64
RUN update-alternatives --set i686-w64-mingw32-g++ /usr/bin/i686-w64-mingw32-g++-posix
RUN update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix

WORKDIR /home/haicam/workspace

RUN apt-get install -y gdb valgrind qemu-user gdb-multiarch

RUN apt-get update
RUN apt-get install -y libasound2-dev libpulse-dev yasm pkg-config

RUN apt-get install -y --no-install-recommends gcc-4.8 gcc-4.8-multilib g++-4.8 g++-4.8-multilib
RUN apt-get install -y libgtk-3-dev

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 30
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 30

RUN apt-get install -y nasm
RUN apt-get install -y autoconf libtool-bin