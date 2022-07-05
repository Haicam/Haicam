#pragma once
#include "haicam/ImageSensor.hpp"
#include "haicam/Runnable.hpp"
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <list>
#include <memory>

namespace haicam
{

    class H264SensorSimulator : public ImageSensor
    {
    private:
        H264SensorSimulator();

    public:
        ~H264SensorSimulator();

        static std::shared_ptr<H264SensorSimulator> create()
        {
            return std::shared_ptr<H264SensorSimulator>(new H264SensorSimulator);
        };

        void run();
    };
}
