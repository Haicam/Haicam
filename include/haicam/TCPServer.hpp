#ifndef __HAICAM_TCPSERVER_HPP__
#define __HAICAM_TCPSERVER_HPP__

#include "haicam/Context.hpp"
#include "haicam/ByteBuffer.hpp"
#include <functional>
#include <list>

namespace haicam
{
    class TCPServer;
    typedef std::shared_ptr<TCPServer> TCPServerPtr;

    class TCPConnection;
    typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;

    class TCPServer : public std::enable_shared_from_this<TCPServer>
    {
    private:
        TCPServer(Context *context, std::string listenIp, int listenPort);

        static void onSocketClose(uv_handle_t *handle);

        static void onNewConnection(uv_stream_t *serverConn, int status);

    public:
        static TCPServerPtr create(Context *context, std::string listenIp, int listenPort);

        TCPServerPtr getPtr();

        virtual ~TCPServer();

        virtual bool listen();

        virtual void onConnectionClosed(TCPConnectionPtr conntPtr);

        virtual void shutdown();

    private:
        Context *context;
        uv_tcp_t server;
        struct sockaddr_in addr;
        std::list<TCPConnectionPtr> connections;

    public:
        std::function<void(TCPConnectionPtr)> onNewConnectionCallback;
        std::function<void(TCPConnectionPtr)> onSentErrorCallback;
        std::function<void(TCPConnectionPtr)> onSentCallback;
        std::function<void(TCPConnectionPtr)> onCloseCallback;
        std::function<void(TCPConnectionPtr, ByteBufferPtr data)> onDataCallback;
    };
}
#endif