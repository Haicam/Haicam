#pragma once
#include "haicam/TCPClient.hpp"
#include "haicam/TCPConnection.hpp"
#include "haicam/FrameCommand.hpp"

namespace haicam
{
    class SSLClient
    {
    private:
        TCPClientPtr tcpClientPtr;
        std::string publicKey;
        uint32 frameNumber;
        Context *context;

        bool sendFrame(uint32 frameNum, uint8 cmd, uint8 cmdType, std::string payload, int encryptType = FRAME_AES_2048, uint32 remoteAddr = FRAME_ADDR_SERVER);
        void sendData(ByteBufferPtr data);

    public:
        SSLClient(Context *context, std::string serverIp, int serverPort, std::string serverPublicKey);
        ~SSLClient();

        void connect();
        bool sendRequest(uint8 cmd, std::string payload, int encryptType = FRAME_AES_2048, uint32 remoteAddr = FRAME_ADDR_SERVER);
        bool sendResponse(uint32 frameNum, uint8 cmd, std::string payload, int encryptType = FRAME_AES_2048, uint32 remoteAddr = FRAME_ADDR_SERVER);
        
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