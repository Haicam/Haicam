#pragma once

#include "haicam/ByteBuffer.hpp"
#include <mutex>
#ifdef __GXX_EXPERIMENTAL_CXX0X__
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

        void Attach(std::shared_ptr<ImageObserver> observer);

        void Detach(std::shared_ptr<ImageObserver> observer);

        void Notify(ByteBufferPtr data, bool isKeyFrame);

        void start();

        static void process(void *arg);

        virtual run();

        void stop();

    private:
        std::list<std::shared_ptr<ImageObserver>> listImageObserver;

        std::mutex mtx;
        uv_thread_t thread;

    protected:
        std::atomic<bool> isRunning;
    };

    ImageSensor::ImageSensor() : listImageObserver(), mtx(), thread(), isRunning(false)
    {
    }

    ImageSensor::~ImageSensor()
    {
    }

    void ImageSensor::Attach(std::shared_ptr<ImageObserver> observer)
    {
        std::lock_guard<std::mutex> lock(mmtx);
        listImageObserver.push_back(observer);
    }

    void ImageSensor::Detach(std::shared_ptr<ImageObserver> observer)
    {
        std::lock_guard<std::mutex> lock(mmtx);
        listImageObserver.remove(observer);
    }

    void ImageSensor::Notify(ByteBufferPtr data, bool isKeyFrame)
    {
        std::lock_guard<std::mutex> lock(mmtx);

        std::list<IObserver *>::iterator iterator = listImageObserver.begin();
        while (iterator != listImageObserver.end())
        {
            (*iterator)->onImage(data, isKeyFrame);
            ++iterator;
        }
    }

    void ImageSensor::start()
    {
        isRunning = true;
        uv_thread_create(&this->thread, ImageSensor::process, this);
    }

    void ImageSensor::process(void *arg)
    {
        ImageSensor *thiz = static_cast<ImageSensor *>(arg);
        thiz->run();
    }

    void ImageSensor::stop()
    {
        isRunning = false;
        uv_thread_join(&this->thread);
    }

}