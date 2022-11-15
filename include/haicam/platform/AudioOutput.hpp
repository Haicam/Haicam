#pragma once
#include "haicam/AudioOutput.hpp"

#if defined(HAICAM_PLATFORM_LINUX)
#include "SDL2/SDL.h"
#endif

namespace haicam
{
    namespace platform
    {

        class AudioOutput : public haicam::AudioOutput
        {
        private:
#ifdef HAICAM_USE_SDL
            SDL_AudioDeviceID devId;
#endif
        public:
            AudioOutput();
            ~AudioOutput();
            bool open();
            void onData(void *data, int* len);
            void close();
        };
    }

}
