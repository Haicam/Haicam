#ifndef ___HAICAM_GM8136_LED_HPP__
#define __HAICAM_GM8136_LED_HPP__

#include "haicam/Led.hpp"

namespace haicam
{
    namespace gm8136
    {
        class Led : public haicam::Led
        {
        public:
            Led(LedType type) : haicam::Led(type){

                                };

            void on();
            void off();
            void setStatus(LedStatus status);
            LedStatus getStatus();
        };
    }
}
#endif