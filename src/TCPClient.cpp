#include "haicam/TCPClient.hpp"
#include "haicam/TCPConnection.hpp"
#include "haicam/Context.hpp"
#include "haicam/ByteBuffer.hpp"
#include <functional>

using namespace haicam;

TCPClient::TCPClient(Context *context, std::string serverIp, int serverPort)
    : context(context),
      serverIp(serverIp),
      serverPort(serverPort),
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
    uv_tcp_nodelay(&client, 1);
    uv_ip4_addr(serverIp.c_str(), serverPort, &addr);
}

void TCPClient::onConnected(uv_connect_t *conn, int status)
{
    TCPClient *thiz = static_cast<TCPClient *>(conn->data);
    if (status != 0)
    {
        if (thiz->onConnectErrorCallback != NULL)
        {
            thiz->onConnectErrorCallback();
        }
    }
    else
    {
        thiz->conntPtr = TCPConnection::create((uv_stream_t *)conn->handle);
        thiz->conntPtr->onSentErrorCallback = thiz->onSentErrorCallback;
        thiz->conntPtr->onSentCallback = thiz->onSentCallback;
        thiz->conntPtr->onCloseCallback = thiz->onCloseCallback;
        thiz->conntPtr->onDataCallback = thiz->onDataCallback;

        thiz->conntPtr->remoteIP = thiz->serverIp;
        thiz->conntPtr->remotePort = thiz->serverPort;

        thiz->conntPtr->readStart();
        if (thiz->onConnectedCallback != NULL)
        {
            thiz->onConnectedCallback(thiz->conntPtr);
        }
    }
}

TCPClientPtr TCPClient::create(Context *context, std::string serverIp, int serverPort)
{
    return TCPClientPtr(new TCPClient(context, serverIp, serverPort));
}

TCPClient::~TCPClient()
{
}

void TCPClient::connect()
{
    connection.data = static_cast<void *>(this);
    uv_tcp_connect(&connection, &client, (const struct sockaddr *)&addr, TCPClient::onConnected);
}

void TCPClient::close()
{
    if (conntPtr.get() != NULL)
    {
        conntPtr->close();
    }
    else
    {
        if (uv_is_active((uv_handle_t *)&client))
        {
            uv_read_stop((uv_stream_t *)&client);
        }

        if (!uv_is_closing((uv_handle_t *)&client))
        {
            uv_close((uv_handle_t *)&client, NULL);
        }
    }
}
