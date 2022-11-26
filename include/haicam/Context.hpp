#ifndef __HAICAM_CONTEXT_HPP__
#define __HAICAM_CONTEXT_HPP__

#include "haicam/Config.hpp"
#include <string>
#include <mutex>
#include <stdlib.h>

extern "C"
{
#include <uv.h>
}

namespace haicam
{
    class Context
    {
    public:
        static Context *getInstance();
        ~Context();

        int run();
        void stop();

        uint64_t getCurrentMillSecs();

    private:
        Context();

    public:
        uv_loop_t *uv_loop;

    private:
        static std::mutex mtx;
        static Context *instance;
        
    };
}

#endif