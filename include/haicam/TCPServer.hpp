#ifndef __HAICAM_TCPSERVER_HPP__
#define __HAICAM_TCPSERVER_HPP__

#include "haicam/Context.hpp"
#include "haicam/ByteBuffer.hpp"
#include "haicam/TCPConnection.hpp"
#include <functional>
#include <list>

namespace haicam
{
    class TCPServer;
    typedef std::shared_ptr<TCPServer> TCPServerPtr;

    class TCPServer
    {
    private:
        TCPServer(Context *context, std::string listenIp, int listenPort)
            : context(context),
              server(),
              addr(),
              onNewConnectionCallback(NULL),
              onSentErrorCallback(NULL),
              onSentCallback(NULL),
              onCloseCallback(NULL),
              onDataCallback(NULL)
        {
            uv_tcp_init(context->uv_loop, &server);
            uv_tcp_nodelay(&server, 1);
            uv_ip4_addr(listenIp.c_str(), listenPort, &addr);
        };

        static void onSocketClose(uv_handle_t *handle){

        };

        static void onNewConnection(uv_stream_t *serverConn, int status)
        {
            TCPServer *thiz = static_cast<TCPServer *>(serverConn->data);
            if (status != 0)
            {
                fprintf(stderr, "New connection error %s\n", uv_strerror(status));
                return;
            }
            else
            {
                uv_tcp_t* newConn = new uv_tcp_t();
                uv_tcp_init(thiz->context->uv_loop, newConn);
                uv_tcp_nodelay(newConn, 1);

                if (uv_accept(serverConn, (uv_stream_t *)newConn) == 0)
                {
                    struct sockaddr_storage addr;
                    int addrLen;
                    uv_tcp_getpeername(newConn, (sockaddr*)&addr, &addrLen);

                    char senderIP[17] = {0};
                    uv_ip4_name((const struct sockaddr_in *)&addr, senderIP, 16);

                    TCPConnectionPtr conntPtr = TCPConnection::create((uv_stream_t *)newConn);
                    conntPtr->onSentErrorCallback = thiz->onSentErrorCallback;
                    conntPtr->onSentCallback = thiz->onSentCallback;
                    conntPtr->onCloseCallback = thiz->onCloseCallback;
                    conntPtr->onDataCallback = thiz->onDataCallback;

                    conntPtr->remoteIP = senderIP;
                    conntPtr->remotePort = ntohs(((const struct sockaddr_in *)&addr)->sin_port);

                    conntPtr->createdByServer = true;

                    conntPtr->readStart();

                    thiz->connections.push_back(conntPtr);

                    if (thiz->onNewConnectionCallback != NULL)
                    {
                        thiz->onNewConnectionCallback(conntPtr);
                    }
                }
                else
                {
                    uv_close((uv_handle_t *)newConn, TCPServer::onSocketClose);
                    delete newConn;
                }
            }
        }

    public:
        static TCPServerPtr create(Context *context, std::string listenIp, int listenPort)
        {
            return TCPServerPtr(new TCPServer(context, listenIp, listenPort));
        };
        ~TCPServer(){

        };

        bool listen()
        {
            uv_tcp_bind(&server, (const struct sockaddr *)&addr, 0);

            server.data = static_cast<void *>(this);
            int r = uv_listen((uv_stream_t *)&server, 128, TCPServer::onNewConnection);
            if (r)
            {
                fprintf(stderr, "Listen error %s\n", uv_strerror(r));
                return false;
            }

            return true;
        }

        void shutdown()
        {
            if (uv_is_active((uv_handle_t *)&server))
            {
                uv_read_stop((uv_stream_t *)&server);
            }

            if (!uv_is_closing((uv_handle_t *)&server))
            {
                uv_close((uv_handle_t *)&server, TCPServer::onSocketClose);
            }

            for (auto itr = connections.begin(); itr != connections.end();)
            {
                (*itr)->close();
                itr = connections.erase(itr);
            }
        }

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