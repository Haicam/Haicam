#include "gtest/gtest.h"
#include "haicam/gm8136/Led.hpp"

using namespace haicam::gm8136;

TEST(haicam_gm8136_LedTest, status_test) {
    Led powerLed(haicam::LED_POWER);
    powerLed.on();
    EXPECT_EQ(haicam::LED_ON, powerLed.getStatus());
}