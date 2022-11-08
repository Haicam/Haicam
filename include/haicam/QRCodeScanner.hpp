#ifndef __HAICAM_QRCODESCANNER_HPP__
#define __HAICAM_QRCODESCANNER_HPP__

#include "haicam/Runnable.hpp"

namespace zbar {
    class ImageScanner;
}

namespace haicam
{

    class QRCodeScanner;

    typedef std::shared_ptr<QRCodeScanner> QRCodeScannerPtr;

    class QRCodeScanner : public Runnable
    {
    private:
        int width;
        int height;

        zbar::ImageScanner* scanner;

        QRCodeScanner(Context *context, int width, int height);
        void scanImage(ByteBufferPtr pData);

    protected:
        void run();

    public:
        ~QRCodeScanner();
        static QRCodeScannerPtr create(Context *context, int width, int height) {
            return QRCodeScannerPtr(new QRCodeScanner(context, width, height));
        }
    };

}

#endif