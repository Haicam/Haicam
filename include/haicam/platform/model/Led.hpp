#pragma once
#include "haicam/platform/Led.hpp"

namespace haicam
{
    namespace platform
    {
        namespace model
        {
            class Led : public platform::Led
            {
            public:
                Led(LedType type):platform::Led(type) {};
            };
        }
    }
}