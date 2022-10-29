
#include "haicam/platform/AudioInput.hpp"
#include "haicam/Utils.hpp"

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

void AudioInput::open()
{
    if(devId) {
        Utils::log("SDL audio record device has already opened");
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
    wanted.callback = audioRecordCallback;


    devId = SDL_OpenAudioDevice(NULL, SDL_TRUE, &wanted, &spec, 0);
    if (!devId) {
        Utils::log("SDL couldn't open an audio record device for capture: %s!\n", SDL_GetError());
    }
}

void AudioInput::onData(void *data, int len)
{
}

void AudioInput::close()
{
    if(devId) SDL_CloseAudioDevice(devId);
}