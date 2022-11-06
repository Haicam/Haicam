#include "haicam/platform/Camera.hpp"
#include "haicam/Context.hpp"
#include "haicam/Utils.hpp"
#include "SDL2/SDL.h"

using namespace haicam::platform;

Camera::Camera()
{
}

Camera::~Camera()
{
     SDL_Quit();
}

void Camera::init(Context* context)
{
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        Utils::log("Unable to initialize SDL: %s", SDL_GetError());
        H_ASSERT_ERR_STR("Unable to initialize SDL");
    }
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