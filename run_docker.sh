docker run -it --rm \
    --cpus=`nproc` \
    --user haicam:haicam \
    -v ~/go:/home/haicam/go \
    -v ~/toolchain:/home/haicam/toolchain \
    -v `pwd`:/home/haicam/workspace \
    haicam/haicam-toolchain:latest
