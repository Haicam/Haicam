#include "haicam/QRCodeScanner.hpp"
//#include "zbar.h"

using namespace haicam;

QRCodeScanner::QRCodeScanner(Context *context, int width, int height)
    : Runnable(context), width(width), height(height)
{
}

void QRCodeScanner::run()
{
    uv_thread_join(&m_thread1);
    free(pVideoData1);
    free(pvideoData2);
}

void QRCodeScanner::start()
{
    this->async.data = static_cast<void *>(this);
    uv_async_init(context->uv_loop, &this->async, QRCodeScanner::asyncCallback);
    uv_thread_create(&this->thread, QRCodeScanner::run, this);
}

void QRCodeScanner::asyncCallback(uv_async_t *handle)
{
    QRCodeScanner* thiz = static_cast<QRCodeScanner *>(handle->data);
    std::string str;
    while (thiz->callBackDataQ.dequeue(str))
    {
        if (thiz->onSuccessCallback != NULL)
        {
            thiz->onSuccessCallback(str);
        }
    }
}

void QRCodeScanner::run(void *data)
{
    QRCodeScanner *thiz = static_cast<QRCodeScanner *>(data);

    ByteBufferPtr frame;
    while (this->input.dequeueWait(frame))
    {
        this->sendDataOut(ByteBuffer::create("wifi:password"));
    }
}
