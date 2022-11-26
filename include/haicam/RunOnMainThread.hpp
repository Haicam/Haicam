#pragma once

#include "haicam/RunOnThread.hpp"

namespace haicam
{
    class RunOnMainThread : public RunOnThread
    {
    private:
        RunOnMainThread(Context *context);
        static RunOnMainThread *instance;

    public:
        static void initOnMainThread(Context *context);
        static RunOnMainThread *getInstance();
    };
}