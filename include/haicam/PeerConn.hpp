#ifndef HAICAM_PEERCONN_HPP
#define HAICAM_PEERCONN_HPP

#include <stdio.h>
#include <list>
#include <string>
#include <functional>
#include "haicam/P2Peer.h"
#include "haicam/ByteBuffer.hpp"
#include "haicam/Timer.hpp"

#if Debug

#ifdef __ANDROID__
#include <android/log.h>
#define printf(...) __android_log_print(ANDROID_LOG_DEBUG, "HaicamJNI", __VA_ARGS__);
#define LOG(...) __android_log_print(ANDROID_LOG_DEBUG, "HaicamJNI", __VA_ARGS__);
#endif
#define LOG(...) log(__VA_ARGS__);
#else

#define printf(...)
#define LOG(...)

#endif

using namespace std;

namespace haicam
{

    class PeerConn : public P2PeerDelegate
    {
    public:
        static PeerConn *createrOffer(uint32_t remoteAddrID, string remoteUUID);
        static PeerConn *createrAnswer(string remoteOfferDescription, uint32_t remoteAddrID, string remoteUUID);
        static PeerConn *createrAnswer(string remoteOfferDescription, uint32_t remoteAddrID, string remoteUUID, string localUUID);
        ~PeerConn();

        void onICECandidateForRemote(string candidate);
        void onDescForRemote(string desc);
        void onSocketData(string buffer);

        bool sendData(char *data, int length);
        bool sendVideoSample(char *data, int length, int duration, int isKeyFrame);
        bool sendAudioSample(char *data, int length, int duration);

        void onData(void *data, int len);
        void onDataBuffer(ByteBufferPtr bufPtr);
        void onReady();
        void onTimeout();
        void onClose();

        void runOnSchedule(std::function<void()> func);

        void reSendICEs();

        P2Peer *peer;
        string localUUID;
        string remoteUUID;
        // default FRAME_ADDR_SERVER, offer waits for server to assign
        uint32_t remoteAddrID;
        // if boxHubId == -1, local peer in the box
        int boxHubId;

        bool bIsReady;
        std::string m_strlastBuffer;

        uint32_t webRTCVideoForCameraId;
        uint32_t webRTCAudioForCameraId;
        bool showMultipleViews;

        string signalType;
        long long unsigned int senderRef; // !=0 local only peer

    private:
        PeerConn();
        bool isOffer;

        std::list<std::string> pendingICEs;

        TimerPtr hearBeatTimer;
        long lastHeartBeatTime;
    };

}

#endif // HAICAM_PEERCONN_HPP
