#ifndef __HAICAM_QRCODESCANNER_HPP__
#define __HAICAM_QRCODESCANNER_HPP__

#include "haicam/Runnable.hpp"

namespace haicam
{

    class QRCodeScanner;
    typedef std::shared_ptr<QRCodeScanner> QRCodeScannerPtr;

    class QRCodeScanner : public Runnable
    {
    private:
        int width;
        int height;

        QRCodeScanner(Context *context, int width, int height);

    protected:
        void run();

    public:
        static QRCodeScannerPtr create(Context *context, int width, int height) {
            return QRCodeScannerPtr(new QRCodeScanner(context, width, height));
        }
    };

}

#endif