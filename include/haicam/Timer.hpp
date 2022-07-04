#pragma once

#include "haicam/Context.hpp"
#include <functional>
#include <memory>

namespace haicam
{
    class Timer;
    typedef std::shared_ptr<Timer> TimerPtr;

    class Timer
    {
    private:
        Timer(Context *context, int timeoutMillSecs, int intervalMillSecs);
    public:
        static TimerPtr create(Context *context, int timeoutMillSecs, int intervalMillSecs = 0);
        ~Timer();

        static void onTimeout(uv_timer_t *handle);
        static void onClose(uv_handle_t* handle){};

        void start();
        void stop();

    private:
        uv_timer_t timer;
        int timeoutMillSecs;
        int intervalMillSecs;
    public:
        std::function<void()> onTimeoutCallback;
    };
}