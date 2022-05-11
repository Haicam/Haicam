#ifndef __HAICAM_SOUNDWAVERECEIVER_HPP__
#define __HAICAM_SOUNDWAVERECEIVER_HPP__

#include "haicam/Runnable.hpp"

namespace haicam
{
    class SoundWaveReceiver;
    typedef std::shared_ptr<SoundWaveReceiver> SoundWaveReceiverPtr;

    class SoundWaveReceiver : public Runnable
    {
    private:
        SoundWaveReceiver(Context *context);

    protected:
        void run();

    public:
        static SoundWaveReceiverPtr create(Context *context)
        {
            return SoundWaveReceiverPtr(new SoundWaveReceiver(context));
        }
    };

}

#endif