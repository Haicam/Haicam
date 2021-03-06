#ifndef __HAICAM_TCPCLIENT_HPP__
#define __HAICAM_TCPCLIENT_HPP__

#include "haicam/Context.hpp"
#include "haicam/ByteBuffer.hpp"
#include <functional>

namespace haicam
{
    class TCPClient;
    typedef std::shared_ptr<TCPClient> TCPClientPtr;

    class TCPConnection;
    typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;

    class TCPClient
    {
    private:
        TCPClient(Context *context, std::string serverIp, int serverPort);

        static void onConnected(uv_connect_t *conn, int status);

    public:
        static TCPClientPtr create(Context *context, std::string serverIp, int serverPort);
        ~TCPClient();

        void connect();

        void close();

    private:
        Context *context;
        std::string serverIp;
        int serverPort;
        uv_connect_t connection;
        uv_tcp_t client;
        struct sockaddr_in addr;
        TCPConnectionPtr conntPtr;

    public:
        std::function<void()> onConnectErrorCallback;
        std::function<void(TCPConnectionPtr)> onConnectedCallback;

        std::function<void(TCPConnectionPtr)> onSentErrorCallback;
        std::function<void(TCPConnectionPtr)> onSentCallback;
        std::function<void(TCPConnectionPtr)> onCloseCallback;
        std::function<void(TCPConnectionPtr, ByteBufferPtr)> onDataCallback;
    };
}
#endif