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
#ifdef HAICAM_USE_SDL
            SDL_AudioDeviceID devId;
#endif
        public:
            AudioInput();
            ~AudioInput();
            bool open();
            void onData(std::shared_ptr<uint8_t> pData, int len);
            void close();
        };
    }

}
