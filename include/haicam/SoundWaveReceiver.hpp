#ifndef __HAICAM_SOUNDWAVERECEIVER_HPP__
#define __HAICAM_SOUNDWAVERECEIVER_HPP__

#include "haicam/Context.hpp"
#include "haicam/SafeQueue.hpp"
#include "haicam/ByteBuffer.hpp"

namespace haicam
{

    class SoundWaveReceiver
    {
    private:
        Context *context;
        SafeQueue<ByteBufferPtr> safeQueue;
        uv_thread_t thread;
        static void run(void* data);

    public:
        SoundWaveReceiver(Context *context);
        ~SoundWaveReceiver();

        void start();
        void stop();
        void pcmDataFeed(ByteBufferPtr data);
    };

}

#endif