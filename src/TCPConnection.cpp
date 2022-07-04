#include "haicam/TCPConnection.hpp"
#include "haicam/TCPServer.hpp"
#include "haicam/TCPClient.hpp"
#include "haicam/Context.hpp"
#include "haicam/ByteBuffer.hpp"
#include <functional>
#include <iostream>

using namespace haicam;

TCPConnection::TCPConnection(uv_stream_t *conn)
    : conn(conn),
      onSentErrorCallback(NULL),
      onSentCallback(NULL),
      onCloseCallback(NULL),
      onDataCallback(NULL),
      remoteIP("0.0.0.0"),
      remotePort(0),
      server()
{
    conn->data = this;
}

void TCPConnection::allocReceiveBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = (char *)malloc(suggested_size);
#if _MSC_VER
    buf->len = (ULONG)suggested_size;
#else
    buf->len = suggested_size;
#endif
}

void TCPConnection::onSocketClose(uv_handle_t *handle)
{
    TCPConnection *thiz = static_cast<TCPConnection *>(handle->data);

    if (thiz->onCloseCallback != NULL)
    {
        thiz->onCloseCallback(thiz->getPtr());
    }

    TCPServerPtr serverPtr = thiz->server.lock();
    if (serverPtr.get() != NULL)
    {
        serverPtr->onConnectionClosed(thiz->getPtr());
    }
}

void TCPConnection::onDataReceived(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    if (nread < 0)
    {
        if (nread != UV_EOF)
            fprintf(stderr, "TCPConnection onDataReceived error %s\n", uv_err_name(nread));
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

void TCPConnection::sentCallback(uv_write_t *req, int err)
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
}

TCPConnectionPtr TCPConnection::create(uv_stream_t *conn)
{
    return TCPConnectionPtr(new TCPConnection(conn));
}

TCPConnection::~TCPConnection()
{
    if (server.use_count() != 0)
    {
        delete conn;
    }
}

void TCPConnection::readStart()
{
    uv_read_start(conn, TCPConnection::allocReceiveBuffer, TCPConnection::onDataReceived);
}

TCPConnectionPtr TCPConnection::getPtr()
{
    return shared_from_this();
}

void TCPConnection::sendData(ByteBufferPtr data)
{
    // Data are not copied over to the uv_buf_t, uv_buf_t.base refers to the same array of characters you used to create it.
    uv_buf_t buf = uv_buf_init(data->getDataPtr(), data->getLength());

    WriteReq *writeReq = new WriteReq();
    writeReq->thiz = this;
    writeReq->data = data; // data use_count + 1

    uv_write_t *req = new uv_write_t();
    req->data = static_cast<void *>(writeReq);

    uv_write(req, conn, &buf, 1, TCPConnection::sentCallback);
}

void TCPConnection::close()
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
