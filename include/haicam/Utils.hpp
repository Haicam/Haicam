#pragma once
#include "json/json.h"
#include <string>
#include "haicam/FrameCommand.hpp"

namespace haicam
{
    class Utils
    {
    public:

        static std::string getFastString(Json::Value value);
        static Json::Value getJsonFromString(std::string str);
        static std::string uint32ToNetworkString(uint32 value);
    };
      
}