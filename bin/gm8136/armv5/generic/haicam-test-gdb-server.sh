# https://azeria-labs.com/arm-on-x86-qemu-user/
qemu-arm -L /usr/src/arm-linux-3.3/toolchain_gnueabi-4.4.0_ARMv5TE/usr/arm-unknown-linux-uclibcgnueabi/sysroot \
 -g 10001 /home/haicam/workspace/bin/gm8136/armv5/generic/haicam-test