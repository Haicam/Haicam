#pragma once
#include <string.h>
#include <stdarg.h>
#include <sstream>
#include "json/json.h"

namespace haicam
{
    class Utils
    {
    public:
        static std::string getFastString(Json::Value value);
        static Json::Value getJsonFromString(std::string str);
        static std::string uint32ToNetworkString(uint32_t value);

        static uint32_t networkStringToUint32(std::string buf, size_t offset);
        static uint16_t networkStringToUint16(std::string buf, size_t offset);

        static bool checkIsFileExsit(std::string fullPath);
        static void makeDir(std::string dir);

        static uint64_t getMillTimestmap();

        static void log_va(const char *format, va_list args);
        static void log(const char *format, ...);

        template<typename T>
        static std::string toStr(T v);

        template<typename T>
        static T fromStrTo(std::string str);

        static std::string uuidV4();

        static std::string executeSystemCommand(std::string command);
    };


    /*
    templated classes/functions cannot be separated as a header + implementation file 
    */
    template <typename T>
    std::string Utils::toStr(T v)
    {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }

    template <typename T>
    T Utils::fromStrTo(std::string str)
    {
        T value(0);
        std::istringstream iss(str);
        iss >> value;
        return value;
    }

}