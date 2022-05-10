#ifndef __HAICAM_QRCODESCANNER_HPP__
#define __HAICAM_QRCODESCANNER_HPP__

#include "haicam/Context.hpp"
#include "haicam/SafeQueue.hpp"
#include "haicam/ByteBuffer.hpp"
#include <functional>

namespace haicam
{

    class QRCodeScanner
    {
    private:
        Context *context;
        int width;
        int height;
        SafeQueue<ByteBufferPtr> threadDataQ;
        SafeQueue<std::string> callBackDataQ;
        uv_async_t async;
        uv_thread_t thread;

        static void asyncCallback(uv_async_t *handle);
        static void run(void *data);

    public:
        QRCodeScanner(Context *context, int width, int height);
        ~QRCodeScanner();

        void start();
        void stop();
        void scanImage(ByteBufferPtr data);

        std::function<void(std::string)> onSuccessCallback;
    };

}

#endif