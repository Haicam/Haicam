#include "gtest/gtest.h"
#include "haicam/UDP.hpp"

using namespace haicam;
using namespace std::placeholders;

void onUdpDataCallback(UdpPtr udp, ByteBufferPtr data, std::string ip, int port)
{

}

TEST(haicam_UDPTest, udp_test) {
    Context *context = Context::getInstance();
    UdpPtr udp = UDP::create(context);
    udp->onDataCallback = std::bind(onUdpDataCallback, udp, _1, _2, _3);
    udp->open();
    udp->close();
}
