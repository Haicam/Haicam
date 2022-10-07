test -d ~/workspace/build/output/linux/x86_64/generic || mkdir -p ~/workspace/build/output/linux/x86_64/generic
cd ~/workspace/build/output/linux/x86_64/generic

#linux x86_64
cmake -DCMAKE_TOOLCHAIN_FILE=/home/haicam/workspace/build/toolchain/linux64.toolchain.cmake -DHAICAM_PLATFORM=linux -DHAICAM_ARCH=x86_64 -DHAICAM_TARGET=generic /home/haicam/workspace
make -j`nproc` && make install
cd ~/workspace