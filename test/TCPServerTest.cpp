#include "gtest/gtest.h"
#include "haicam/TCPServer.hpp"

using namespace haicam;
using namespace std::placeholders;

void onNewConnection(TcpServerPtr server, TcpConnectionPtr conn)
{

}

TEST(haicam_TCPServerTest, tcp_server_test) {
    Context *context = Context::getInstance();
    TcpServerPtr tcp = TCPServer::create(context, "127.0.0.1", 8888);
    tcp->onNewConnectionCallback = std::bind(onNewConnection, tcp, _1);
    tcp->listen();
    tcp->close();
}
