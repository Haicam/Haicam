#include "haicam/UDP.hpp"

using namespace haicam;

struct UDP::ReqObj
{
    UDP *thiz;
    ByteBufferPtr data;
};

UDP::UDP(Context *context, std::string bindIp, int bindPort)
    : context(context),
      socket(),
      addr(),
      multicastAddr(),
      onSentErrorCallback(NULL),
      onSentCallback(NULL),
      onCloseCallback(NULL),
      onDataCallback(NULL)
{
    uv_udp_init(context->uv_loop, &socket);
    socket.data = static_cast<void *>(this);

    uv_ip4_addr(bindIp.c_str(), bindPort, &addr);
};

void UDP::allocReceiveBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = (char *)malloc(suggested_size);
#if _MSC_VER
    buf->len = (ULONG)suggested_size;
#else
    buf->len = suggested_size;
#endif
};

void UDP::onSocketClose(uv_handle_t *handle)
{
    UDP *thiz = static_cast<UDP *>(handle->data);
    if (thiz->onCloseCallback != NULL)
        thiz->onCloseCallback();
}

void UDP::onDataReceived(uv_udp_t *socket, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags)
{
    if (nread < 0)
    {
        fprintf(stderr, "UDP onDataReceived error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t *)socket, UDP::onSocketClose);
    }
    else if (nread > 0)
    {
        char senderIP[17] = {0};
        uv_ip4_name((const struct sockaddr_in *)addr, senderIP, 16);
        std::string remoteIP = senderIP;
        int remotePort = ntohs(((const struct sockaddr_in *)addr)->sin_port);

        UDP *thiz = static_cast<UDP *>(socket->data);
        if (thiz->onDataCallback != NULL)
            thiz->onDataCallback(ByteBuffer::create(buf->base, nread), remoteIP, remotePort);
    }

    free(buf->base);
}

void UDP::sentCallback(uv_udp_send_t *req, int err)
{
    ReqObj *reqObj = static_cast<ReqObj *>(req->data);
    UDP *thiz = reqObj->thiz;
    if (err)
    {
        if (thiz->onSentErrorCallback != NULL)
            thiz->onSentErrorCallback();
    }
    else
    {
        if (thiz->onSentCallback != NULL)
            thiz->onSentCallback();
    }
    delete reqObj; // reqObj->data use_count - 1
    delete req;
};

UDPPtr UDP::create(Context *context, std::string bindIp, int bindPort)
{
    return UDPPtr(new UDP(context, bindIp, bindPort));
};

UDP::~UDP(){

};

void UDP::sendDataTo(ByteBufferPtr data, std::string ip, int port)
{
    struct sockaddr_in toAddr;
    uv_ip4_addr(ip.c_str(), port, &toAddr);

    // Data are not copied over to the uv_buf_t, uv_buf_t.base refers to the same array of characters you used to create it.
    uv_buf_t buf = uv_buf_init(data->getDataPtr(), data->getLength());

    ReqObj *reqObj = new ReqObj();
    reqObj->thiz = this;
    reqObj->data = data; // data use_count + 1

    uv_udp_send_t *req = new uv_udp_send_t();
    req->data = static_cast<void *>(reqObj);

    uv_udp_send(req, &socket, &buf, 1, (const struct sockaddr *)&toAddr, UDP::sentCallback);
}

void UDP::enableBroadcast()
{
    uv_udp_set_broadcast(&socket, 1);
}

void UDP::enableMulticast(std::string multicastAddr)
{
    this->multicastAddr = multicastAddr;
    uv_udp_set_multicast_loop(&socket, 0);
    uv_udp_set_multicast_ttl(&socket, 10);
    // uv_udp_set_multicast_interface(&socket, interfaceAddr);
    // uv_udp_set_membership(&socket, multicastAddr.c_str(), interfaceAddr.c_str(), UV_JOIN_GROUP);
    uv_udp_set_membership(&socket, multicastAddr.c_str(), NULL, UV_JOIN_GROUP);
}

void UDP::open()
{
    uv_udp_bind(&socket, (const struct sockaddr *)&addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(&socket, UDP::allocReceiveBuffer, UDP::onDataReceived);
}

void UDP::close()
{
    if (!this->multicastAddr.empty())
    {
        uv_udp_set_membership(&socket, multicastAddr.c_str(), NULL, UV_LEAVE_GROUP);
    }

    if (uv_is_active((uv_handle_t *)&socket))
    {
        uv_udp_recv_stop(&socket);
    }

    if (uv_is_closing((uv_handle_t *)&socket) == 0)
    {
        uv_close((uv_handle_t *)&socket, UDP::onSocketClose);
    }
}
