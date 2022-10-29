#include "gtest/gtest.h"
#include "haicam/H264SensorSimulator.hpp"
#include "haicam/ByteBuffer.hpp"
#include "haicam/RTSPServer.hpp"

#include "haicam/Timer.hpp"

using namespace haicam;

TEST(haicam_H264SensorSimulatorTest, run_test)
{
    Context *context = Context::getInstance();

    std::shared_ptr<H264SensorSimulator> h264Simulator = H264SensorSimulator::create();
    std::shared_ptr<RTSPServer> rtspServer = RTSPServer::create();
    TimerPtr timer = Timer::create(context, 3000);

    h264Simulator->attach(rtspServer);
    h264Simulator->start();


    H_FUNC(
        timeOut, (Timer * timer, std::shared_ptr<H264SensorSimulator> h264Simulator, std::shared_ptr<RTSPServer> rtspServer) {
            h264Simulator->detach(rtspServer);
            h264Simulator->stop();

            EXPECT_GT(rtspServer->frameReceived, 0);
        });
    
    timer->onTimeoutCallback = H_BINDV(timeOut, timer.get(), h264Simulator, rtspServer);
    timer->start();

    context->run();
    delete context;
}