#include "gtest/gtest.h"
#include "haicam/TCPClient.hpp"

using namespace haicam;
using namespace std::placeholders;

void onDataCallback(TcpClientPtr tcp, TcpConnectionPtr conn , ByteBufferPtr data)
{

}

TEST(haicam_TCPClientTest, tcp_client_test) {
    Context *context = Context::getInstance();
    TcpClientPtr tcp = TCPClient::create(context, "127.0.0.1", 8888);
    tcp->onDataCallback = std::bind(onDataCallback, tcp, _1, _2);
    tcp->connect();
    tcp->close();
}
