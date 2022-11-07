#include "haicam/SoundWaveReceiver.hpp"

using namespace haicam;

SoundWaveReceiver::SoundWaveReceiver(Context *context) : Runnable(context)
{
}

void SoundWaveReceiver::run()
{
    ByteBufferPtr frame;
    while (this->input.dequeueWait(frame))
    {
        /* sound wave analysis */
        this->sendDataOut(ByteBuffer::create("sound wave analysis"));
    }
}