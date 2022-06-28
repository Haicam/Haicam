#include "gtest/gtest.h"
#include "haicam/H264SensorSimulator.hpp"
#include "haicam/ByteBuffer.hpp"

using namespace haicam;
using namespace std::placeholders;

TEST(haicam_H264SensorSimulatorTest)
{
    Context *context = Context::getInstance();

   
   Printf("Start the H264sensorsimulator Test\n");

    context->run();
    delete context;
}
