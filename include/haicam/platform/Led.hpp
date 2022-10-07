#pragma once
#include "haicam/Led.hpp"

namespace haicam
{
    namespace platform
    {
        class Led : public haicam::Led
        {
        public:
            Led(LedType type):haicam::Led(type){};
        };
    }
}