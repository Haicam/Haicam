#include "haicam/SSLClient.hpp"
#include "haicam/Utils.hpp"
#include "haicam/Encryption.hpp"

using namespace std::placeholders;

using namespace haicam;
using namespace haicam::Encryption;

SSLClient::SSLClient(Context *context, std::string serverIp, int serverPort, std::string serverPublicKey)
    :tcpClientPtr(TCPClient::create(context, serverIp, serverPort)), publicKey(serverPublicKey), frameNumber(0), context(context)
{
    tcpClientPtr->onConnectErrorCallback = std::bind(&SSLClient::onConnectError, this);
    tcpClientPtr->onConnectedCallback = std::bind(&SSLClient::onConnected, this, _1);  
    tcpClientPtr->onSentErrorCallback = std::bind(&SSLClient::onSentError, this, _1);
    tcpClientPtr->onSentCallback = std::bind(&SSLClient::onSent, this, _1);
    tcpClientPtr->onCloseCallback = std::bind(&SSLClient::onClose, this, _1);
    tcpClientPtr->onDataCallback = std::bind(&SSLClient::onData, this, _1, _2); 
}

SSLClient::~SSLClient()
{
}

void SSLClient::connect()
{
    tcpClientPtr->connect();
}

bool SSLClient::sendFrame(uint32 frameNum, uint8 cmd, uint8 cmdType, std::string payload, int encryptType, uint32 remoteAddr)
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
        strpayloadStr = EncodeRSAData(context->getServerRSAKey1024(), data2, 128);
    }
    else if (encryptType == FRAME_AES_1024)
    {
        strpayloadStr = EncodeAES(context->getAESKey1024(), data2);
    } else if (encryptType == FRAME_RSA_2048)
    {
        strpayloadStr = EncodeRSAData(context->getServerRSAKey2048(), data2, 256);
    } else if (encryptType == FRAME_AES_2048)
    {
        strpayloadStr = EncodeAES(context->getAESKey2048(), data2);
    }
    else if (encryptType == FRAME_PLAIN)
    {
        strpayloadStr = data2;
    }

    if (strpayloadStr == "") {
        return false;
    }
    
    data.append(Utils::uint32ToNetworkString((uint32)strpayloadStr.length()));
    data.append(strpayloadStr);
    uint8 checksum = 0xFF;
    
    for(int i = 1; i < data.length(); i ++){
        checksum ^= data[i];
    }
    data.append(1, checksum);

    return true;

}

bool SSLClient::sendRequest(uint8 cmd, std::string payload, int encryptType, uint32 remoteAddr)
{
    uint32 frameNum = frameNumber ++;
    return sendFrame(frameNum, cmd, FRAME_CMD_REQ, payload, encryptType, remoteAddr);
}

bool SSLClient::sendResponse(uint32 frameNum, uint8 cmd, std::string payload, int encryptType, uint32 remoteAddr)
{
    return sendFrame(frameNum, cmd, FRAME_CMD_RES, payload, encryptType, remoteAddr);
}

void SSLClient::sendData(ByteBufferPtr data)
{
    tcpClientPtr->sendData(data);
}

void SSLClient::close() {
    tcpClientPtr->close();
}

void SSLClient::onConnectError()
{

}

void SSLClient::onConnected(TCPConnectionPtr conn)
{

}

void SSLClient::onSSLConnected()
{

}

void SSLClient::onSentError(TCPConnectionPtr conn)
{

}

void SSLClient::onSent(TCPConnectionPtr conn)
{

}

void SSLClient::onClose(TCPConnectionPtr conn)
{

}

void SSLClient::onData(TCPConnectionPtr conn, ByteBufferPtr data)
{

}