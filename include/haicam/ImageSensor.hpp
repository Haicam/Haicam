#pragma once

#include "haicam/ByteBuffer.hpp"
#include <mutex>
#include <list>
#ifdef HAICAM_GM8136_GCC
#include <cstdatomic>
#else
#include <atomic>
#endif

namespace haicam
{

    class ImageObserver
    {
    public:
        virtual void onImage(ByteBufferPtr data, bool isKeyFrame) = 0;
    };

    class ImageSensor
    {
    public:
        ImageSensor();
        
        ~ImageSensor();

        void attach(std::shared_ptr<ImageObserver> observer);

        void detach(std::shared_ptr<ImageObserver> observer);

        void notify(ByteBufferPtr data, bool isKeyFrame);

        void start();

        static void process(void *arg);

        virtual void run() = 0;

        void stop();

    private:
        std::list<std::shared_ptr<ImageObserver>> listImageObserver;

        std::mutex mtx;
        uv_thread_t thread;

    protected:
        std::atomic<bool> isRunning;
    };

}