#include "gtest/gtest.h"
#include "haicam/Timer.hpp"

using namespace haicam;

int haicam_TimerTest_callback_times = 0;

void haicam_TimerTest_timeout(Timer *timer) {
    
    haicam_TimerTest_callback_times ++;
    if (haicam_TimerTest_callback_times == 3)
    {
        timer->stop();
    }
}

TEST(haicam_TimerTest, timer_test) {
    Context* context = Context::getInstance();

    TimerPtr timer = Timer::create(context, 100, 100);
    timer->onTimeoutCallback = std::bind(haicam_TimerTest_timeout, timer.get());
    timer->start();

    context->run();
    delete context;

    EXPECT_EQ(haicam_TimerTest_callback_times, 3);
}