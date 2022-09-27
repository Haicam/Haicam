#include "haicam/SSLTCPClient.hpp"
#include "haicam/Utils.hpp"
#include "haicam/Encryption.hpp"
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

using namespace std::placeholders;

using namespace haicam;
using namespace haicam::Encryption;

SSLTCPClient::SSLTCPClient(Context *context, std::string serverIp, int serverPort, std::string serverPublicKey)
    : tcpClientPtr(TCPClient::create(context, serverIp, serverPort)), publicKey(serverPublicKey), frameNumber(1), context(context), m_strlastBuffer(""), connected(false), clientRandom(), preMasterKey(), masterKey()
{
    tcpClientPtr->onConnectErrorCallback = std::bind(&SSLTCPClient::onConnectError, this);
    tcpClientPtr->onConnectedCallback = std::bind(&SSLTCPClient::onConnected, this, _1);
    tcpClientPtr->onSentErrorCallback = std::bind(&SSLTCPClient::onSentError, this, _1);
    tcpClientPtr->onSentCallback = std::bind(&SSLTCPClient::onSent, this, _1);
    tcpClientPtr->onCloseCallback = std::bind(&SSLTCPClient::onClose, this, _1);
    tcpClientPtr->onDataCallback = std::bind(&SSLTCPClient::onData, this, _1, _2);
}

SSLTCPClient::~SSLTCPClient()
{
}

void SSLTCPClient::connect()
{
    tcpClientPtr->connect();
}

bool SSLTCPClient::sendFrame(uint32 frameNum, uint8 cmd, uint8 cmdType, std::string payload, uint8 encryptType, uint32 remoteAddr)
{
    std::string data;

    data.append(1, FRAME_CMD_SOF);
    data.append(1, encryptType);

    std::string strpayloadStr = "";

    std::string data2;
    data2.append(Utils::uint32ToNetworkString(frameNum));
    data2.append(Utils::uint32ToNetworkString(context->localAddr));
    data2.append(Utils::uint32ToNetworkString(remoteAddr));
    data2.append(1, cmd);
    data2.append(1, cmdType);
    uint32 len = (uint32)payload.length();
    data2.append(Utils::uint32ToNetworkString(len));

    data2.append(payload);

    if (encryptType == FRAME_RSA_1024)
    {
        strpayloadStr = EncodeRSAData(context->getRSAKey1024(), data2, 128);
    }
    else if (encryptType == FRAME_AES_128)
    {
        strpayloadStr = EncodeAES(context->getAESKey128(), data2);
    }
    else if (encryptType == FRAME_RSA_2048)
    {
        strpayloadStr = EncodeRSAData(context->getRSAKey2048(), data2, 256);
    }
    else if (encryptType == FRAME_AES_256)
    {
        strpayloadStr = EncodeAES(context->getAESKey256(), data2);
    }
    else if (encryptType == FRAME_SSL_RSA_2048)
    {
        strpayloadStr = EncodeRSAData(context->getServerRSAKey2048(), data2, 256, 1);
    }
    else if (encryptType == FRAME_SSL_AES_256)
    {
        strpayloadStr = EncodeAES(std::string((const char*)masterKey, 32), data2);
    }
    else if (encryptType == FRAME_PLAIN)
    {
        strpayloadStr = data2;
    }
    else
    {
        H_ASSERT_ERR_STR("Not supported encryption type");
    }

    if (strpayloadStr == "")
    {
        return false;
    }

    data.append(Utils::uint32ToNetworkString((uint32)strpayloadStr.length()));
    data.append(strpayloadStr);
    uint8 checksum = 0xFF;

    for (int i = 1; i < data.length(); i++)
    {
        checksum ^= data[i];
    }
    data.append(1, checksum);

    this->tcpClientPtr->sendData(ByteBuffer::create(data));

    return true;
}

bool SSLTCPClient::sendRequest(uint8 cmd, std::string payload, uint8 encryptType, uint32 remoteAddr)
{
    uint32 frameNum = frameNumber++;

    TimerPtr timer = Timer::create(context, requestTimeoutMilliSecs);
    timer->onTimeoutCallback = std::bind(&SSLTCPClient::onRequestTimeout, this, timer.get(), frameNum);
    timer->start();

    requestTimers.insert({frameNum, timer});

    return sendFrame(frameNum, cmd, FRAME_CMD_REQ, payload, encryptType, remoteAddr);
}

bool SSLTCPClient::sendResponse(uint32 frameNum, uint8 cmd, std::string payload, uint8 encryptType, uint32 remoteAddr)
{
    return sendFrame(frameNum, cmd, FRAME_CMD_RES, payload, encryptType, remoteAddr);
}

void SSLTCPClient::addListener(SSLTCPClientListenerPtr listener)
{
    listeners.push_back(listener);
    if(connected) {
        listener->onSSLTCPConnected();
    }
}

void SSLTCPClient::removeListener(SSLTCPClientListenerPtr listener)
{
    listeners.remove(listener);
}

void SSLTCPClient::onRequest(uint8 cmd, uint32 fromAddr, uint32 frameNum, const std::string &payload)
{
    std::list<SSLTCPClientListenerPtr>::iterator l;
    for (l = listeners.begin(); l != listeners.end(); l++)
    {
        (*l)->onSSLTCPRequest(cmd, fromAddr, frameNum, payload);
    }
}

void SSLTCPClient::onResponse(uint8 cmd, uint32 fromAddr, uint32 frameNum, const std::string &payload)
{
    if (requestTimers.find(frameNum) != requestTimers.end()) {
        requestTimers[frameNum]->stop();
        requestTimers.erase(frameNum);
    }

    if (cmd == FRAME_CMD_RSA_HANDSHAKE) {
        H_ASSERT(payload.length() == 17);

        uint8 status = payload[0];
        H_ASSERT(status == 1);

        std::string serverRandom = payload.substr(1);

        const char *key = "haicam.tech";
        unsigned char *result = NULL;
        unsigned int resultlen = -1;

        std::string data;
        data.append((const char*)clientRandom, sizeof(clientRandom));
        data.append(serverRandom);
        data.append((const char*)preMasterKey, sizeof(preMasterKey));

        result = HMAC(EVP_sha256(), key, strlen(key), (const unsigned char*)data.c_str(), data.length(), masterKey, &resultlen);

        H_ASSERT(result != NULL && resultlen == 32);

        onSSLConnected();

        return;
    }

    std::list<SSLTCPClientListenerPtr>::iterator l;
    for (l = listeners.begin(); l != listeners.end(); l++)
    {
        (*l)->onSSLTCPResponse(cmd, fromAddr, frameNum, payload);
    }
}

void SSLTCPClient::onRequestTimeout(Timer *timer, uint32 frameNum)
{
    timer->stop();
    requestTimers.erase(frameNum);

    std::list<SSLTCPClientListenerPtr>::iterator l;
    for (l = listeners.begin(); l != listeners.end(); l++)
    {
        (*l)->onSSLTCPRequestTimeout(frameNum);
    }
}

void SSLTCPClient::sendData(ByteBufferPtr data)
{
    tcpClientPtr->sendData(data);
}

void SSLTCPClient::close()
{
    tcpClientPtr->close();
}

void SSLTCPClient::onConnectError()
{
}

void SSLTCPClient::onConnected(TCPConnectionPtr conn)
{
    uint8 version = 1;
    uint8 type = FRAME_SSL_AES_256;
    RAND_priv_bytes(clientRandom, sizeof(clientRandom));
    RAND_priv_bytes(preMasterKey, sizeof(preMasterKey));

    std::string payload;
    payload.append(1, version);
    payload.append(1, type);
    payload.append((const char*)clientRandom, sizeof(clientRandom));
    payload.append((const char*)preMasterKey, sizeof(preMasterKey));
    

    sendRequest(FRAME_CMD_RSA_HANDSHAKE, payload, FRAME_SSL_RSA_2048);
}

void SSLTCPClient::onSSLConnected()
{
    connected = true;

    std::list<SSLTCPClientListenerPtr>::iterator l;
    for (l = listeners.begin(); l != listeners.end(); l++)
    {
        (*l)->onSSLTCPConnected();
    }
}

void SSLTCPClient::onSentError(TCPConnectionPtr conn)
{
}

void SSLTCPClient::onSent(TCPConnectionPtr conn)
{
}

void SSLTCPClient::onClose(TCPConnectionPtr conn)
{
}

void SSLTCPClient::onData(TCPConnectionPtr conn, ByteBufferPtr data)
{
    std::string buffer = data->toString();

    if (m_strlastBuffer.empty())
    {
        if ((uint8)buffer[0] != FRAME_CMD_SOF && (uint8)buffer[0] != FRAME_SIGN_HEART_BEAT)
        {
            return;
        }
    }
    else
    {
        buffer = m_strlastBuffer.append(buffer);
    }

    while (1)
    {
        if (buffer.size() > 0)
        {
            if ((uint8)buffer[0] == FRAME_SIGN_HEART_BEAT)
            {
                buffer = buffer.substr(1);
                continue;
            }
            if ((uint8)buffer[0] != FRAME_CMD_SOF)
            {
                m_strlastBuffer = "";
                return;
            }
        }

        if (buffer.length() < heardSize + 1)
        {
            break;
        }

        uint32 len = Utils::networkStringToUint32(buffer, heardSize - 4);

        H_ASSERT(len < 1 * 1024 * 1024);

        if (buffer.length() < len + heardSize + 1)
        {
            break;
        }

        uint8 checksum = 0xFF;
        for (int i = 1; i < len + heardSize; i++)
        {
            checksum ^= buffer[i];
        }

        if (checksum != (uint8)buffer[len + heardSize])
        {
            m_strlastBuffer = "";
            return;
        }

        std::string strframe = buffer.substr(heardSize, len);

        if ((uint8)buffer[1] == FRAME_RSA_1024)
        {
            strframe = DecodeRSAData(context->getRSAKey1024(), strframe, 128);
        }
        else if ((uint8)buffer[1] == FRAME_AES_128)
        {
            strframe = DecodeAES(context->getAESKey128(), strframe);
        }
        else if ((uint8)buffer[1] == FRAME_RSA_2048)
        {
            strframe = DecodeRSAData(context->getRSAKey2048(), strframe, 256);
        }
        else if ((uint8)buffer[1] == FRAME_AES_256)
        {
            strframe = DecodeAES(context->getAESKey256(), strframe);
        }else if ((uint8)buffer[1] == FRAME_SSL_RSA_2048)
        {
            strframe = DecodeRSAData(context->getServerRSAKey2048(), strframe, 256, 1);
        }
        else if ((uint8)buffer[1] == FRAME_SSL_AES_256)
        {
            strframe = DecodeAES(std::string((const char*)masterKey, 32), strframe);
        }
        else if ((uint8)buffer[1] == FRAME_PLAIN)
        {
            // plain data
        }
        else
        {
            H_ASSERT_WARN_STR("Not supported encryption type");
        }

        if (strframe.size() > heardSize2)
        {
            uint32 frameNum = Utils::networkStringToUint32(strframe, 0);
            uint32 uSRC = Utils::networkStringToUint32(strframe, 4);
            uint8 cmd = (uint8)strframe[heardSize2 - 6];
            uint8 type = (uint8)strframe[heardSize2 - 5];
            std::string payloadStr = strframe.substr(heardSize2, strframe.length() - heardSize2);

            if (type == FRAME_CMD_REQ)
            {
                onRequest(cmd, uSRC, frameNum, payloadStr);
            }
            else if (type == FRAME_CMD_RES)
            {
                onResponse(cmd, uSRC, frameNum, payloadStr);
            }
            else
            {
                H_ASSERT_WARN_STR("Not supported type");
            }
        }

        buffer = buffer.substr(len + heardSize + 1);
    }

    m_strlastBuffer = buffer;
}