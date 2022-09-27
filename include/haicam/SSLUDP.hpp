#pragma once
#include "haicam/UDP.hpp"
#include "haicam/FrameCommand.hpp"
#include "haicam/Timer.hpp"
#include <map>
#include <list>

namespace haicam
{
    class SSLUDPListener
    {
    public:
        virtual void onSSLUDPRequest(uint8 cmd, std::string fromIP, int fromPort, const std::string &payload) = 0;
        virtual void onSSLUDPResponse(uint8 cmd, std::string fromIP, int fromPort, const std::string &payload) = 0;
        virtual void onSSLUDPRequestTimeout(uint8 cmd) = 0;
    };

    typedef std::shared_ptr<SSLUDPListener> SSLUDPListenerPtr;

    class SSLUDP : public UDP
    {
    private:
        std::list<SSLUDPListenerPtr> listeners;
        std::map<uint8, TimerPtr> requestTimers;

    public:
        SSLUDP(Context *context, std::string bindIp = "0.0.0.0", int bindPort = 0);
        ~SSLUDP();

        void addListener(SSLUDPListenerPtr listener);
        void removeListener(SSLUDPListenerPtr listener);

        bool sendFrame(uint8 cmd, uint8 cmdType, std::string &payload, uint8 encryptType, std::string &ip, int port);
        void onData(ByteBufferPtr data, std::string fromIP, int fromPort);

        bool sendRequest(uint8 cmd, std::string payload, std::string &ip, int port, uint8 encryptType = FRAME_RSA_2048, int timeoutMillSecs = 5000);
        bool sendResponse(uint8 cmd, std::string payload, std::string &ip, int port, uint8 encryptType = FRAME_RSA_2048, int timeoutMillSecs = 5000);

        void onRequest(uint8 cmd, std::string fromIP, int fromPort, const std::string &payload);
        void onResponse(uint8 cmd, std::string fromIP, int fromPort, const std::string &payload);
        void onRequestTimeout(Timer *timer, uint8 cmd, uint8 cmdType, std::string &payload, uint8 encryptType, std::string &ip, int port, uint64 timeoutByMillSecs);
    };
}