docker run -it --rm \
    --cpus=`nproc` \
    --user haicam:haicam \
    -v ~/go:/home/haicam/go \
    -v ~/toolchain:/home/haicam/toolchain:ro \
    -v `pwd`:/home/haicam/workspace \
    -v ~/Dropbox/test-libs/uv-cpp:/tmp/uv-cpp \
    haicam/haicam-toolchain:latest
