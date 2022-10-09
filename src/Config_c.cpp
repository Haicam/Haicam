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