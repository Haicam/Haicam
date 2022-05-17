#ifndef __HAICAM_TCPCONNECTION_HPP__
#define __HAICAM_TCPCONNECTION_HPP__

#include "haicam/Context.hpp"
#include "haicam/ByteBuffer.hpp"
#include <functional>

namespace haicam
{
    class TCPServer;
    typedef std::shared_ptr<TCPServer> TCPServerPtr;

    class TCPConnection;
    typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;

    class TCPConnection : public std::enable_shared_from_this<TCPConnection>
    {
    private:
        TCPConnection(uv_stream_t *conn);

        static void allocReceiveBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

        static void onSocketClose(uv_handle_t *handle);

        static void onDataReceived(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

        static void sentCallback(uv_write_t *req, int err);

        struct WriteReq
        {
            TCPConnection *thiz;
            ByteBufferPtr data;
        };

    public:
        static TCPConnectionPtr create(uv_stream_t *conn);

        ~TCPConnection();

        void readStart();

        TCPConnectionPtr getPtr();

        void sendData(ByteBufferPtr data);

        void close();

    private:
        uv_stream_t *conn;

    public:
        std::function<void(TCPConnectionPtr)> onSentErrorCallback;
        std::function<void(TCPConnectionPtr)> onSentCallback;
        std::function<void(TCPConnectionPtr)> onCloseCallback;
        std::function<void(TCPConnectionPtr, ByteBufferPtr)> onDataCallback;

        std::string remoteIP;
        int remotePort;
        std::weak_ptr<TCPServer> server;
    };
}
#endif