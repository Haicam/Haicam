#include "gtest/gtest.h"
#include "haicam/Context.hpp"
#include "haicam/platform/model/AudioInput.hpp"
#include "haicam/platform/model/Config.hpp"
#include "haicam/Utils.hpp"
#include "haicam/MacroDefs.hpp"

using namespace haicam;

// ./bin/linux/x86_64/generic/haicam-test --gtest_filter=haicam_sdl_test.audio_test
TEST(haicam_sdl_test, audio_test)
{
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        Utils::log("Unable to initialize SDL: %s", SDL_GetError());
        H_ASSERT_ERR_STR("Unable to initialize SDL");
    }

    H_NEWN_SP(Config, config, platform::model::Config());
    config->init();

    H_NEWN_SP(AudioInput, audioInput, platform::model::AudioInput());
    audioInput->open();

    while (1)
    {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            Utils::log("SDL_Event %i", e.type);
            if (e.type == SDL_QUIT) {// SDL_AUDIODEVICEADDED - 4352
                break;
            } 
        }
        SDL_Delay(16);
    }

    audioInput->close();

    SDL_Quit();
}