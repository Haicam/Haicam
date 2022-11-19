#include "discovery.h"

#ifdef __ANDROID__

#include <dlfcn.h>

#if Debug
#include <jni.h>
#include <android/log.h>
#define TAG "HaicamJNI cocos2d-x"
#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#endif

//move to java System.loadLibrary("haicam_ext").  As it causes react native freeze
#include "libhaicam_ext.h"
#else

#if Debug == 0
#define printf(...)
#endif

#include "libhaicam_ext.h"

#endif

#ifdef HAICAM_WINDOWS_32
#include "dlfcn.h"
#endif

extern "C" {

void onOnvifCameraFound(void* cObjRef, char* urn, char* brand, char* model, char* ip, int port)
{
    ((DiscoveryDelegate*)cObjRef)->onOnvifCameraFound(urn, brand, model, ip, port);

}

void onBandCameraFound(void* cObjRef, char* brand, char* mac, char* ipv4, int port, int isDhcp)
{
    ((DiscoveryDelegate*)cObjRef)->onBandCameraFound(brand, mac, ipv4, port, isDhcp);
}

void onGetCameraStreams(void* cObjRef, char* cameraUuid, char* ip, char* jsonPayload, int httpCode)
{
    ((DiscoveryDelegate*)cObjRef)->onGetCameraStreams(cameraUuid, ip, jsonPayload, httpCode);

}

void onCameraEvent(void* cObjRef, int camera_id, char* camera_uuid, char* motionTimeStr, char* motionStateStr, int statusCode)
{
    ((DiscoveryDelegate*)cObjRef)->onCameraEvent(camera_id, camera_uuid, motionTimeStr, motionStateStr, statusCode);
}

} // extern "C"

typedef char* (*T_GenerateUUID)();
typedef void (*T_GetCameraStreams)(char*, char*, int, char*, char*);
typedef char* (*T_StartPullEvents)(int, char*, char*, int, char*, char*);
typedef void (*T_StopPullEvents)(char*);
typedef void (*T_DiscoveryRun)(void*);
typedef int (*T_SetIPAddress)(char*, int, char*, char*, char*, char*, int);

typedef char* (*T_GetZWavePortsList)();
typedef int (*T_OpenZWavePort)(char*);
typedef int (*T_ZWaveWriteData)(void*, int);
typedef int (*T_ZWaveReadData)(void**);

T_GenerateUUID t_GenerateUUID = NULL;
T_GetCameraStreams t_GetCameraStreams = NULL;
T_StartPullEvents t_StartPullEvents = NULL;
T_StopPullEvents t_StopPullEvents = NULL;
T_DiscoveryRun t_DiscoveryRun = NULL;
T_SetIPAddress t_SetIPAddress = NULL;

T_GetZWavePortsList t_GetZWavePortsList = NULL;
T_OpenZWavePort t_OpenZWavePort = NULL;
T_ZWaveWriteData t_ZWaveWriteData = NULL;
T_ZWaveReadData t_ZWaveReadData = NULL;

Discovery* Discovery::getInstance() {
    static Discovery* instance = NULL;
    if (instance == NULL) {
        instance = new Discovery();
    }
    return instance;
}

Discovery::Discovery() {

    delegate = NULL;
    running = false;

    //printf("onvif new Discovery instance\n");

//move to java System.loadLibrary("haicam_ext").  As it causes react native freeze
#if defined(__ANDROID__xxxxx) || defined(HAICAM_WINDOWS_32)

    t_GenerateUUID = (T_GenerateUUID)dlsym(haicam_go_p2p_so_handle, "GenerateUUID");
    if(t_GenerateUUID == NULL) {
        printf("onvif t_GenerateUUID: %s\n", dlerror());
    }

    t_GetCameraStreams = (T_GetCameraStreams)dlsym(haicam_go_p2p_so_handle, "GetCameraStreams");
    
    if(t_GetCameraStreams == NULL) {
        printf("onvif t_GetCameraStreams: %s\n", dlerror());
    }

    t_StartPullEvents = (T_StartPullEvents)dlsym(haicam_go_p2p_so_handle, "StartPullEvents");
    t_StopPullEvents = (T_StopPullEvents)dlsym(haicam_go_p2p_so_handle, "StopPullEvents");
    t_DiscoveryRun = (T_DiscoveryRun)dlsym(haicam_go_p2p_so_handle, "DiscoveryRun");
    t_SetIPAddress = (T_SetIPAddress)dlsym(haicam_go_p2p_so_handle, "SetIPAddress");

    t_GetZWavePortsList = (T_GetZWavePortsList)dlsym(haicam_go_p2p_so_handle, "GetZWavePortsList");
    t_OpenZWavePort = (T_OpenZWavePort)dlsym(haicam_go_p2p_so_handle, "OpenZWavePort");
    t_ZWaveWriteData = (T_ZWaveWriteData)dlsym(haicam_go_p2p_so_handle, "ZWaveWriteData");
    t_ZWaveReadData = (T_ZWaveReadData)dlsym(haicam_go_p2p_so_handle, "ZWaveReadData");

    if(t_DiscoveryRun == NULL) {
        printf("onvif t_DiscoveryRun: %s\n", dlerror());
    } else {
        printf("dlsym: onvif Discovery is linked\n");
    }
#else
    t_GenerateUUID = &GenerateUUID;
    t_GetCameraStreams = &GetCameraStreams;
    t_StartPullEvents = &StartPullEvents;
    t_StopPullEvents = &StopPullEvents;
    t_DiscoveryRun = &DiscoveryRun;
    t_SetIPAddress = &SetIPAddress;

    t_GetZWavePortsList = &GetZWavePortsList;
    t_OpenZWavePort = &OpenZWavePort;
    t_ZWaveWriteData = &ZWaveWriteData;
    t_ZWaveReadData = &ZWaveReadData;
#endif
}

void Discovery::run(DiscoveryDelegate* delegate)
{
    if(!running) {
        this->delegate = delegate;
        running = true;
        t_DiscoveryRun((void*) delegate);
    }
}

std::string Discovery::generateUUID()
{
    char* key = t_GenerateUUID();
    std::string uuid(key);
    free(key);

    return uuid;
}

void Discovery::getCameraStreams(char* cameraUUID, char* ipv4, int port, char* username, char* password)
{
    t_GetCameraStreams(cameraUUID, ipv4, port, username, password);
}

std::string Discovery::startPullEvents(int cameraId, char* cameraUUID, char* ipv4, int port, char* username, char* password)
{
    char* key = t_StartPullEvents(cameraId, cameraUUID, ipv4, port, username, password);

    std::string eventKey(key);
    free(key);

    return eventKey;
}

void Discovery::stopPullEvents(std::string key) 
{
    t_StopPullEvents((char*)key.c_str());

}

int Discovery::setIPAddress(char* cameraIPv4, int port, char* username, char* password, char* ipv4, char* gateway, int usingDHCP)
{
    return t_SetIPAddress(cameraIPv4, port, username, password, ipv4, gateway, usingDHCP);

}

std::string Discovery::getZWavePortsList() {
    char* ret = t_GetZWavePortsList();
    std::string str(ret);
    free(ret);
    return str;
}

int Discovery::openZWavePort(char* port){
    return t_OpenZWavePort(port);
}

int Discovery::zwaveWriteData(void* data, int len){
    return t_ZWaveWriteData(data, len);
}

int Discovery::zwaveReadData(void** data){
    return t_ZWaveReadData(data);
}
