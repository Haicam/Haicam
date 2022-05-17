#include "gtest/gtest.h"
#include "haicam/Context.hpp"

using namespace haicam;

int haicam_ContextTest_callback_times = 0

void haicam_ContextTest_timeout(uv_timer_t *handle) {
    fprintf(stderr, "haicam_ContextTest_timeout\n");
    uv_close((uv_handle_t *)handle, NULL);

    haicam_ContextTest_callback_times ++;
}

TEST(haicam_ContextTest, context_test) {
    Context* context = Context::getInstance();

    EXPECT_EQ(true, context != NULL);
    EXPECT_EQ(true, context->uv_loop != NULL);

    uv_timer_t timer;
    uv_timer_init(context->uv_loop, &timer);

    uv_timer_start(&timer, haicam_ContextTest_timeout, 100, 0);

    context->run();
    delete context;

    EXPECT_EQ(haicam_ContextTest_callback_times, 1);
}