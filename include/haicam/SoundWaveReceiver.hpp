#ifndef __HAICAM_SOUNDWAVERECEIVER_HPP__
#define __HAICAM_SOUNDWAVERECEIVER_HPP__

#include "haicam/Context.hpp"
#include "haicam/SafeQueue.hpp"

namespace haicam
{

    class SoundWaveReceiver
    {
    private:
        Context *context;

    public:
        SoundWaveReceiver(Context *context);
        ~SoundWaveReceiver();

        void start();
        static void run(void* arg);
        void stop();
        void pcmDataFeed(void *data, int length);
    };

}

#endif