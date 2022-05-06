#ifndef __HAICAM_CONTEXT_HPP__
#define __HAICAM_CONTEXT_HPP__

extern "C" {
     #include <uv.h>
}

namespace haicam
{
    class Context
    {
    public:
        static Context* getInstance();
        ~Context();

        int run();
        void stop();

        uv_loop_t *uv_loop;
    private:
        Context();
        static Context* instance;
    };
}

#endif