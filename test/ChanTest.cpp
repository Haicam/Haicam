#include "gtest/gtest.h"
#include "haicam/Context.hpp"
#include "haicam/Chan.hpp"
#include "haicam/ThreadWorker.hpp"

using namespace haicam;

Chan<int> chan(2);
Chan<int> chan2(0);

// ./bin/linux/x86_64/generic/haicam-test --gtest_filter=haicam_ChanTest.chan_test

TEST(haicam_ChanTest, chan_test)
{
    Context *context = Context::getInstance();

    printf("this is after work1 \n");

    ThreadWorker worker(context);

    static const char* s = "xxxxxxxx";

    FUNC(work, {
        printf("this is after work %s\n", s);
        sleep(2);
        printf("this is after work3 closed %i\n", chan.size());
        chan.closeAndClean();
        chan2.closeAndClean();
    });

    FUNC(after_work, {
        printf("this is after work4\n");
    });

    worker.run(work::bind())
        ->then(after_work::bind())
        ->start();

    chan << 1;
    chan << 1;
    chan << 1;

    chan2 << 1;
    chan2 << 1;

    context->run();
    delete context;
}