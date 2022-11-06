#include "haicam/platform/Camera.hpp"
#include "haicam/Utils.hpp"
#include "haicam/UserDefault.hpp"
extern "C" {
    #include "BoardMgr.h"
    #include "HardwareEncryption.h"
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
    //TODO
    //initGMSystem()

    // init isp328
    initAdvance();

    // init hardware AES128
    CInitEncodeCBCAES();

    //TODO
    //get mac address

    if(!Config::getInstance()->isDevelopment()) {
        if(get_flag_WTD_reboot()) {
            isStartedByUser = false;
        }
        if (UserDefault::getInstance()->getBoolForKey("update_firmware")) {
            isStartedByUser = true;
        }
        if (UserDefault::getInstance()->getBoolForKey("rebootNoSound")) {
            isStartedByUser = false;
        }
    }
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