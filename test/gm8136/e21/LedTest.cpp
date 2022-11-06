#include "gtest/gtest.h"
#include "haicam/platform/model/Led.hpp"
#include "haicam/MacroDefs.hpp"
#include "haicam/platform/model/AudioInput.hpp"

using namespace haicam;

TEST(haicam_gm8136_e21_LedTest, status_test) {

    H_NEWN_SP(AudioInput, audioInput, platform::model::AudioInput());
    audioInput->open();

    platform::model::Led powerLed(haicam::LED_POWER);
    powerLed.on();
    EXPECT_EQ(haicam::LED_ON, powerLed.getStatus());
}