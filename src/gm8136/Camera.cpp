#include "haicam/platform/Camera.hpp"
#include "haicam/Utils.hpp"
extern "C" {
    #include "BoardMgr.h"
}

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
    Board_Init();
}

void Camera::telnetOn()
{
    Utils::executeSystemCommand(Config::getInstance()->getShellTelnetOn());
    Utils::log("Haicam telnet on");
}

void Camera::telnetOff()
{
    Utils::executeSystemCommand(Config::getInstance()->getShellTelnetOff());
    Utils::log("Haicam telnet off");
}

void Camera::upgradeFirmware()
{
    if (Utils::checkIsFileExsit(Config::getInstance()->getShellUpgradeFirmware()))
        Utils::executeSystemCommand(Config::getInstance()->getShellUpgradeFirmware());
    else
        Utils::executeSystemCommand("/app/upgrate.sh");
    Utils::log("Haicam upgradeFirmware");
}

void Camera::factoryDefault()
{
    Utils::log("Haicam Camera::factoryDefault");
    Factory_Default();
}

void Camera::startWatchdog()
{

}