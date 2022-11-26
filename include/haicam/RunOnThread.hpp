#pragma once

#include "haicam/Context.hpp"
#include "haicam/SafeQueue.hpp"
#include <functional>

namespace haicam
{

    class RunOnThread
    {
    private:
        static void asyncCloseCallback(uv_handle_t *handle);
        static void asyncCallback(uv_async_t *handle);

    public:
        RunOnThread(Context *context);

        virtual ~RunOnThread();

        void startSchedule();

        // run on other thread
        void schedule(std::function<void()> callback);

        void stopSchedule();

    private:
        Context *context;
        SafeQueue<std::function<void()>> callbackQueue;
        uv_async_t async;

        // used in main thread only
        bool isStopped;
    };
}