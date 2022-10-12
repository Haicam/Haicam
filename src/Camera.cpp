#include "haicam/Camera.hpp"
#include "haicam/Config.hpp"
#include "haicam/Utils.hpp"
#include <signal.h>
#include "haicam/platform/model/Watchdog.hpp"
#include "haicam/UserDefault.hpp"

using namespace haicam;

Camera *Camera::instance = NULL;

Camera::Camera():isStartedByUser(true)
{
}

Camera::~Camera()
{
    instance = NULL;
}

void Camera::init(Context *context)
{
    if (this->context != NULL)
        return;

    this->context = context;

    instance = this;

    watchdogPtr = std::make_shared<platform::model::Watchdog>(context);
}

Camera *Camera::getInstance()
{
    H_ASSERT(instance != NULL);
    return instance;
}

void Camera::start()
{
    H_ASSERT(context != NULL);

    if (Config::getInstance()->isDevelopment())
    {
        this->telnetOn();
    }
    else
    {
        this->startWatchdog();
    }

    this->registerSignal();

    if (UserDefault::getInstance()->getBoolForKey("update_firmware"))
    {
        this->upgradeFirmware();
    }
}

void Camera::stop()
{
}

void Camera::telnetOn()
{
    Utils::log("Do not support Camera::telnetOn");
}

void Camera::telnetOff()
{
    Utils::log("Do not support Camera::telnetOfflnetOff");
}

void Camera::processSignal(int sig)
{
    if ((SIGTERM == sig) || (SIGINT == sig) || (SIGSEGV == sig))
    {
        exit(EXIT_FAILURE);
    }
}

void Camera::registerSignal()
{
    signal(SIGSEGV, Camera::processSignal);
    signal(SIGINT, Camera::processSignal);
    signal(SIGTERM, Camera::processSignal);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGILL, SIG_IGN);
}

void Camera::upgradeFirmware()
{
    Utils::log("Do not support Camera::upgradeFirmware");
}

void Camera::factoryDefault()
{
    Utils::log("Do not support Camera::factoryDefault");
}

void Camera::startWatchdog()
{
    if (watchdogPtr.get() == NULL)
        return;
    Utils::log("Do not support Camera::startWatchdog");
    watchdogPtr->start();
}