#include "gtest/gtest.h"
#include "haicam/QRCodeScanner.hpp"
#include "haicam/ByteBuffer.hpp"

using namespace haicam;
using namespace std::placeholders;

void onWifiFound(QRCodeScannerPtr scanner, ByteBufferPtr wifiConfig)
{
    scanner->stop();
    printf("wifiConfig %s\n", wifiConfig->toString().c_str());
}

void timeoutFunc(QRCodeScannerPtr scanner)
{

}

TEST(haicam_QRCodeScannerTest, wifi_qrcode_test) {
    Context* context = Context::getInstance();

    QRCodeScannerPtr scanner = QRCodeScanner::create(context, 640, 480);
    scanner->onSuccessCallback = std::bind(onWifiFound, scanner, _1);
    scanner->onTimeoutCallback = std::bind(timeoutFunc, scanner);

    scanner->start(2 * 60 * 1000);

    for(int i = 0; i < 10; i ++)
    {
        int len = 1231;
        void* img = (void *)malloc(len);;
        ByteBufferPtr data = ByteBuffer::create(img, len);
        free(img);
        scanner->sendDataIn(data);
    }
    
    context->run();
}