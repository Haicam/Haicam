#ifndef __HAICAM_TCPCLIENT_HPP__
#define __HAICAM_TCPCLIENT_HPP__

#include "haicam/Context.hpp"
#include "haicam/ByteBuffer.hpp"
#include "haicam/TCPConnection.hpp"
#include <functional>

namespace haicam
{
    class TCPClient;
    typedef std::shared_ptr<TCPClient> TcpClientPtr;

    class TCPClient
    {
    private:
        TCPClient(Context *context, std::string serverIp, int serverPort)
            : context(context),
              connection(),
              client(),
              addr(),
              conntPtr(),
              onConnectErrorCallback(NULL),
              onConnectedCallback(NULL),
              onSentErrorCallback(NULL),
              onSentCallback(NULL),
              onCloseCallback(NULL),
              onDataCallback(NULL)
        {
            uv_tcp_init(context->uv_loop, &client);
            uv_tcp_nodelay(&client, 1 );
            uv_ip4_addr(serverIp.c_str(), serverPort, &addr);
        };

        static void onConnected(uv_connect_t *conn, int status)
        {
            TCPClient *thiz = static_cast<TCPClient *>(conn->data);
            if (status != 0) {
                if (thiz->onConnectErrorCallback != NULL) {
                    thiz->onConnectErrorCallback();
                }
            } else {
                thiz->conntPtr = TCPConnection::create(*(uv_stream_t*)conn->handle);
                thiz->conntPtr->onSentErrorCallback = thiz->onSentErrorCallback;
                thiz->conntPtr->onSentCallback = thiz->onSentCallback;
                thiz->conntPtr->onCloseCallback = thiz->onCloseCallback;
                thiz->conntPtr->onDataCallback = thiz->onDataCallback;
                thiz->conntPtr->readStart();
                if (thiz->onConnectedCallback != NULL) {
                    thiz->onConnectedCallback(thiz->conntPtr);
                }
            }
        }

    public:
        static TcpClientPtr create(Context *context, std::string serverIp, int serverPort)
        {
            return TcpClientPtr(new TCPClient(context, serverIp, serverPort));
        };
        ~TCPClient(){

        };

        void connect()
        {
            connection.data = static_cast<void *>(this);
            uv_tcp_connect(&connection, &client, (const struct sockaddr*) &addr, TCPClient::onConnected);
        }

        void close()
        {
            if (conntPtr.get() != NULL) {
                conntPtr->close();
            }
        }

    private:
        Context *context;
        uv_connect_t connection;
        uv_tcp_t client;
        struct sockaddr_in addr;
        TcpConnectionPtr conntPtr;

    public:
        std::function<void()> onConnectErrorCallback;
        std::function<void(TcpConnectionPtr)> onConnectedCallback;

        std::function<void(TcpConnectionPtr)> onSentErrorCallback;
        std::function<void(TcpConnectionPtr)> onSentCallback;
        std::function<void(TcpConnectionPtr)> onCloseCallback;
        std::function<void(TcpConnectionPtr, ByteBufferPtr)> onDataCallback;
    };
}
#endif