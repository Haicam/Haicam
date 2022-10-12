#pragma once
#include <string>
#include <stdio.h>
#include "haicam/MacroDefs.hpp"

namespace haicam
{
    class Config
    {
    public:
        H_CFG_VAR(bool, isDevelopment, true);
        H_CFG_VAR(std::string, getUserWritablePath, "");
        H_CFG_VAR(std::string, getSecurityDevice, "/dev/security");
        H_CFG_VAR(std::string, getWatchdogDevice, "/dev/watchdog");

        H_CFG_VAR(std::string, getMemDevice, "/dev/mem");
        H_CFG_VAR(std::string, getIRCtrlDevice, "/dev/sar_adc_drv");
        H_CFG_VAR(std::string, getISPDevice, "/dev/isp328");

        H_CFG_VAR(int, getWatchdogTimeout, 20);// 20 seconds
        H_CFG_VAR(int, getWatchdogTick, 3000);// 3 seconds

        H_CFG_VAR(std::string, getShellTelnetOn, "/usr/sbin/telnetd &");
        H_CFG_VAR(std::string, getShellTelnetOff, "killall telnetd");
        H_CFG_VAR(std::string, getShellUpgradeFirmware, "/app/upgrade.sh");
        H_CFG_VAR(std::string, getShellFactoryDefault, "/mnt/mtd/DevManager/factory_default.sh");

        void init()
        {
            instance = this;
        };
        static Config *getInstance()
        {
            H_ASSERT(instance != NULL);
            return instance;
        };

    protected:
        static Config *instance;
        
    };
}