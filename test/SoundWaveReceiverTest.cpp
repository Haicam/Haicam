#include "gtest/gtest.h"
#include "haicam/SoundWaveReceiver.hpp"
#include "haicam/ByteBuffer.hpp"

using namespace haicam;
using namespace std::placeholders;

TEST(haicam_SoundWaveReceiverTest, wifi_soundwave_test)
{
    Context *context = Context::getInstance();

    ASSERT_TRUE(context != NULL);

    SoundWaveReceiverPtr soundWaveReceiver = SoundWaveReceiver::create(context);
    soundWaveReceiver->start(1000);
    for (int i = 0; i < 10; i++)
    {
        soundWaveReceiver->sendDataIn(ByteBuffer::create("PCM Raw data"));
    }
    soundWaveReceiver->stop();

    context->run();
    delete context;
}