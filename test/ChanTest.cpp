#include "gtest/gtest.h"
#include "haicam/Context.hpp"
#include "haicam/Chan.hpp"
#include "haicam/ThreadWorker.hpp"
#include "haicam/platform/model/Config.hpp"

using namespace haicam;
using namespace std::placeholders;

Chan<int> chan(2);
Chan<int> chan2(0);

// ./bin/linux/x86_64/generic/haicam-test --gtest_filter=haicam_ChanTest.chan_test

TEST(haicam_ChanTest, chan_test)
{
    Config* config = new platform::model::Config();
    config->init();

    Context *context = Context::getInstance();

    H_NEW_SP(ThreadWorker<int>, worker, (context));

    const char* s = "xxxxxxxx";

    H_TFUNC(int, work, (const char* str, ThreadWorker<int>* thiz){
        printf("work %s\n", str);
        sleep(2);
        printf("closed %i\n", chan.size());
        chan.closeAndClean();
        chan2.closeAndClean();

        if(thiz->running) {

        }

        //std::exception e;
        //throw e;

        return 1000;
    });

    H_FUNC(after_work, (int data){
        printf("after work %i\n", data);
    });

    H_FUNC(error, (int code){
        printf("after error %i\n", code);
    });

    worker->run(H_BINDV(work, s, worker.get()))
        ->then(H_BINDV(after_work, _1))
        ->error(H_BINDV(error, _1))
        ->start(100);

    chan << 1;
    chan << 1;
    chan << 1;

    chan2 << 1;
    chan2 << 1;

    context->run();

    delete context;
}