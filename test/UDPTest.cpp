#include "gtest/gtest.h"
#include "haicam/UDP.hpp"

using namespace haicam;
using namespace std::placeholders;

int haicam_UDPTest_callback_times = 0;

void haicam_UDPTest_onUdpDataCallback1(UDP* udp1, ByteBufferPtr data, std::string ip, int port)
{
    std::string str = data->toString();
    ASSERT_EQ(str, "hello");
    udp1->sendDataTo(ByteBuffer::create("world"), ip, port);
    udp1->close();

    haicam_UDPTest_callback_times ++;
}

void haicam_UDPTest_onUdpDataCallback2(UDP* udp2, ByteBufferPtr data, std::string ip, int port)
{
    std::string str = data->toString();
    ASSERT_EQ(str, "world");
    udp2->close();

    haicam_UDPTest_callback_times ++;
}

// ./bin/linux/x86_64/generic/haicam-test --gtest_filter=haicam_UDPTest.udp_test
// gdb --args ./bin/linux/x86_64/generic/haicam-test --gtest_filter=haicam_UDPTest.udp_test
TEST(haicam_UDPTest, udp_test) {
    Context *context = Context::getInstance();

    UDPPtr udp1 = UDP::create(context, "127.0.0.1", 9898);
    udp1->onDataCallback = std::bind(haicam_UDPTest_onUdpDataCallback1, udp1.get(), _1, _2, _3);
    udp1->open();

    UDPPtr udp2 = UDP::create(context);
    udp2->onDataCallback = std::bind(haicam_UDPTest_onUdpDataCallback2, udp2.get(), _1, _2, _3);
    udp2->open();


    udp2->sendDataTo(ByteBuffer::create("hello"), "127.0.0.1", 9898);


    context->run();
    delete context;

    ASSERT_EQ(haicam_UDPTest_callback_times, 2);

}
