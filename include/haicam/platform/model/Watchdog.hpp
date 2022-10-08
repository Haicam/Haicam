#pragma once
#include "haicam/platform/Watchdog.hpp"
#include "haicam/Context.hpp"

namespace haicam
{
    namespace platform
    {
        namespace model
        {
            class Watchdog : public platform::Watchdog
            {
            public:
                Watchdog(Context *context) : platform::Watchdog(context)
                {
                }
            };
        }
    }
}