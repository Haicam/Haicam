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
        static TimerPtr create(Context *context, int timeoutMillSecs, int intervalMillSecs);
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
    
    Timer::Timer(Context *context, int timeoutMillSecs, int intervalMillSecs)
    :timer(), timeoutMillSecs(timeoutMillSecs), intervalMillSecs(intervalMillSecs), onTimeoutCallback(NULL)
    {
        uv_timer_init(context->uv_loop, &timer);
        
    }

    TimerPtr Timer::create(Context *context, int timeoutMillSecs, int intervalMillSecs = 0)
    {
        return TimerPtr(new Timer(context, timeoutMillSecs, intervalMillSecs));
    }

    void Timer::onTimeout(uv_timer_t *handle)
    {
        Timer* thiz = static_cast<Timer*>(handle->data);
        if(thiz->onTimeoutCallback != NULL) {
            thiz->onTimeoutCallback();
        }
    }

    void Timer::start()
    {
        timer.data = static_cast<void*>(this);
        uv_timer_start(&timer, Timer::onTimeout, timeoutMillSecs, intervalMillSecs);
    }

    void Timer::stop()
    {
        if (uv_is_active((uv_handle_t*)&timer))
        {
            uv_timer_stop(&timer);
        }

        if (uv_is_closing((uv_handle_t*)&timer) == 0)
        {
            uv_close((uv_handle_t *)&timer, Timer::onClose);
        }
        
    }
    
    Timer::~Timer()
    {
        // memory leak if uv_close here
    }
}