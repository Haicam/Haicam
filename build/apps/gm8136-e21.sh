test -d ~/workspace/build/output/gm8136/armv5/e21 || mkdir -p ~/workspace/build/output/gm8136/armv5/e21
cd ~/workspace/build/output/gm8136/armv5/e21

#gm8136 armv5
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/arm-gm8136.toolchain.cmake -DHAICAM_PLATFORM=gm8136 -DHAICAM_ARCH=armv5 -DHAICAM_TARGET=e21 /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace