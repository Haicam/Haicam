#ifndef __HAICAM_CONTEXT_HPP__
#define __HAICAM_CONTEXT_HPP__

#include <string>
#include <mutex>
#include <stdlib.h>

extern "C"
{
#include <uv.h>
}

#define HAICAM_DEV 1

#define H_ASSERT(expr)                                         \
    do                                                         \
    {                                                          \
        if (!(expr))                                           \
        {                                                      \
            fprintf(stderr,                                    \
                    "Assertion failed in %s on line %d: %s\n", \
                    __FILE__,                                  \
                    __LINE__,                                  \
                    #expr);                                    \
            abort();                                           \
        }                                                      \
    } while (0)

#define H_ASSERT_ERR_STR(errStr)                     \
    do                                                         \
    {                                                          \
        if (true)                                           \
        {                                                      \
            fprintf(stderr,                                    \
                    "Assertion failed in %s on line %d: %s\n", \
                    __FILE__,                                  \
                    __LINE__,                                  \
                    errStr);                                    \
            abort();                                           \
        }                                                      \
    } while (0)

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