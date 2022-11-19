//
// Created by Frank on 2/11/20.
//

#ifndef HAICAMLEANBACK_P2PEER_H
#define HAICAMLEANBACK_P2PEER_H

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

#define addr_t  long long unsigned int 

#ifdef __cplusplus
#include <string>
using namespace std;

extern "C" {
#endif
extern void onGoLog(char* msg);

extern void onICECandidateForRemote(void* cObjRef, char* candidate);
extern void onDescForRemote(void* cObjRef, char* desc);
extern void onReady(void* cObjRef);
extern void onClose(void* cObjRef);
extern void onData(void* cObjRef, void* data, int length);

#ifdef __cplusplus
}

extern void* haicam_go_p2p_so_handle;

class P2PeerDelegate {
public:
    virtual void onICECandidateForRemote(string candidate) = 0;
    virtual void onDescForRemote(string desc) = 0;
    virtual void onData(void* data, int len) = 0;
    virtual void onReady() = 0;
    virtual void onClose() = 0;
};

class P2Peer
{
public:
    ~P2Peer();

    static P2Peer* createOffer(P2PeerDelegate* delegate, string iceServers);
    static P2Peer* createAnswer(P2PeerDelegate* delegate, string offerDesc, string iceServers);

    void onICECandidateForRemote(string candidate);
    void onDescForRemote(string desc);
    void onData(void* data, int len);
    void onReady();
    void onClose();

    bool sendData(void * data, int len);
    bool sendVideoSample(void * data, int len, int millsecs, int isKeyFrame);
    bool sendAudioSample(void * data, int len, int millsecs);
    bool setRemoteDescription(string desc);
    bool addICECandidate(string candidate);

protected:
    P2Peer();
    P2PeerDelegate* delegate;
    addr_t goObjRef;
    bool isReady;
    
};
#endif

#endif //HAICAMLEANBACK_P2PEER_H
