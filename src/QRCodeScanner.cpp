#include "haicam/QRCodeScanner.hpp"
#include "zbar.h"

using namespace haicam;

QRCodeScanner::QRCodeScanner(Context *context, int width, int height)
    : Runnable(context), width(width), height(height)
{
}

void QRCodeScanner::run()
{
    ByteBufferPtr frame;
    while (this->input.dequeueWait(frame))
    {
        this->sendDataOut(ByteBuffer::create("wifi:password"));
    }
}
