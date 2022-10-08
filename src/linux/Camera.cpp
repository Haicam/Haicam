#include "haicam/platform/Camera.hpp"
#include "haicam/Context.hpp"
#include "haicam/Utils.hpp"

using namespace haicam::platform;

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::init(Context* context)
{
    haicam::Camera::init(context);
}

void Camera::telnetOn()
{
    Utils::log("linux telnet on: not support");
}

void Camera::telnetOff()
{
    Utils::log("linux telnet off: not support");
}

void Camera::upgradeFirmware() 
{
    Utils::log("linux upgradeFirmware: not support");
}

void Camera::factoryDefault()
{
    Utils::log("linux factoryDefault: not support");
}

void Camera::startWatchdog()
{
    Utils::log("linux startWatchdog: not support");
}