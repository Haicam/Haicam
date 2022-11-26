#include "haicam/Config.hpp"
#include "haicam/UserDefault.hpp"
#include "haicam/Utils.hpp"
#include "haicam/FrameCommand.hpp"
#include "haicam/PeerManager.hpp"
#include "haicam/Encryption.hpp"
#include "haicam/ServerConnection.hpp"
#include "json/json.h"

using namespace haicam;

PeerManager* PeerManager::getInstance()
{
    static PeerManager* instance = nullptr;

    if(instance == nullptr)
        instance = new PeerManager();

    return instance;
}

PeerManager::PeerManager()
{
    frameNumber = 0;
}

string PeerManager::getPeerUUIDByRemoteAddrId(uint32_t remoteAddrId, bool* connected)
{
    string peerUUID;
    *connected = false;

    for (auto iter = waitingForOffer.begin(); iter != waitingForOffer.end(); iter++) {
        if (iter->second == remoteAddrId){
            *connected = false;
            peerUUID = iter->first;
            break;
        }
    }

    for (unordered_map<string, PeerConn*>::iterator iter = connectons.begin(); iter != connectons.end(); iter++) {
        if (iter->second->remoteAddrID == remoteAddrId){
            *connected = true;
            peerUUID = iter->second->localUUID;
            break;
        }
    }

    return peerUUID;
}

void PeerManager::createAskForOffer(uint32_t localAddrId, uint32_t remoteAddrId)
{
    bool connected = false;
    string penddingLocalUUID = getPeerUUIDByRemoteAddrId(remoteAddrId, &connected);
    if (connected) {
        return;
    }

    if (penddingLocalUUID.empty())
        penddingLocalUUID = Utils::uuidV4();

    waitingForOffer[penddingLocalUUID] = remoteAddrId;

    Json::Value jsonValue;
    jsonValue["command"] = WEBRTC_CMD;
    jsonValue["action"] = "ASK_OFFER";
    jsonValue["addr_id"] = UserDefault::getInstance()->getLocalAddr();
    jsonValue["src"] = penddingLocalUUID;
    jsonValue["dest"] = Json::nullValue;
    ServerConnection::getInstance()->sendRequest(FRAME_CMD_SEND_COMMAND, jsonValue, remoteAddrId);
}

void PeerManager::createAskForAnswer()
{
    if(connectons.find("-") != connectons.end()) {
        return;
    }

    // we only allow one peer waiting for connection,
    string penddingRemoteUUID = "-";
    // send server for assignment
    connectons[penddingRemoteUUID] = PeerConn::createrOffer(FRAME_ADDR_SERVER, penddingRemoteUUID);

}

void PeerManager::removePeerConn(std::string remoteUUID)
{
    PeerConn* peerConn = NULL;

    if(connectons.find(remoteUUID) != connectons.end())
    {
        peerConn = connectons[remoteUUID];

        Json::Value jsonValue;
        jsonValue["command"] = WEBRTC_CMD;
        jsonValue["action"] = "CLOSE";
        jsonValue["addr_id"] = UserDefault::getInstance()->getLocalAddr();
        jsonValue["src"] = peerConn->localUUID;
        jsonValue["dest"] = peerConn->remoteUUID;
        ServerConnection::getInstance()->sendRequest(FRAME_CMD_SEND_COMMAND, jsonValue, peerConn->remoteAddrID);

        connectons.erase(remoteUUID);
    }

    if (peerConn != NULL) {
        delete peerConn;
        peerConn = NULL;
    }

}

bool PeerManager::sendFrameToPeers(uint8_t cmd, uint8_t type, std::string payloadStr, vector<uint32_t> remoteAddrs ,int iDecode)
{
    for (std::vector<uint32_t>::iterator it = remoteAddrs.begin(); it != remoteAddrs.end(); ++it)
        sendFrame(cmd, type, payloadStr, *it , iDecode);

    return true;
}

bool PeerManager::sendFrame(UINT8 cmd, UINT8 type, std::string payloadStr, UINT32 uDST ,int iDecode, P2Peer* fromPeer) {

    uint32_t iSendNum = frameNumber++;

    std::string data = Encryption::packFrame(iSendNum, cmd, type, payloadStr, uDST, iDecode);

    for (unordered_map<string, PeerConn*>::iterator iter = connectons.begin(); iter != connectons.end(); iter++) {
        if (uDST == FRAME_ADDR_SERVER) {
            if(fromPeer != NULL && iter->second->peer == fromPeer) // webrtc data control command
            {
                iter->second->sendData((char*)data.c_str(),data.size());

            } else if (cmd == FRAME_CMD_PLAYER_CAMERA_CHANNEL && type == FRAME_CMD_REQ && iter->second->showMultipleViews) { // webrtc JPEG data
                iter->second->sendData((char*)data.c_str(),data.size());
            }
            continue;
        }
        if (iter->second->remoteAddrID == uDST){
            iter->second->sendData((char*)data.c_str(),data.size());
        }
    }

    return true;
}

bool PeerManager::sendVideoSample(uint8_t* data, int length, int duration, int isKeyFrame, uint32_t cameraId) {

    for (unordered_map<string, PeerConn*>::iterator iter = connectons.begin(); iter != connectons.end(); iter++) {
        if (iter->second->webRTCVideoForCameraId == cameraId){
            iter->second->sendVideoSample((char*)data, length, duration, isKeyFrame);
        }
    }

    return true;
}

bool PeerManager::getIsReady(UINT32 uDST){
    for (unordered_map<string, PeerConn*>::iterator iter = connectons.begin(); iter != connectons.end(); iter++) {
        if (iter->second->remoteAddrID == uDST){
            bool ready =  iter->second->bIsReady;
            return ready;
        }
    }
    return false;
}

void PeerManager::onWebRTCEvent(string eventPayload, long long unsigned int senderRef)
{
    Json::Value payload = Utils::getJsonFromString(eventPayload);
    string action = payload["action"].asString();
    string src_uuid = payload["src"].asString();
    string dest_uuid = payload["dest"].asString();
    uint32 remoteAddrID = payload["addr_id"].asUInt();
    if(action == "ASK_OFFER")
    {
        if(connectons.find(src_uuid) == connectons.end())
        {
            LOG("PeerConn src_uuid not found %s", src_uuid.c_str());
            if(dest_uuid == "-" && connectons.find("-") != connectons.end() && !payload["desc"].isNull())
            {
                LOG("PeerConn pendding offer assigned by the server, and returned remote description");
                // pendding offer assigned by the server, and returned remote description
                connectons[src_uuid] = connectons["-"];
                connectons.erase("-");
                string remoteDescription = payload["desc"].asString();
                connectons[src_uuid]->remoteUUID = src_uuid;
                connectons[src_uuid]->remoteAddrID = remoteAddrID;
                if(connectons[src_uuid]->peer != nullptr)
                    connectons[src_uuid]->peer->setRemoteDescription(remoteDescription);

                // create a new offer waiting for connection
                createAskForAnswer();
            } else {
                LOG("PeerConn create a new offer asked by peer without remote description");
                // create a new offer asked by peer without remote description
                connectons[src_uuid] = PeerConn::createrOffer(remoteAddrID, src_uuid);
            }
        } else {
            LOG("src_uuid found %s", src_uuid.c_str());
            if(!payload["desc"].isNull())
            {
                LOG("offer asked by peer, and returned remote description");
                // offer asked by peer, and returned remote description
                string remoteDescription = payload["desc"].asString();
                if(connectons[src_uuid]->peer != nullptr)
                    connectons[src_uuid]->peer->setRemoteDescription(remoteDescription);
            } else {
                // resend desc and ice
                LOG("PeerConn resend desc and ice");
                connectons[src_uuid]->reSendICEs();
            }
        }
    } else  if(action == "ASK_ANSWER")
    {
        if(connectons.find(src_uuid) == connectons.end())
        {
            // from website video player
            if(dest_uuid == "" && !payload["desc"].isNull() && remoteAddrID == FRAME_ADDR_SERVER) {
                string remoteDescription = payload["desc"].asString();
                connectons[src_uuid] = PeerConn::createrAnswer(remoteDescription, remoteAddrID, src_uuid);
                if(payload.isMember("signal_type")) connectons[src_uuid]->signalType = payload["signal_type"].asString();
                if(senderRef != 0) connectons[src_uuid]->senderRef = senderRef;
                if(payload.isMember("camera_id")) {
                    connectons[src_uuid]->webRTCVideoForCameraId = payload["camera_id"].asUInt();
                }
                return;
            }
            // end from website video player

            if(waitingForOffer.find(dest_uuid) == waitingForOffer.end()) {
                // HERE SHOULD NOT HAPPEN!! WE DIDN'T ASK ANY OFFER!!
                string remoteDescription = payload["desc"].asString();
                connectons[src_uuid] = PeerConn::createrAnswer(remoteDescription, remoteAddrID, src_uuid);
                connectons[src_uuid]->remoteAddrID = remoteAddrID;
            } else {
                // asked offer is comming now

                waitingForOffer.erase(dest_uuid);
                string remoteDescription = payload["desc"].asString();
                connectons[src_uuid] = PeerConn::createrAnswer(remoteDescription, remoteAddrID, src_uuid, dest_uuid);
            }

        } else {
            if(!payload["desc"].isNull())
            {
                // NEW REMOTE DESCRIPTION COMES AGAIN!!
                LOG("NEW REMOTE DESCRIPTION COMES AGAIN!!");
                //string remoteDescription = payload["desc"].asString();
                //if(connectons[src_uuid]->peer != nullptr)
                //    connectons[src_uuid]->peer->setRemoteDescription(remoteDescription);
            }
        }
    } else if (action == "NEW_ICE")
    {
        if(connectons.find(src_uuid) != connectons.end())
        {
            string ice = payload["ice"].asString();
            if(connectons[src_uuid]->peer != nullptr)
                connectons[src_uuid]->peer->addICECandidate(ice);
        }

    } else if (action == "CLOSE") {
        removePeerConn(src_uuid);
    }
}