#pragma once
#include "haicam/Context.hpp"
#include "haicam/Timer.hpp"

namespace haicam
{
    class Watchdog
    {
    private:
        TimerPtr timer;

    protected:
        Context *context;

    public:
        Watchdog(Context* context);
        virtual ~Watchdog();
        virtual void start();
        virtual void stop();
        virtual void enable();
        virtual void disable();
        virtual void update();
    };
}