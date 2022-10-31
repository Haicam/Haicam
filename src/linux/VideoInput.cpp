
#include "haicam/platform/VideoInput.hpp"
#include "haicam/Utils.hpp"

using namespace haicam::platform;

VideoInput::VideoInput()
{

}
VideoInput::~VideoInput()
{

}

bool VideoInput::open()
{
    if(isRunning) return true;

    isRunning = true;

    capture = H_MK_SP(cv::VideoCapture, (1));
    capture->set(cv::CAP_PROP_CONVERT_RGB, false); 

    
    uv_thread_create(&this->thread, VideoInput::process, this);

    return true;
}

void VideoInput::process(void *arg)
{
    VideoInput *thiz = static_cast<VideoInput *>(arg);
    thiz->run();

}

void VideoInput::onData(std::shared_ptr<uint8_t> data, int len)
{

}

void VideoInput::run()
{
    while (isRunning)
    {
        cv::Mat frame;
        *capture >> frame; 
        if (frame.empty())
        {
            uv_sleep(15);
            continue;
        }

        H_MEM_SP(uint8_t, new_data, frame.rows*frame.cols*2); // for YUYV

        if(frame.isContinuous()) {
            memcpy(new_data.get(), frame.ptr(0), frame.rows*frame.cols*2); 
        } else {
            uint8_t *p = new_data.get();
            for(int i  = 0; i < frame.rows; i++){
                memcpy(p, frame.ptr(i), frame.cols*2); 
                p += frame.cols * 2;
            }
        }

        mtx.lock();
        onData(new_data, frame.rows*frame.cols*2);
        mtx.unlock();

        uv_sleep(10);
    }
    
}

void VideoInput::close()
{
    if(isRunning) {
        isRunning = false;
        uv_thread_join(&this->thread);
        capture->release();
        capture.reset();
    }
}