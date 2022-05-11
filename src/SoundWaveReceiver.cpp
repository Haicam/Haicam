#include "haicam/SoundWaveReceiver.hpp"
#include "liquid/liquid.h"

using namespace haicam;

SoundWaveReceiver::SoundWaveReceiver(Context *context) : context(context)
{
}

SoundWaveReceiver::~SoundWaveReceiver()
{
}

void SoundWaveReceiver::start()
{
    uv_thread_create(&this->thread, SoundWaveReceiver::run, this);
}

void SoundWaveReceiver::run(void *data)
{
    SoundWaveReceiver* thiz = static_cast<SoundWaveReceiver*>(data);

    ByteBufferPtr frame;
    while (thiz->safeQueue.dequeueWait(frame))
    {
        /* sound wave analysis */
    }
}

void SoundWaveReceiver::stop()
{
    safeQueue.close();
    uv_thread_join(&this->thread);
}

void SoundWaveReceiver::pcmDataFeed(ByteBufferPtr data)
{
    safeQueue.enqueue(data);
}
