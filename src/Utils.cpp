#include "haicam/Utils.hpp"
#include <netdb.h>
#include <arpa/inet.h>

using namespace haicam;

std::string Utils::getFastString(Json::Value value){
    Json::FastWriter writer;
    return writer.write(value);
}

Json::Value Utils::getJsonFromString(std::string str)
{
    Json::Reader reader;
    Json::Value value;
    if(reader.parse(str, value)){
        return value;
    }else{
        value.clear();
        return value ;
    }
}

std::string Utils::uint32ToNetworkString(uint32 value)
{
    std::string ret = "";
    value = htonl(value);
    ret.append(1,(value & 0xFF));
    ret.append(1,((value >> 8) & 0xFF));
    ret.append(1,((value >> 16) & 0xFF));
    ret.append(1,((value >> 24) & 0xFF));
    return ret;
}