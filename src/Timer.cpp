#include "haicam/Timer.hpp"

using namespace haicam;

Timer::Timer(Context *context, int timeoutMillSecs, int intervalMillSecs)
    :timer(), timeoutMillSecs(timeoutMillSecs), intervalMillSecs(intervalMillSecs), onTimeoutCallback(NULL)
{
    uv_timer_init(context->uv_loop, &timer);
    
}

TimerPtr Timer::create(Context *context, int timeoutMillSecs, int intervalMillSecs)
{
    return TimerPtr(new Timer(context, timeoutMillSecs, intervalMillSecs));
}

void Timer::onTimeout(uv_timer_t *handle)
{
    Timer* thiz = static_cast<Timer*>(handle->data);
    if(thiz->onTimeoutCallback != NULL) {
        thiz->onTimeoutCallback();
    }
    if (thiz->intervalMillSecs == 0) {
        thiz->stop();
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