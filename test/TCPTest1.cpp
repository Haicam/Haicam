#include "gtest/gtest.h"
#include "haicam/TCPServer.hpp"
#include "haicam/TCPClient.hpp"
#include "haicam/TCPConnection.hpp"
#include <iostream>

using namespace haicam;
using namespace std::placeholders;

int haicam_TCPTest1_callback_times = 0;

void haicam_TCPTest1_server_onSentCallback(TCPServer* server, TCPConnectionPtr conn)
{
	
    haicam_TCPTest1_callback_times ++;
   std::cout<<"1\n";
    
}
void haicam_TCPTest1_server_onSentErrorCallback(TCPServer* server, TCPConnectionPtr conn)
{
    haicam_TCPTest1_callback_times ++;
    std::cout<<"2\n";
    
}

void haicam_TCPTest1_server_onNewConnection(TCPServer* server, TCPConnectionPtr conn)
{
    haicam_TCPTest1_callback_times ++;
    std::cout<<"3\n";
}


void haicam_TCPTest1_server_onCloseCallback(TCPServer* server, TCPConnectionPtr conn)
{
    haicam_TCPTest1_callback_times ++;
    std::cout<<"4\n";
}

void haicam_TCPTest1_server_onDataCallback(TCPServer* server, TCPConnectionPtr conn, ByteBufferPtr data)
{
    std::string str = data->toString();
    ASSERT_EQ(str, "hello");
    conn->sendData(ByteBuffer::create("world"));

    haicam_TCPTest1_callback_times ++;
    std::cout<<"5\n";
}

void haicam_TCPTest1_client_onConnectedCallback(TCPClient* client, TCPConnectionPtr conn)
{
    conn->sendData(ByteBuffer::create("hello"));

    haicam_TCPTest1_callback_times ++;
    std::cout<<"6\n";
}

/*void haicam_TCPTest1_client_onSentErrorCallback(TCPClient* client, TCPConnectionPtr conn)
{

    haicam_TCPTest1_callback_times ++;
    std::cout<<"7\n";
}

/*void haicam_TCPTest1_client_onSentCallback(TCPClient* client, TCPConnectionPtr conn)
{
  
    haicam_TCPTest1_callback_times ++;
    std::cout<<"8\n";
}*/

void haicam_TCPTest1_client_onCloseCallback(TCPClient* client, TCPConnectionPtr conn)
{
    
    haicam_TCPTest1_callback_times ++;
    std::cout<<"9\n";
}


void haicam_TCPTest1_client_onDataCallback(TCPClient* client, TCPConnectionPtr conn, ByteBufferPtr data)
{
    std::string str = data->toString();
    ASSERT_EQ(str, "world");

    haicam_TCPTest1_callback_times ++;
    std::cout<<"10\n";

    client->close();
}

void haicam_TCPTest1_timeout(uv_timer_t *handle) {
    uv_close((uv_handle_t *)handle, NULL);

    TCPServer* server = (TCPServer*) handle->data;

    server->shutdown();

    haicam_TCPTest1_callback_times ++;
    std::cout<<"11\n";
}

TEST(haicam_TCPTest1, tcp_test) {
    Context *context = Context::getInstance();

    TCPServerPtr server = TCPServer::create(context, "127.0.0.1", 8888);
    server->onNewConnectionCallback = std::bind(haicam_TCPTest1_server_onNewConnection, server.get(), _1);
   //server->onSentErrorCallback = std::bind(haicam_TCPTest1_server_onSentErrorCallback, server.get(), _1);
    server->onDataCallback = std::bind(haicam_TCPTest1_server_onDataCallback, server.get(), _1, _2);
    server->onCloseCallback = std::bind(haicam_TCPTest1_server_onCloseCallback , server.get(), _1);
    server->onSentErrorCallback = std::bind(haicam_TCPTest1_server_onSentErrorCallback, server.get(), _1);
    server->onSentCallback = std::bind(haicam_TCPTest1_server_onSentCallback, server.get(), _1);
    server->listen();

    TCPClientPtr client = TCPClient::create(context, "127.0.0.1", 8888);
   client->onConnectedCallback = std::bind(haicam_TCPTest1_client_onConnectedCallback, client.get(), _1);
    //client->onSentErrorCallback = std::bind(haicam_TCPTest1_client_onSentErrorCallback, client.get(), _1);
    //client->onSentCallback = std::bind(haicam_TCPTest1_client_onSentCallback, client.get(), _1);
    client->onCloseCallback = std::bind(haicam_TCPTest1_client_onCloseCallback, client.get(), _1);
    client->onDataCallback = std::bind(haicam_TCPTest1_client_onDataCallback, client.get(), _1, _2);
    client->connect();

    uv_timer_t timer;
    
    timer.data = (void*) server.get();

    uv_timer_init(context->uv_loop, &timer);

    uv_timer_start(&timer, haicam_TCPTest1_timeout, 3000, 0);

    context->run();
    delete context;
