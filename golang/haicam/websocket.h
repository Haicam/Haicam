#ifndef HAICAM_WEBSOCKET_H
#define HAICAM_WEBSOCKET_H

#ifndef HAICAM_WINDOWS_DEF
#define HAICAM_WINDOWS_DEF
#ifdef _WIN32
#ifdef _WIN64
#define HAICAM_WINDOWS_64
#else
#define HAICAM_WINDOWS_32
#endif
#endif
#endif

#ifndef addr_t
#define addr_t long long unsigned int
#endif

#ifdef __cplusplus
#include <string>
#include <map>
#include <mutex>
extern "C"
{
#endif

    extern void onWebSocketData(void *cObjRef, long long unsigned int goObjRef, void *data, int length);

#ifdef __cplusplus
}

extern void *haicam_go_p2p_so_handle;

namespace haicam
{

    class WebSocketDelegate
    {
    public:
        virtual void onWebSocketData(long long unsigned int goObjRef, void *data, int length) = 0;
    };

    class WebSocket
    {
    public:
        static WebSocket *getInstance();

        WebSocketDelegate *delegate;

        void start(std::string listenAddr);
        void sendData(long long unsigned int goObjRef, std::string data);

        void sendDataToAll(std::string data);
        void addOrUpdateSocket(long long unsigned int socket);
        void removeSocket(long long unsigned int socket);

        std::string getDirectoryUsage(std::string diretory);
        std::string getGetExePath();

    private:
        std::recursive_mutex mMutex;
        std::map<long long unsigned int, int> socketMap;

        WebSocket();
    };

}

#endif

#endif
