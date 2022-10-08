#pragma once
#include "haicam/Camera.hpp"

namespace haicam
{
    namespace platform
    {
        class Camera : public haicam::Camera
        {
        public:
            Camera();
            ~Camera();
            void init(Context* context);

            void telnetOn();
            void telnetOff();
            void upgradeFirmware();
            void factoryDefault();

            void startWatchdog();
        };
    }
}