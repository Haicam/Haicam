#ifndef __HAICAM_UDP_HPP__
#define __HAICAM_UDP_HPP__
#include "haicam/Context.hpp"
#include "haicam/ByteBuffer.hpp"
#include <functional>

namespace haicam
{
    class UDP;
    typedef std::shared_ptr<UDP> UDPPtr;

    class UDP
    {
    private:
        struct ReqObj;

        static void allocReceiveBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

        static void onSocketClose(uv_handle_t *handle);

        static void onDataReceived(uv_udp_t *socket, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags);

        static void sentCallback(uv_udp_send_t *req, int err);

    protected:
        Context *context;
        UDP(Context *context, std::string bindIp, int bindPort);

    public:
        static UDPPtr create(Context *context, std::string bindIp = "0.0.0.0", int bindPort = 0);

        virtual ~UDP();

        void sendDataTo(ByteBufferPtr data, std::string ip, int port);

        void enableBroadcast();

        void enableMulticast(std::string multicastAddr);

        virtual void open();

        virtual void close();

    private:
        uv_udp_t socket;
        struct sockaddr_in addr;
        std::string multicastAddr;

    public:
        std::function<void()> onSentErrorCallback;
        std::function<void()> onSentCallback;
        std::function<void()> onCloseCallback;
        std::function<void(ByteBufferPtr data, std::string fromIP, int fromPort)> onDataCallback;
    };
}
#endif