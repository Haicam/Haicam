#include "gtest/gtest.h"
#include "SDL2/SDL.h"

TEST(haicam_sdl_test, audio_test)
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
        printf("SDL Err\n");
    }

    int i, count = SDL_GetNumAudioDevices(1);

    printf("SDL test: %i \n", count);

    for (i = 0; i < count; ++i)
    {
        SDL_Log("Audio device %d: %s", i, SDL_GetAudioDeviceName(i, 1));
    }
}