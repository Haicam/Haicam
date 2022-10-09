#pragma once
#include <mutex>
#include "haicam/Context.hpp"
#include "haicam/Watchdog.hpp"
#include <memory>

namespace haicam
{
    class Camera
    {
    protected:
        static Camera* instance;
        Context* context;

        std::shared_ptr<Watchdog> watchdogPtr;



    public:
        Camera();
        virtual ~Camera();

        virtual void init(Context* context);
        static Camera* getInstance();

        virtual void start();
        virtual void stop();

        virtual void telnetOn();
        virtual void telnetOff();

        static void processSignal(int sig);
        virtual void registerSignal();
        virtual void upgradeFirmware();
        virtual void factoryDefault();

        virtual void startWatchdog();
    };
        
}