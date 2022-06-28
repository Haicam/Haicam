#include "haicam/H264SensorSimulator.hpp"

using namespace haicam;

H264SensorSimulator::H264SensorSimulator(Context *context) : Runnable(context)
{
}

void H264SensorSimulator::run()
{
    ByteBufferPtr frame;
    while (this->input.dequeueWait(frame))
    {
        this->sendDataOut(ByteBuffer::create("Data Simulation"));
    }
}
