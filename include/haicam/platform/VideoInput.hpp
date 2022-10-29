#pragma once
#include "haicam/Context.hpp"
#include "haicam/MacroDefs.hpp"
#include "haicam/VideoInput.hpp"
#include <mutex>

#ifdef HAICAM_PLATFORM_GM8136
#include <cstdatomic>
#else
#include <atomic>
#endif

#ifdef HAICAM_PLATFORM_LINUX
#include <opencv2/opencv.hpp>
#endif

namespace haicam
{
    namespace platform
    {

        class VideoInput : public haicam::VideoInput
        {
        private:
            std::mutex mtx;
            uv_thread_t thread;
            std::atomic<bool> isRunning;

#ifdef HAICAM_PLATFORM_LINUX
            H_TP_PTR(cv::VideoCapture, capture);
#endif

            static void process(void *arg);
            void run();
        public:
            VideoInput();
            ~VideoInput();
            void open();
            void onData(std::shared_ptr<uint8_t> data, int len);
            void close();
        };
    }

}