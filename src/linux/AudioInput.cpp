
#include "haicam/platform/AudioInput.hpp"
#include "haicam/Utils.hpp"
#include "haicam/MacroDefs.hpp"

using namespace haicam::platform;

AudioInput::AudioInput()
{
    //SDL_Init in Camera::init()
    devId = 0;
}
AudioInput::~AudioInput()
{
    //SDL_Quit in ~Camera()
}

static void audioRecordCallback(void* userdata, Uint8* data, int len) 
{
    AudioInput* thiz = (AudioInput*) userdata;
    thiz->onData((void*)data, len);
}

bool AudioInput::open()
{
    if(devId) {
        Utils::log("SDL audio record device has already opened");
        return false;
    }
    SDL_AudioSpec wanted;
    SDL_AudioSpec spec;
    SDL_zero(wanted);
    SDL_zero(spec);

    wanted.freq = 16000;
    wanted.format = AUDIO_S16LSB;// AUDIO_S16SYS
    wanted.channels = 1;
    wanted.samples = 960;

    wanted.userdata = (void*)this;
    wanted.callback = audioRecordCallback;


    devId = SDL_OpenAudioDevice(NULL, SDL_TRUE, &wanted, &spec, 0);
    if (!devId) {
        Utils::log("SDL couldn't open an audio record device for capture: %s!\n", SDL_GetError());
        return false;
    }

    H_ASSERT(spec.format == wanted.format);

    Utils::log("freq: %i", spec.freq);
    Utils::log("format: %i", spec.format);
    Utils::log("channels: %i", spec.channels);
    Utils::log("samples: %i", spec.samples);

    Utils::log("SDL audio record device opened!\n");

    SDL_PauseAudioDevice(devId, SDL_FALSE);

    return true;
}

void AudioInput::onData(void *data, int len)
{
    Utils::log("len: %i", len); // samples * 2
}

void AudioInput::close()
{
    if(devId) SDL_CloseAudioDevice(devId);
}