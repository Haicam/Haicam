#include "haicam/platform/Camera.hpp"
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
    // TODO resetAccount();
    Utils::executeSystemCommand(Config::getInstance()->getShellFactoryDefault());
    Utils::log("Haicam Camera::factoryDefault");
}

void Camera::startWatchdog()
{

}