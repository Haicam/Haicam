#ifndef __HAICAM_TCPCONNECTION_HPP__
#define __HAICAM_TCPCONNECTION_HPP__

#include "haicam/Context.hpp"
#include "haicam/ByteBuffer.hpp"
#include <functional>

namespace haicam
{
    class TCPConnection;
    typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;

    struct WriteReq
    {
        TCPConnection *thiz;
        ByteBufferPtr data;
    };

    class TCPConnection : public std::enable_shared_from_this<TCPConnection>
    {
    private:
        TCPConnection(uv_stream_t* conn)
            : conn(conn),
              onSentErrorCallback(NULL),
              onSentCallback(NULL),
              onCloseCallback(NULL),
              onDataCallback(NULL),
              remoteIP("0.0.0.0"),
              remotePort(0),
              createdByServer(false)
        {
            conn->data = this;
        };

        static void allocReceiveBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
        {
            buf->base = (char *)malloc(suggested_size);
#if _MSC_VER
            buf->len = (ULONG)suggested_size;
#else
            buf->len = suggested_size;
#endif
        };

        static void onSocketClose(uv_handle_t *handle)
        {
            TCPConnection *thiz = static_cast<TCPConnection *>(handle->data);
            if (thiz->onCloseCallback != NULL)
                thiz->onCloseCallback(thiz->getPtr());
        }

        static void onDataReceived(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
        {
            if (nread < 0)
            {
                if (nread != UV_EOF) fprintf(stderr, "TCPConnection onDataReceived error %s\n", uv_err_name(nread));
                uv_close((uv_handle_t *)stream, TCPConnection::onSocketClose);
            }
            else if (nread > 0)
            {
                TCPConnection *thiz = static_cast<TCPConnection *>(stream->data);
                if (thiz->onDataCallback != NULL)
                    thiz->onDataCallback(thiz->getPtr(), ByteBuffer::create(buf->base, nread));
            }

            free(buf->base);
        }

        static void sentCallback(uv_write_t *req, int err)
        {
            WriteReq *writeReq = static_cast<WriteReq *>(req->data);
            TCPConnection *thiz = writeReq->thiz;
            if (err)
            {
                if (thiz->onSentErrorCallback != NULL)
                    thiz->onSentErrorCallback(thiz->getPtr());
            }
            else
            {
                if (thiz->onSentCallback != NULL)
                    thiz->onSentCallback(thiz->getPtr());
            }
            delete writeReq; // writeReq->data use_count - 1
            delete req;
        };

    public:
        static TCPConnectionPtr create(uv_stream_t* conn)
        {
            return TCPConnectionPtr(new TCPConnection(conn));
        };
        ~TCPConnection(){
            if(createdByServer) 
            { 
                delete conn;
            }
        };

        void readStart() 
        {
            uv_read_start(conn, TCPConnection::allocReceiveBuffer, TCPConnection::onDataReceived);
        }

        TCPConnectionPtr getPtr()
        {
            return shared_from_this();
        };

        void sendData(ByteBufferPtr data)
        {
            // Data are not copied over to the uv_buf_t, uv_buf_t.base refers to the same array of characters you used to create it.
            uv_buf_t buf = uv_buf_init(data->getData(), data->getLength());

            WriteReq *writeReq = new WriteReq();
            writeReq->thiz = this;
            writeReq->data = data; // data use_count + 1

            uv_write_t* req = new uv_write_t();
            req->data = static_cast<void*>(writeReq);

            uv_write(req, conn, &buf, 1, TCPConnection::sentCallback);
        }

        void close()
        {
            if (uv_is_active((uv_handle_t *)conn))
            {
                uv_read_stop(conn);
            }

            if (!uv_is_closing((uv_handle_t *)conn))
            {
                uv_close((uv_handle_t *)conn, TCPConnection::onSocketClose);
            }
        }

    private:
        uv_stream_t* conn;

    public:
        std::function<void(TCPConnectionPtr)> onSentErrorCallback;
        std::function<void(TCPConnectionPtr)> onSentCallback;
        std::function<void(TCPConnectionPtr)> onCloseCallback;
        std::function<void(TCPConnectionPtr, ByteBufferPtr)> onDataCallback;

        std::string remoteIP;
        int remotePort;
        bool createdByServer;
    };
}
#endif