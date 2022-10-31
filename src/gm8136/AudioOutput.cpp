
#include "haicam/platform/AudioOutput.hpp"

using namespace haicam::platform;

AudioOutput::AudioOutput()
{
}
AudioOutput::~AudioOutput()
{
}

bool AudioOutput::open()
{
    return true;
}

void AudioOutput::onData(void *data, int len)
{
}

void AudioOutput::close()
{
}