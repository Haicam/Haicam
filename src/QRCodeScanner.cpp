#include <cstddef>
#include "zbar.h"
#include "haicam/QRCodeScanner.hpp"

#include <memory>

using namespace haicam;

QRCodeScanner::QRCodeScanner(Context *context, int width, int height)
    : Runnable(context), width(width), height(height)
{
    scanner = new zbar::ImageScanner();
    scanner->set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
}

QRCodeScanner::~QRCodeScanner()
{
    delete scanner;
}

void QRCodeScanner::run()
{
    ByteBufferPtr data;
    while (this->input.dequeueWait(data))
    {
        scanImage(data);
    }
}

void QRCodeScanner::scanImage(ByteBufferPtr pData)
{
    char* data = pData->getDataPtr();

    zbar::Image image(width, height, "Y800", data, width * height);
    int n = scanner->scan(image);

    if (n > 0) {
        for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end();  ++symbol) {
            std::string str = symbol->get_data();
            this->sendDataOut(ByteBuffer::create(str));
        }
    }
    image.set_data(NULL, 0);
}
