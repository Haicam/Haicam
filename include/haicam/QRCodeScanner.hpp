#ifndef __HAICAM_QRCODESCANNER_HPP__
#define __HAICAM_QRCODESCANNER_HPP__

#include "haicam/Context.hpp"
#include "haicam/SafeQueue.hpp"

namespace haicam
{

    class QRCodeScanner
    {
    private:
        Context *context;

    public:
        QRCodeScanner(Context *context);
        ~QRCodeScanner();

        void start();
        static void run(void* arg);
        void stop();
        void scanImage(void *data, int width, int height);
    };

}

#endif