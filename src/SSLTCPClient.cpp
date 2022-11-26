#include "haicam/SSLTCPClient.hpp"
#include "haicam/Utils.hpp"
#include "haicam/Encryption.hpp"
#include "haicam/UserDefault.hpp"
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

bool SSLTCPClient::sendRequest(uint8 cmd, Json::Value payload, uint32 remoteAddr, uint8 encryptType)
{
    std::string payloadStr = Utils::getFastString(payload);
    return sendRequest(cmd, payloadStr, remoteAddr, encryptType);
}

bool SSLTCPClient::sendRequest(uint8 cmd, std::string payload, uint32 remoteAddr, uint8 encryptType)
{
    uint32 frameNum = frameNumber++;

    TimerPtr timer = Timer::create(context, requestTimeoutMilliSecs);
    timer->onTimeoutCallback = std::bind(&SSLTCPClient::onRequestTimeout, this, timer.get(), frameNum);
    timer->start();

    requestTimers.insert({frameNum, timer});

    std::string frame = packFrame(frameNum, cmd, FRAME_CMD_REQ, payload, remoteAddr, encryptType, std::string((const char *)masterKey, 32));

    if (!frame.empty())
    {
        sendData(ByteBuffer::create(frame));
        return true;
    }
    else
    {
        return false;
    }
}

bool SSLTCPClient::sendResponse(uint32 frameNum, uint8 cmd, std::string payload, uint32 remoteAddr, uint8 encryptType)
{
    std::string frame = packFrame(frameNum, cmd, FRAME_CMD_RES, payload, remoteAddr, encryptType, std::string((const char *)masterKey, 32));

    if (!frame.empty())
    {
        sendData(ByteBuffer::create(frame));
        return true;
    }
    else
    {
        return false;
    }
}

void SSLTCPClient::addListener(SSLTCPClientListenerPtr listener)
{
    listeners.push_back(listener);
    if (connected)
    {
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
    if (requestTimers.find(frameNum) != requestTimers.end())
    {
        requestTimers[frameNum]->stop();
        requestTimers.erase(frameNum);
    }

    if (cmd == FRAME_CMD_RSA_HANDSHAKE)
    {
        H_ASSERT(payload.length() == 17);

        uint8 status = payload[0];
        H_ASSERT(status == 1);

        std::string serverRandom = payload.substr(1);

        const char *key = "haicam.tech";
        unsigned char *result = NULL;
        unsigned int resultlen = -1;

        std::string data;
        data.append((const char *)clientRandom, sizeof(clientRandom));
        data.append(serverRandom);
        data.append((const char *)preMasterKey, sizeof(preMasterKey));

        result = HMAC(EVP_sha256(), key, strlen(key), (const unsigned char *)data.c_str(), data.length(), masterKey, &resultlen);

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
    payload.append((const char *)clientRandom, sizeof(clientRandom));
    payload.append((const char *)preMasterKey, sizeof(preMasterKey));

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

    uint8 cmd;
    uint8 cmdType;
    uint32 fromAddr;
    uint32 frameNum;
    std::string payload;

    while (unpackFrame(cmd, cmdType, fromAddr, frameNum, payload,
                       m_strlastBuffer, buffer, std::string((const char *)masterKey, 32)))
    {
        if (cmdType == FRAME_CMD_REQ)
        {
            onRequest(cmd, fromAddr, frameNum, payload);
        }
        else if (cmdType == FRAME_CMD_RES)
        {
            onResponse(cmd, fromAddr, frameNum, payload);
        }
        else
        {
            H_ASSERT_WARN_STR("Not supported type");
        }
    }
}