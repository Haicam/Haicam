#pragma once
#include "haicam/Watchdog.hpp"
#include "haicam/Context.hpp"

namespace haicam
{
    namespace platform
    {
        class Watchdog: public haicam::Watchdog
        {
        public:
            Watchdog(Context* context);
            void enable();
            void disable();
            void update();
        };
    }
}