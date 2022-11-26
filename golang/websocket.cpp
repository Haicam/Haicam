#include "haicam/websocket.h"
#include "libhaicam_ext.h"
#include <sys/time.h>

#ifdef _WIN32
#define localtime_r(T,Tm) (localtime_s(Tm,T) ? NULL : Tm)
#endif

#ifdef HAICAM_WINDOWS_32
#include "dlfcn.h"
#endif

using namespace haicam;

extern "C" {

static void _log(const char* buf) 
{
#if Debug
#ifdef __ANDROID__
    char out[500];// < android logcat max line length
    size_t len = strlen(buf);
    bool firstTime = true;
    for(int i = 0; i < len;)
    {
        memset(out, 0, 500);
        if(len - i < 500) {
            memcpy(out, buf + i, len - i);
            i += len - i;
        } else {
            memcpy(out, buf + i, 499);
            i += 499;
        }
        
        if(firstTime) {
            printf("Golog: %s\n", out);
            firstTime = false;
        } else {
            printf("Golog: <<%s\n",out);
        }
    }
#else
    char dtime[100];
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    time_t now = time (0);
    int milli = curTime.tv_usec / 1000;
    struct tm timeInfo;
    localtime_r(&now,&timeInfo);
    strftime (dtime, 100, "%Y-%m-%d %H:%M:%S", &timeInfo);
    printf("%s.%d Golog: %s\n",dtime,milli, buf);
#endif
#endif
}

void onWebSocketData(void* cObjRef, long long unsigned int goObjRef, void* data, int length)
{
    ((WebSocketDelegate*)cObjRef)->onWebSocketData(goObjRef, data, length);
}

}// end extern "C"


typedef void (*T_WebSocketStart)(char*, void*);
typedef void (*T_SendWebSocketData)(long long unsigned int, void*, int);
typedef char* (*T_DirectoryUsage)(char*);
typedef char* (*T_GetExePath)();

T_WebSocketStart t_WebSocketStart = NULL;
T_SendWebSocketData t_SendWebSocketData = NULL;
T_DirectoryUsage t_DirectoryUsage = NULL;
T_GetExePath t_GetExePath = NULL;

WebSocket::WebSocket() {

//move to java System.loadLibrary("haicam_ext").  As it causes react native freeze
#if defined(__ANDROID__xxxxx) || defined(HAICAM_WINDOWS_32)
    t_WebSocketStart = (T_WebSocketStart)dlsym(haicam_go_p2p_so_handle, "WebSocketStart");
    t_SendWebSocketData = (T_SendWebSocketData)dlsym(haicam_go_p2p_so_handle, "SendWebSocketData");
    t_DirectoryUsage = (T_DirectoryUsage)dlsym(haicam_go_p2p_so_handle, "DirectoryUsage");
    t_GetExePath = (T_GetExePath)dlsym(haicam_go_p2p_so_handle, "GetExePath");

#else
    t_WebSocketStart = &WebSocketStart;
    t_SendWebSocketData = &SendWebSocketData;
    t_DirectoryUsage = &DirectoryUsage;
    t_GetExePath = &GetExePath;

#endif

    delegate = nullptr;
};

WebSocket* WebSocket::getInstance() {
    static WebSocket* instance = NULL;
    if (instance == NULL) {
        instance = new WebSocket();
    }
    return instance;
};

void WebSocket::start(std::string listenAddr) {
    if (delegate == nullptr) {
        printf("Please set WebSocket delegate first!");
        exit(1);
    }
    t_WebSocketStart((char*)listenAddr.c_str(), (void*)delegate);
};

void WebSocket::sendData(long long unsigned int goObjRef, std::string data) {
    const std::lock_guard<std::recursive_mutex> lock(mMutex);

    if (socketMap.find(goObjRef) != socketMap.end()) {
        t_SendWebSocketData(goObjRef, (void*)data.c_str(), data.length());
    }
};

void WebSocket::sendDataToAll(std::string data) {
    const std::lock_guard<std::recursive_mutex> lock(mMutex);

    for(auto it=socketMap.begin(); it!=socketMap.end(); ++it) {
        t_SendWebSocketData(it->first, (void*)data.c_str(), data.length());
    }

};

void WebSocket::addOrUpdateSocket(long long unsigned int socket) {

    const std::lock_guard<std::recursive_mutex> lock(mMutex);
    socketMap[socket] = 1;

};

void WebSocket::removeSocket(long long unsigned int socket) {
    const std::lock_guard<std::recursive_mutex> lock(mMutex);
    socketMap.erase(socket);
};

std::string WebSocket::getDirectoryUsage(std::string diretory)
{
    char* ret = t_DirectoryUsage((char*)diretory.c_str());
    std::string jsonStr(ret);
    free(ret);

    return jsonStr;
}

std::string WebSocket::getGetExePath()
{
    char* ret = t_GetExePath();
    std::string str(ret);
    free(ret);

    return str;
}

