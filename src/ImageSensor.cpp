#include "haicam/ImageSensor.hpp"

using namespace haicam;

ImageSensor::ImageSensor() : listImageObserver(), mtx(), thread(), isRunning(false)
{
}

ImageSensor::~ImageSensor()
{
}

void ImageSensor::attach(std::shared_ptr<ImageObserver> observer)
{
    std::lock_guard<std::mutex> lock(mtx);
    listImageObserver.push_back(observer);
}

void ImageSensor::detach(std::shared_ptr<ImageObserver> observer)
{
    std::lock_guard<std::mutex> lock(mtx);
    listImageObserver.remove(observer);
}

void ImageSensor::notify(ByteBufferPtr data, bool isKeyFrame)
{
    std::lock_guard<std::mutex> lock(mtx);

    std::list<std::shared_ptr<ImageObserver>>::iterator iterator = listImageObserver.begin();
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