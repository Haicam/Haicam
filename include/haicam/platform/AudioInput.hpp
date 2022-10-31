#pragma once
#include "haicam/AudioInput.hpp"

#if defined(HAICAM_PLATFORM_LINUX)
#include "SDL2/SDL.h"
#endif

namespace haicam
{
    namespace platform
    {

        class AudioInput : public haicam::AudioInput
        {
        private:
#if defined(HAICAM_PLATFORM_LINUX)
            SDL_AudioDeviceID devId;
#endif
        public:
            AudioInput();
            ~AudioInput();
            bool open();
            void onData(void *data, int len);
            void close();
        };
    }

}
