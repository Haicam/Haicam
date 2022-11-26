#ifndef HAICAM_PEERMANAGER_HPP
#define HAICAM_PEERMANAGER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <stdint.h>
#include "haicam/PeerConn.hpp"

using namespace std;

namespace haicam
{

    class PeerManager
    {
    public:
        static PeerManager *getInstance();
        void onWebRTCEvent(string eventPayload, long long unsigned int senderRef = 0);

        void createAskForOffer(uint32_t localAddrId, uint32_t remoteAddrId);
        void createAskForAnswer();

        string getPeerUUIDByRemoteAddrId(uint32_t remoteAddrId, bool *connected);
        void removePeerConn(std::string remoteUUID);

        bool sendFrameToPeers(uint8_t cmd, uint8_t type, std::string payloadStr, vector<uint32_t> remoteAddrs, int iDecode = FRAME_PLAIN);
        bool sendFrame(uint8_t cmd, uint8_t type, std::string payloadStr, uint32_t uDST, int iDecode = FRAME_PLAIN, P2Peer *fromPeer = NULL);

        bool sendVideoSample(uint8_t *data, int length, int duration, int isKeyFrame, uint32_t cameraId);

        bool getIsReady(uint32_t uDST);

        std::recursive_mutex mtx;

    private:
        PeerManager();

        unordered_map<string, PeerConn *> connectons;
        unordered_map<string, uint32_t> waitingForOffer;

        uint32_t frameNumber;
    };

}

#endif // HAICAM_PEERMANAGER_HPP
