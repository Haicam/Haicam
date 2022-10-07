#include "gtest/gtest.h"
#include "haicam/platform/model/Led.hpp"

using namespace haicam::platform::model;

TEST(haicam_gm8136_e21_LedTest, status_test) {
    Led powerLed(haicam::LED_POWER);
    powerLed.on();
    EXPECT_EQ(haicam::LED_ON, powerLed.getStatus());
}