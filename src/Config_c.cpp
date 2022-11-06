#include "haicam/Config.hpp"

extern "C" {
    #include "haicam/Config.h"
}

using namespace haicam;

const char* haicam_cfgGetSecurityDevice()
{
    return Config::getInstance()->getSecurityDevice().c_str();
}

const char* haicam_cfgGetShellFactoryDefault()
{
    return Config::getInstance()->getShellFactoryDefault().c_str();
}

const char* haicam_cfgGetMemDevice()
{
    return Config::getInstance()->getMemDevice().c_str();
}

const char* haicam_cfgGetIRCtrlDevice()
{
    return Config::getInstance()->getIRCtrlDevice().c_str();
}

const char* haicam_cfgGetISPDevice()
{
    return Config::getInstance()->getISPDevice().c_str();
}