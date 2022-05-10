#include "gtest/gtest.h"
#include "haicam/QRCodeScanner.hpp"
#include "haicam/ByteBuffer.hpp"

using namespace haicam;
using namespace std::placeholders;

uv_timer_t timer;

void onWifiFound(QRCodeScanner* scanner, std::string wifiConfig)
{
    uv_timer_stop(&timer);
    scanner->stop();
    printf("wifiConfig %s\n", wifiConfig.c_str());
}

void timeoutFunc(uv_timer_t *handle)
{
    uv_timer_stop(handle);
    QRCodeScanner* scanner = (QRCodeScanner*)handle->data;
    scanner->stop();
}

TEST(haicam_QRCodeScannerTest, wifi_qrcode_test) {
    Context* context = Context::getInstance();

    QRCodeScanner scanner(context, 640, 480);
    scanner.onSuccessCallback = std::bind(onWifiFound, &scanner, _1);

    uv_timer_init(context->uv_loop, &timer);
    timer.data = (void*)&scanner;
    uv_timer_start(&timer, timeoutFunc, 0, 30000);

    scanner.start();

    for(int i = 0; i < 10; i ++)
    {
        int len = 1231;
        void* img = (void *)malloc(len);;
        ByteBufferPtr data = ByteBuffer::create(img, len);
        free(img);
        scanner.scanImage(data);
    }
    
    context->run();
}