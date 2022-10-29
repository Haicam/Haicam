test -d /tmp/openh264 && rm -rf /tmp/openh264
cd /tmp
git clone https://github.com/cisco/openh264.git
cd openh264
git checkout v2.3.1

#linux x86_64
#make CFLAGS='-fPIE' -j`nproc` OS=linux ARCH=x86_64
make -j`nproc` OS=linux ARCH=x86_64 PREFIX=/home/haicam/workspace/libs/openh264/linux/x86_64
make PREFIX=/home/haicam/workspace/libs/openh264/linux/x86_64 install-headers
make PREFIX=/home/haicam/workspace/libs/openh264/linux/x86_64 install-static-lib 
make PREFIX=/home/haicam/workspace/libs/openh264/linux/x86_64 install-static
