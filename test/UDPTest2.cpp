#include "gtest/gtest.h"
#include "haicam/UDP.hpp"

using namespace haicam;
using namespace std::placeholders;

int err = 0, sent_cb = 0, close_cb = 0, data_cb = 0;

void haicam_UDPTest_onDataCallback(UDP *udp,ByteBufferPtr data,std::string ip,int port){
    std::string str = data->toString();
    ASSERT_EQ(str, "world");
    udp->sendDataTo(ByteBuffer::create("world"), ip, port);
    udp->close();
    data_cb++;
}

void haicam_UDPTest_onCloseCallback(){
    close_cb++;
	// To check close DataCallback
}

void haicam_UDPTest_onSentCallback(){
    sent_cb++;
	// To check close onSentCallback
}

void haicam_UDPTest_onSentErrorCallback(){
    err++;
}


TEST(haicam_UDPTest2, udp_test2){
	
	  Context *context = Context::getInstance();

   UDPPtr udp1 = UDP::create(context, "127.0.0.1", 9898);
   udp1->onDataCallback = std::bind(haicam_UDPTest_onDataCallback, udp1.get(), _1, _2, _3);
   udp1->onCloseCallback = std::bind(haicam_UDPTest_onCloseCallback);
   udp1->onSentCallback = std::bind(haicam_UDPTest_onSentCallback);
   udp1->onSentErrorCallback = std::bind(haicam_UDPTest_onSentErrorCallback);
   udp1->open();

   UDPPtr udp2 = UDP::create(context,"1271.0.0.1", 9898);
   udp2->onDataCallback = std::bind(haicam_UDPTest_onDataCallback, udp2.get(), _1, _2, _3);
   udp2->onCloseCallback = std::bind(haicam_UDPTest_onCloseCallback);
   udp2->onSentCallback = std::bind(haicam_UDPTest_onSentCallback);
   udp2->onSentErrorCallback = std::bind(haicam_UDPTest_onSentErrorCallback);
   udp2->open();
    
   udp2->sendDataTo(ByteBuffer::create("world"), "127.0.0.1", 9898);

   context->run();
   delete context;
    std::cerr << "sent_cb " << sent_cb << std::endl;
   std::cerr << "data_cb " << data_cb << std::endl;
   std::cerr << "close_cb " << close_cb << std::endl;
   std::cerr << "err " << err << std::endl;

   ASSERT_EQ(sent_cb, 3);
   ASSERT_EQ(data_cb, 2);
   ASSERT_EQ(close_cb ,2);
   ASSERT_EQ(err, 0);
}