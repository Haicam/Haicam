
#include "haicam/platform/VideoInput.hpp"

using namespace haicam::platform;

VideoInput::VideoInput()
{
}
VideoInput::~VideoInput()
{
}

bool VideoInput::open()
{
    return true;
}

void VideoInput::onData(std::shared_ptr<uint8_t> data, int len)
{
}

void VideoInput::close()
{
}