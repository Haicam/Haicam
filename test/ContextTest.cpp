#include "gtest/gtest.h"
#include "haicam/Context.hpp"

using namespace haicam;

TEST(haicam_ContextTest, context_test) {
    Context* context = Context::getInstance();

    EXPECT_EQ(true, context != NULL);
    EXPECT_EQ(true, context->uv_loop != NULL);
}