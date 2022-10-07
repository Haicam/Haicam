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