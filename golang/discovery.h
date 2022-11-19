#ifndef HAICAMLEANBACK_DISCOVERY_H
#define HAICAMLEANBACK_DISCOVERY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

#ifdef __cplusplus
#include <string>

extern "C" {
#endif
    void onOnvifCameraFound(void* cObjRef, char* urn, char* brand, char* model, char* ip, int port);
    void onBandCameraFound(void* cObjRef, char* brand, char* mac, char* ipv4, int port, int isDhcp);
    void onGetCameraStreams(void* cObjRef, char* cameraUuid, char* ip, char* jsonPayload, int httpCode);
    void onCameraEvent(void* cObjRef, int camera_id, char* camera_uuid, char* motionTimeStr, char* motionStateStr, int statusCode);

#ifdef __cplusplus
}
#endif

extern void* haicam_go_p2p_so_handle;

#ifdef __cplusplus

class DiscoveryDelegate {
public:
    virtual void onOnvifCameraFound(char* urn, char* brand, char* model, char* ip, int port) = 0;
    virtual void onBandCameraFound(char* brand, char* mac, char* ipv4, int port, int isDhcp) = 0;
    virtual void onGetCameraStreams(char* cameraUuid, char* ip, char* jsonPayload, int httpCode) = 0;
    virtual void onCameraEvent(int camera_id, char* camera_uuid, char* motionTimeStr, char* motionStateStr, int statusCode) = 0;
};

class Discovery {
public:
    static Discovery* getInstance();
    void run(DiscoveryDelegate* delegate);

    std::string generateUUID();
    void getCameraStreams(char* cameraUUID, char* ipv4, int port, char* username, char* password);
    std::string startPullEvents(int cameraId, char* cameraUUID, char* ipv4, int port, char* username, char* password);
    void stopPullEvents(std::string key);
    int setIPAddress(char* cameraIPv4, int port, char* username, char* password, char* ipv4, char* gateway, int usingDHCP);

    std::string getZWavePortsList();
    int openZWavePort(char* port);
    int zwaveWriteData(void* data, int len);
    int zwaveReadData(void** data);

    DiscoveryDelegate* delegate;
private:
    Discovery();

    bool running;

};
#endif

#endif
