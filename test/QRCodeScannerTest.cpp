#include "gtest/gtest.h"
#include "haicam/QRCodeScanner.hpp"
#include "haicam/ByteBuffer.hpp"

using namespace haicam;
using namespace std::placeholders;

void onWifiFound(std::weak_ptr<QRCodeScanner> scanner, ByteBufferPtr wifiConfig)
{
    printf("wifiConfig %s\n", wifiConfig->toString().c_str());

    QRCodeScannerPtr p = scanner.lock();
    if(p)p->stop();
}


void timeoutFunc(std::weak_ptr<QRCodeScanner> scanner)
{
    printf("timeoutFunc !!!!!!!!!!!!!!!!!!!!!!!!! \n");
}

TEST(haicam_QRCodeScannerTest, wifi_qrcode_test) {
    Context* context = Context::getInstance();

    H_ASSERT(context->uv_loop != NULL);

    QRCodeScannerPtr scanner = QRCodeScanner::create(context, 640, 480);

    scanner->onSuccessCallback = std::bind(onWifiFound, (std::weak_ptr<QRCodeScanner>) scanner, _1);

    scanner->onTimeoutCallback = std::bind(timeoutFunc, (std::weak_ptr<QRCodeScanner>) scanner);

    scanner->start(1000);

    for(int i = 0; i < 2; i ++)
    {
        int len = 1231;
        void* img = (void *)malloc(len);;
        ByteBufferPtr data = ByteBuffer::create(img, len);
        free(img);
        scanner->sendDataIn(data);
    }

    context->run();
    delete context;
}