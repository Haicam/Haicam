#include "haicam/SSLUDP.hpp"
#include "haicam/Encryption.hpp"
#include "haicam/Utils.hpp"

using namespace std::placeholders;

using namespace haicam;
using namespace haicam::Encryption;

SSLUDP::SSLUDP(Context *context, std::string bindIp, int bindPort)
    : UDP(context, bindIp, bindPort)
{
    this->onDataCallback = std::bind(&SSLUDP::onData, this, _1, _2, _3);
    ;
}

SSLUDP::~SSLUDP()
{
}

void SSLUDP::addListener(SSLUDPListenerPtr listener)
{
    listeners.push_back(listener);
}

void SSLUDP::removeListener(SSLUDPListenerPtr listener)
{
    listeners.remove(listener);
}

bool SSLUDP::sendFrame(uint8 cmd, uint8 cmdType, std::string &payload, uint8 encryptType, std::string &ip, int port)
{
    std::string data;
    if (encryptType == FRAME_PLAIN)
    {
        data.append(1, FRAME_CMD_SOF);
        data.append(1, encryptType);
        data.append(1, cmd);
        data.append(1, cmdType);
        data.append(Utils::uint32ToNetworkString((uint32)payload.length()));
        data.append(payload);
    }
    else if (encryptType == FRAME_RSA_1024)
    {
        data.append(1, FRAME_CMD_SOF);
        data.append(1, encryptType);
        data.append(1, cmd);
        data.append(1, cmdType);

        std::string strPayloadRSA = EncodeRSAData(context->getRSAKey1024(), payload, 128);

        data.append(Utils::uint32ToNetworkString((uint32)strPayloadRSA.length()));
        data.append(strPayloadRSA);
    }
    else if (encryptType == FRAME_RSA_2048)
    {
        std::string data2;
        data2.append(1, cmd);
        data2.append(1, cmdType);
        uint32 len = (uint32)payload.length();
        data2.append(Utils::uint32ToNetworkString(len));
        data2.append(payload);
        std::string strPayloadRSA = EncodeRSAData(context->getRSAKey2048(), data2, 256);

        data.append(1, FRAME_CMD_SOF);
        data.append(1, encryptType);
        data.append(Utils::uint32ToNetworkString((uint32)strPayloadRSA.length()));
        data.append(strPayloadRSA);
    }
    else
    {
        H_ASSERT_ERR_STR("not support");
    }

    uint8 checksum = 0xFF;
    for (int i = 1; i < data.length(); i++)
    {
        checksum ^= data[i];
    }

    data.append(1, checksum);

    this->sendDataTo(ByteBuffer::create(data), ip, port);
}

bool SSLUDP::sendRequest(uint8 cmd, std::string payload, std::string &ip, int port, uint8 encryptType, int timeoutMillSecs)
{
    uint64 timeoutByMillSecs = context->getCurrentMillSecs() + timeoutMillSecs;

    TimerPtr timer = Timer::create(context, 500, 500);// repeat every 0.5 second for unreliable UDP protocol
    timer->onTimeoutCallback = std::bind(&SSLUDP::onRequestTimeout, this, timer.get(), cmd, FRAME_UDP_CMD_REQ, payload, encryptType, ip, port, timeoutByMillSecs);
    requestTimers.insert({cmd, timer});

    timer->start();

    sendFrame(cmd, FRAME_UDP_CMD_REQ, payload, encryptType, ip, port);
}

bool SSLUDP::sendResponse(uint8 cmd, std::string payload, std::string &ip, int port, uint8 encryptType, int timeoutMillSecs)
{
    sendFrame(cmd, FRAME_UDP_CMD_RES, payload, encryptType, ip, port);
}

void SSLUDP::onRequestTimeout(Timer *timer, uint8 cmd, uint8 cmdType, std::string &payload, uint8 encryptType, std::string &ip, int port, uint64 timeoutByMillSecs)
{
    if(context->getCurrentMillSecs() < timeoutByMillSecs) {
        sendFrame(cmd, cmdType, payload, encryptType, ip, port);
    } else {
        timer->stop();
        requestTimers.erase(cmd);
        std::list<SSLUDPListenerPtr>::iterator l;
        for (l = listeners.begin(); l != listeners.end(); l++)
        {
            (*l)->onSSLUDPRequestTimeout(cmd);
        }
    }
}

void SSLUDP::onRequest(uint8 cmd, std::string fromIP, int fromPort, const std::string &payload)
{
    std::list<SSLUDPListenerPtr>::iterator l;
    for (l = listeners.begin(); l != listeners.end(); l++)
    {
        (*l)->onSSLUDPRequest(cmd, fromIP, fromPort, payload);
    }
}

void SSLUDP::onResponse(uint8 cmd, std::string fromIP, int fromPort, const std::string &payload)
{
    if (requestTimers.find(cmd) != requestTimers.end()) {
        requestTimers[cmd]->stop();
        requestTimers.erase(cmd);
    }

    std::list<SSLUDPListenerPtr>::iterator l;
    for (l = listeners.begin(); l != listeners.end(); l++)
    {
        (*l)->onSSLUDPResponse(cmd, fromIP, fromPort, payload);
    }
}

void SSLUDP::onData(ByteBufferPtr data, std::string fromIP, int fromPort)
{
    std::string buffer = data->toString();

    if ((uint8)buffer[0] != FRAME_CMD_SOF)
    {
        return;
    }

    uint8 encryptType = (uint8)buffer[1];

    std::string strframe;
    uint8 cmd = 0;
    uint8 type = 0;

    if (encryptType == FRAME_PLAIN)
    {
        uint32 len = Utils::networkStringToUint32(buffer, 4);
        if (buffer.length() < len + 8)
        {
            return;
        }

        uint8 checksum = 0xFF;
        for (int i = 1; i < len + 4; i++)
        {
            checksum ^= buffer[i];
        }

        if (checksum != (uint8)buffer[4 + len] || buffer.length() < len + 4 + 1)
        {
            return;
        }

        cmd = buffer[2];
        type = buffer[3];

        strframe = buffer.substr(8, len);
    }
    else if (encryptType == FRAME_RSA_1024)
    { // this logic should be only in the old camea firmware

        uint32 len = Utils::networkStringToUint32(buffer, 4);
        if (buffer.length() < len + 8)
        {
            return;
        }

        cmd = buffer[2];
        type = buffer[3];
        strframe = buffer.substr(8, len);

        strframe = DecodeRSAData(context->getRSAKey1024(), strframe, 128);
    }
    else if (encryptType == FRAME_RSA_2048)
    { // new camera logic
        uint32 len = Utils::networkStringToUint32(buffer, heardSize - 4);

        uint8 checksum = 0xFF;
        for (int i = 1; i < len + heardSize; i++)
        {
            checksum ^= buffer[i];
        }

        if (checksum != (uint8)buffer[heardSize + len] || buffer.length() < len + heardSize + 1)
        {
            return;
        }

        std::string strframe = buffer.substr(heardSize, len);
        strframe = DecodeRSAData(context->getRSAKey2048(), strframe, 256);

        cmd = strframe[0];
        type = strframe[1];
        strframe = strframe.substr(6);
    }
    else
    {
        return;
    }

    if (type == FRAME_UDP_CMD_REQ)
    {
        onRequest(cmd, fromIP, fromPort, strframe);
    }
    else if (type == FRAME_UDP_CMD_RES)
    {
        onResponse(cmd, fromIP, fromPort, strframe);
    }
    else
    {
        H_ASSERT_WARN_STR("Not supported type");
    }
}