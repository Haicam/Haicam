#include "haicam/H264SensorSimulator.hpp"

using namespace haicam;

H264SensorSimulator::H264SensorSimulator() : ImageSensor()
{

}

void H264SensorSimulator::run()
{
    struct __attribute__((__packed__))  
    {
        long long millTimestamp;
        bool isKeyframe;
        int length;
    } frameHeader;

    long long lastFrameTime = -1;
    
    while (isRunning)
    {
        //fread((void*) &frameHeader, sizeof(frameHeader), 1, fp);
        // fake data
        frameHeader.length = 1024;
        frameHeader.millTimestamp = 1000;
        lastFrameTime = 0;

        if (lastFrameTime > -1 && frameHeader.millTimestamp > lastFrameTime) {
            uv_sleep(frameHeader.millTimestamp - lastFrameTime);   
        }

        lastFrameTime = frameHeader.millTimestamp;

        ByteBufferPtr frame = ByteBuffer::create(frameHeader.length);
        frame->getDataPtr();
        //fread((void*) data->getDataPtr(), 1, frameHeader.length, fp);
        notify(frame, frameHeader.isKeyframe);
    }
}

H264SensorSimulator::~H264SensorSimulator()
{
    
}
