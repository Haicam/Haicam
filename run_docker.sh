# docker exec -it haicam-docker bash
docker run -it --rm \
    --cpus=`nproc` \
    --user haicam:haicam \
    -v ~/go:/home/haicam/go \
    -v ~/toolchain:/home/haicam/toolchain:ro \
    -v `pwd`:/home/haicam/workspace \
    --name haicam-docker \
    haicam/haicam-toolchain:latest
