#pragma once
#include "haicam/TCPClient.hpp"
#include "haicam/TCPConnection.hpp"
#include "haicam/FrameCommand.hpp"
#include "haicam/Timer.hpp"
#include <map>
#include <list>
#include "json/json.h"

namespace haicam
{
    class SSLTCPClientListener
    {
    public:
        virtual void onSSLTCPRequest(uint8 cmd, uint32 fromAddr, uint32 frameNum, const std::string &payload) = 0;
        virtual void onSSLTCPResponse(uint8 cmd, uint32 fromAddr, uint32 frameNum, const std::string &payload) = 0;
        virtual void onSSLTCPRequestTimeout(uint32 frameNum) = 0;
        virtual void onSSLTCPConnected() = 0;
    };

    typedef std::shared_ptr<SSLTCPClientListener> SSLTCPClientListenerPtr;

    class SSLTCPClient
    {
    private:
        static const int requestTimeoutMilliSecs = 10000;
        std::map<uint32, TimerPtr> requestTimers;

        TCPClientPtr tcpClientPtr;
        std::string publicKey;
        uint32 frameNumber; // from 1
        Context *context;
        std::string m_strlastBuffer;

        uint8 clientRandom[16];
        uint8 preMasterKey[16];
        uint8 masterKey[32]; // AES 256

        bool connected;

        std::list<SSLTCPClientListenerPtr> listeners;

        void sendData(ByteBufferPtr data);

    public:
        SSLTCPClient(Context *context, std::string serverIp, int serverPort, std::string serverPublicKey);
        virtual ~SSLTCPClient();

        void addListener(SSLTCPClientListenerPtr listener);
        void removeListener(SSLTCPClientListenerPtr listener);

        void connect();
        bool sendRequest(uint8 cmd, Json::Value payload, uint32 remoteAddr = FRAME_ADDR_SERVER, uint8 encryptType = FRAME_SSL_AES_256);
        bool sendRequest(uint8 cmd, std::string payload, uint32 remoteAddr = FRAME_ADDR_SERVER, uint8 encryptType = FRAME_SSL_AES_256);
        bool sendResponse(uint32 frameNum, uint8 cmd, std::string payload, uint32 remoteAddr = FRAME_ADDR_SERVER, uint8 encryptType = FRAME_SSL_AES_256);

        void onRequest(uint8 cmd, uint32 fromAddr, uint32 frameNum, const std::string &payload);
        void onResponse(uint8 cmd, uint32 fromAddr, uint32 frameNum, const std::string &payload);

        void onRequestTimeout(Timer *timer, uint32 frameNum);

        void close();

        void onConnectError();
        void onConnected(TCPConnectionPtr conn);
        void onSSLConnected();

        void onSentError(TCPConnectionPtr conn);
        void onSent(TCPConnectionPtr conn);
        void onClose(TCPConnectionPtr conn);
        void onData(TCPConnectionPtr conn, ByteBufferPtr data);
    };

}