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

        std::string getRSAKey1024();
        std::string getRSAKey2048();
        std::string getAESKey256();
        std::string getAESKey128();

        std::string getServerRSAKey2048();

        uint64_t getCurrentMillSecs();
        
    private:
        Context();

    public:
        uv_loop_t *uv_loop;
        uint32_t localAddr;

    private:
        static std::mutex mtx;
        static Context *instance;
    };
}

#endif