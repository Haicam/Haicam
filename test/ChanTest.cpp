#include "gtest/gtest.h"
#include "haicam/Context.hpp"
#include "haicam/Chan.hpp"


using namespace haicam;

Chan<int> chan(2);
Chan<int> chan2(0);

void work(uv_work_t *req)
{
    printf("this is after work2\n");
    sleep(2);
    printf("this is after work3 closed %i\n", chan.size());
    chan.closeAndClean();
    chan2.closeAndClean();
}

void after_work(uv_work_t *req, int status)
{
    printf("this is after work4\n");
}

// ./bin/linux/x86_64/generic/haicam-test --gtest_filter=haicam_ChanTest.chan_test

TEST(haicam_ChanTest, chan_test)
{
    Context *context = Context::getInstance();

    uv_work_t req;

    printf("this is after work1 \n");

    uv_queue_work(context->uv_loop, &req, work, after_work);


    chan << 1;
    chan << 1;
    chan << 1;

    chan2 << 1;
    chan2 << 1;

    context->run();
    delete context;
}