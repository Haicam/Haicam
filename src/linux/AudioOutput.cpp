
#include "haicam/platform/AudioOutput.hpp"
#include "haicam/Utils.hpp"

using namespace haicam::platform;

AudioOutput::AudioOutput()
{
    //SDL_Init in Camera::init()
    devId = 0;
}
AudioOutput::~AudioOutput()
{
    //SDL_Quit in ~Camera()
}

static void audioPlayCallback(void* userdata, Uint8* data, int len) 
{
    AudioOutput* thiz = (AudioOutput*) userdata;
    thiz->onData((void*)data, len);
}

void AudioOutput::open()
{
    if(devId) {
        Utils::log("SDL audio play device has already opened");
        return;
    }
    SDL_AudioSpec wanted;
    SDL_AudioSpec spec;
    SDL_zero(wanted);
    SDL_zero(spec);

    wanted.freq = 44100;
    wanted.format = AUDIO_F32SYS;
    wanted.channels = 1;
    wanted.samples = 4096;

    wanted.userdata = (void*)this;
    wanted.callback = audioPlayCallback;

    devId = SDL_OpenAudioDevice(NULL, SDL_FALSE, &wanted, &spec, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (!devId) {
        Utils::log("SDL couldn't open an audio play device for capture: %s!\n", SDL_GetError());
    }
}

void AudioOutput::onData(void *data, int len)
{
}

void AudioOutput::close()
{
    if(devId) SDL_CloseAudioDevice(devId);
}