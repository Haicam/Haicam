#pragma once
#include "haicam/ImageSensor.hpp"
#include "haicam/Runnable.hpp"
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

        void run();
    };

    H264SensorSimulator::H264SensorSimulator() : ImageSensor()
    {
    }

    std::shared_ptr<H264SensorSimulator> create()
    {
        return std::shared_ptr<H264SensorSimulator>(new H264SensorSimulator);
    }

    H264SensorSimulator::~H264SensorSimulator()
    {
    }

    void H264SensorSimulator::run()
    {
    }
}
