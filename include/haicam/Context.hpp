#ifndef __HAICAM_CONTEXT_HPP__
#define __HAICAM_CONTEXT_HPP__

#include <mutex>
#include <stdlib.h>

extern "C"
{
#include <uv.h>
}

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

namespace haicam
{
    class Context
    {
    public:
        static Context *getInstance();
        ~Context();

        int run();
        void stop();

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