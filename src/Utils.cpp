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

uint32 Utils::networkStringToUint32(std::string buf,size_t offset){
    std::string strbuf = buf.substr(offset,4);
    uint32 ret = *(uint32*)strbuf.data();
    ret = ntohl(ret);
    return ret;
}

uint16 Utils::networkStringToUint16(std::string buf,size_t offset){
    std::string strbuf = buf.substr(offset,2);
    uint16 ret = *(uint16*)strbuf.data();
    ret = ntohs(ret);
    return ret;
}