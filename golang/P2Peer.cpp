#include <string.h>
#include "P2Peer.h"
#include <sys/time.h>

#ifdef _WIN32
#define localtime_r(T,Tm) (localtime_s(Tm,T) ? NULL : Tm)
#endif


#ifdef __ANDROID__
#include <dlfcn.h>
//#include "haicam-ext.h"

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

void onGoLog(char* buf) 
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
    printf("%s.%d Golog: %s\n",dtime, milli, buf);
#endif
#endif
}

void onICECandidateForRemote(void* cObjRef, char* candidate)
{
    P2Peer* peer = (P2Peer*)cObjRef;
    peer->onICECandidateForRemote(candidate);
}

void onDescForRemote(void* cObjRef, char* desc)
{
    P2Peer* peer = (P2Peer*)cObjRef;
    peer->onDescForRemote(desc);
}
void onReady(void* cObjRef)
{
    P2Peer* peer = (P2Peer*)cObjRef;
    peer->onReady();
}
void onClose(void* cObjRef)
{
    printf("PeerConn goCallback onClose cRef: %p\n", cObjRef);

    P2Peer* peer = (P2Peer*)cObjRef;
    peer->onClose();
}
void onData(void* cObjRef, void* data, int length)
{
    P2Peer* peer = (P2Peer*)cObjRef;
    peer->onData(data, length);
}

} // end extern "C"

void* haicam_go_p2p_so_handle = NULL;

typedef void (*T_SetRemoteDescription)(addr_t, char*);
typedef void (*T_AddICECandidate)(addr_t, char*);
typedef void (*T_SendData)(addr_t, void*, int);
typedef void (*T_SendVideoSample)(addr_t, void*, int, int, int);
typedef void (*T_SendAudioSample)(addr_t, void*, int, int);
typedef void (*T_ClosePeer)(addr_t);
typedef addr_t (*T_CreateOfferer)(void*, char*);
typedef addr_t (*T_CreateAnswerer)(void*, char*, char*);


T_SetRemoteDescription t_SetRemoteDescription = NULL;
T_AddICECandidate t_AddICECandidate = NULL;
T_SendData t_SendData = NULL;
T_SendVideoSample t_SendVideoSample = NULL;
T_SendAudioSample t_SendAudioSample = NULL;
T_ClosePeer t_ClosePeer = NULL;
T_CreateOfferer t_CreateOfferer = NULL;
T_CreateAnswerer t_CreateAnswerer = NULL;


P2Peer::P2Peer()
{
    delegate = NULL;
    goObjRef = 0;
    isReady = false;

//move to java System.loadLibrary("haicam_ext").  As it causes react native freeze
#if defined(__ANDROID__xxxxx) || defined(HAICAM_WINDOWS_32)
 
    t_SetRemoteDescription = (T_SetRemoteDescription)dlsym(haicam_go_p2p_so_handle, "SetRemoteDescription");
    
    if(t_SetRemoteDescription == NULL) {
        //printf("t_SetRemoteDescription: %s\n", dlerror());
    }
    
    t_AddICECandidate = (T_AddICECandidate)dlsym(haicam_go_p2p_so_handle, "AddICECandidate");
    t_SendData = (T_SendData)dlsym(haicam_go_p2p_so_handle, "SendData");
    t_SendVideoSample = (T_SendVideoSample)dlsym(haicam_go_p2p_so_handle, "SendVideoSample");
    t_SendAudioSample = (T_SendAudioSample)dlsym(haicam_go_p2p_so_handle, "SendAudioSample");
    t_ClosePeer = (T_ClosePeer)dlsym(haicam_go_p2p_so_handle, "ClosePeer");
    t_CreateOfferer = (T_CreateOfferer)dlsym(haicam_go_p2p_so_handle, "CreateOfferer");
    t_CreateAnswerer = (T_CreateAnswerer)dlsym(haicam_go_p2p_so_handle, "CreateAnswerer");
    
    if(t_CreateAnswerer == NULL) {
        //printf("t_CreateAnswerer: %s\n", dlerror());
    }
    
#else
    t_SetRemoteDescription = &SetRemoteDescription;
    t_AddICECandidate = &AddICECandidate;
    t_SendData = &SendData;
    t_SendVideoSample = &SendVideoSample;
    t_SendAudioSample = &SendAudioSample;
    t_ClosePeer = &ClosePeer;
    t_CreateOfferer = &CreateOfferer;
    t_CreateAnswerer = &CreateAnswerer;
#endif

}

P2Peer* P2Peer::createOffer(P2PeerDelegate* delegate, string iceServers)
{
    P2Peer* peer = new P2Peer();
    peer->delegate = delegate;
    
    peer->goObjRef = t_CreateOfferer((void*)peer, (char*)iceServers.c_str());
    
    printf("PeerConn P2Peer createOffer gObjRef: %llu cRef: %p\n", peer->goObjRef, (void*)peer);

    return peer;
}

P2Peer* P2Peer::createAnswer(P2PeerDelegate* delegate, string offerDesc, string iceServers)
{
    P2Peer* peer = new P2Peer();
    peer->delegate = delegate;
    
    peer->goObjRef = t_CreateAnswerer((void*)peer, (char*)offerDesc.c_str(), (char*)iceServers.c_str());
    
    printf("PeerConn P2Peer createAnswer gObjRef: %llu cRef: %p\n", peer->goObjRef, (void*)peer);

    return peer;
}

void P2Peer::onDescForRemote(string desc)
{
    delegate->onDescForRemote(desc);
}

void P2Peer::onICECandidateForRemote(string candidate)
{
    delegate->onICECandidateForRemote(candidate);

}

void P2Peer::onReady()
{
    isReady = true;
    delegate->onReady();

}

void P2Peer::onClose()
{
    isReady = false;
    delegate->onClose();
    
}

void P2Peer::onData(void *data, int len)
{
    delegate->onData(data, len);

}

bool P2Peer::setRemoteDescription(string desc)
{
    if(!goObjRef) return false;
    
     t_SetRemoteDescription(goObjRef, (char*)desc.c_str());

    return true;
}

bool P2Peer::addICECandidate(string candidate)
{
    if(!goObjRef) return false;

    t_AddICECandidate(goObjRef, (char*)candidate.c_str());

    return true;
}

bool P2Peer::sendData(void * data, int len)
{
    if(!isReady || !goObjRef) return false;

    t_SendData(goObjRef, data, len);

    return true;
}

bool P2Peer::sendVideoSample(void * data, int len, int millsecs, int isKeyFrame)
{
    if(!isReady || !goObjRef) return false;

    t_SendVideoSample(goObjRef, data, len, millsecs, isKeyFrame);

    return true;
}

bool P2Peer::sendAudioSample(void * data, int len, int millsecs)
{
    if(!isReady || !goObjRef) return false;

    t_SendAudioSample(goObjRef, data, len, millsecs);

    return true;
}

P2Peer::~P2Peer() {
    isReady = false;
    printf("P2Peer::~P2Peer \n");
        
    if (goObjRef) {
        t_ClosePeer(goObjRef);
        goObjRef = 0;
    }
    
    delegate = NULL;
}
