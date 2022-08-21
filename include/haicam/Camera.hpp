#pragma once
#include <mutex>
#include "haicam/Context.hpp"

namespace haicam
{
    class Camera
    {
    private:
        static std::mutex mtx;
        static Camera *instance;

        Context* context;
        
        Camera();

    public:
        ~Camera();
        static Camera* getInstance();

        void init(Context* context);
        void start();
        void stop();
    };
        
}