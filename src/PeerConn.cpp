#include "haicam/Config.hpp"
#include "haicam/UserDefault.hpp"
#include "haicam/Utils.hpp"
#include "haicam/FrameCommand.hpp"
#include "haicam/Encryption.hpp"
#include "haicam/PeerManager.hpp"
#include "haicam/PeerConn.hpp"
#include "haicam/RunOnMainThread.hpp"
#include "haicam/websocket.h"
#include "haicam/ServerConnection.hpp"
#include "haicam/Context.hpp"
#include "json/json.h"

using namespace haicam;

PeerConn::PeerConn()
    : isOffer(false), peer(nullptr), remoteAddrID(FRAME_ADDR_SERVER), boxHubId(-1), hearBeatTimer()
{
    localUUID = Utils::uuidV4();
    bIsReady = false;

    webRTCVideoForCameraId = 0;
    webRTCAudioForCameraId = 0;
    showMultipleViews = false;

    signalType = "";
    senderRef = 0; // !=0 local only peer

    lastHeartBeatTime = 0;
}

PeerConn *PeerConn::createrOffer(uint32 remoteAddrID, string remoteUUID)
{

    PeerConn *conn = new PeerConn();
    conn->remoteAddrID = remoteAddrID;
    conn->remoteUUID = remoteUUID;
    conn->isOffer = true;

    string iceServers = "[]";
    if (conn->senderRef == 0)
    {
        iceServers = UserDefault::getInstance()->getIceServers();
    }
    conn->peer = P2Peer::createOffer(conn, iceServers);

    return conn;
}

PeerConn *PeerConn::createrAnswer(string remoteOfferDescription, uint32 remoteAddrID, string remoteUUID)
{
    // LOG("EVC11 PeerConn::createrAnswer 1");
    PeerConn *conn = new PeerConn();
    conn->remoteAddrID = remoteAddrID;
    conn->remoteUUID = remoteUUID;
    conn->isOffer = false;

    string iceServers = "[]";
    if (conn->senderRef == 0)
    {
        iceServers = UserDefault::getInstance()->getIceServers();
        ;
    }

    conn->peer = P2Peer::createAnswer(conn, remoteOfferDescription, iceServers);

    return conn;
}

PeerConn *PeerConn::createrAnswer(string remoteOfferDescription, uint32 remoteAddrID, string remoteUUID, string localUUID)
{
    // LOG("EVC11 PeerConn::createrAnswer 2");
    PeerConn *conn = new PeerConn();
    conn->remoteAddrID = remoteAddrID;
    conn->remoteUUID = remoteUUID;
    conn->isOffer = false;
    conn->localUUID = localUUID;

    string iceServers = "[]";
    if (conn->senderRef == 0)
    {
        iceServers = UserDefault::getInstance()->getIceServers();
        ;
    }

    conn->peer = P2Peer::createAnswer(conn, remoteOfferDescription, iceServers);

    return conn;
}

void PeerConn::onDescForRemote(string desc)
{
    Json::Value jsonValue;
    jsonValue["command"] = WEBRTC_CMD;
    if (isOffer)
        jsonValue["action"] = "ASK_ANSWER";
    else
        jsonValue["action"] = "ASK_OFFER";
    jsonValue["addr_id"] = UserDefault::getInstance()->getLocalAddr();
    jsonValue["src"] = localUUID;
    jsonValue["dest"] = remoteUUID;
    jsonValue["desc"] = desc;
    jsonValue["signal_type"] = signalType;

    if (senderRef != 0)
        WebSocket::getInstance()->sendData(senderRef, Utils::getFastString(jsonValue));
    else
        ServerConnection::getInstance()->sendRequest(FRAME_CMD_SEND_COMMAND, jsonValue, remoteAddrID);

    if (!bIsReady)
        pendingICEs.push_back(Utils::getFastString(jsonValue));
}

void PeerConn::onICECandidateForRemote(string candidate)
{
    Json::Value jsonValue;
    jsonValue["command"] = WEBRTC_CMD;
    jsonValue["action"] = "NEW_ICE";
    jsonValue["addr_id"] = UserDefault::getInstance()->getLocalAddr();
    jsonValue["src"] = localUUID;
    jsonValue["dest"] = remoteUUID;
    jsonValue["ice"] = candidate;

    if (senderRef != 0)
        WebSocket::getInstance()->sendData(senderRef, Utils::getFastString(jsonValue));
    else
        ServerConnection::getInstance()->sendRequest(FRAME_CMD_SEND_COMMAND, jsonValue, remoteAddrID);

    if (!bIsReady)
        pendingICEs.push_back(Utils::getFastString(jsonValue));
}

void PeerConn::onReady()
{
    H_ASSERT(peer != nullptr);

    if (!bIsReady) {
        
	    bIsReady = true;

        if(!isOffer) {
            lastHeartBeatTime = time(NULL);

            hearBeatTimer = Timer::create(Context::getInstance(), 0, 3000);
            hearBeatTimer->onTimeoutCallback = std::bind(&PeerConn::onTimeout, this);
            hearBeatTimer->start();
        }
    }

    pendingICEs.clear();
}

void PeerConn::onTimeout()
{
    if(!bIsReady) return;

    if (time(NULL) - lastHeartBeatTime > 15) {
        PeerManager::getInstance()->removePeerConn(remoteUUID);
    } else {
        PeerManager::getInstance()->sendFrame(FRAME_CMD_HEART_BEAT, FRAME_CMD_REQ, "{}", remoteAddrID);
    }
}

void PeerConn::reSendICEs()
{

    for (std::list<std::string>::iterator it = pendingICEs.begin(); it != pendingICEs.end(); ++it)
    {
        if (senderRef != 0)
            WebSocket::getInstance()->sendData(senderRef, *it);
        else
            ServerConnection::getInstance()->sendRequest(FRAME_CMD_SEND_COMMAND, *it, remoteAddrID);
    }
}

void PeerConn::onClose()
{
    bIsReady = false;

    PeerManager::getInstance()->removePeerConn(remoteUUID);
}

void PeerConn::runOnSchedule(std::function<void()> func)
{
    RunOnMainThread::getInstance()->schedule(func);
}

void PeerConn::onData(void *data, int len)
{
    onSocketData(string((char *)data, len));
}

void PeerConn::onDataBuffer(ByteBufferPtr bufPtr)
{
    onSocketData(bufPtr->toString());
}

void PeerConn::onSocketData(string buffer)
{
    uint8 cmd;
    uint8 type;
    uint32 uSRC;
    uint32 uSequenceId;
    std::string payloadStr;

    while (Encryption::unpackFrame(cmd, type, uSRC, uSequenceId, payloadStr,
                       m_strlastBuffer, buffer))
    {
        if (cmd == FRAME_CMD_HEART_BEAT)
        {
            if(type == FRAME_CMD_REQ)
                PeerManager::getInstance()->sendFrame(FRAME_CMD_HEART_BEAT, FRAME_CMD_RES, "{}", uSRC);
            else
                lastHeartBeatTime = time(NULL);
        }
        else if (cmd == FRAME_CMD_PLAYER_SOUND && type == FRAME_CMD_REQ)
        {
            if ((uint8)(payloadStr[0]) == PLAYER_SOUND_PLAY)
            {
                std::string soundData = payloadStr.substr(1);
                // TODO
            }
            else
            { // stop play sound
            }
        }
        else if (cmd == FRAME_CMD_CREATE_CAMERA_CHANNEL && type == FRAME_CMD_RES)
        {
        }
        else if (cmd == FRAME_CMD_PLAYER_CAMERA_CONTROLLER && type == FRAME_CMD_REQ)
        {
            Json::Value jsonValue = Utils::getJsonFromString(payloadStr);
            /*
                COMMAND_H264_PLAY:          11,
                COMMAND_H264_ALIVE:         12,
                COMMAND_H264_STOP:          13,
                COMMAND_JPEG_PLAY:          21,
                COMMAND_JPEG_ALIVE:         22,
                COMMAND_JPEG_STOP:          23,
            */

            if (jsonValue["command"].asInt() > 10)
            {

                /* TODO
                // hub not login to server yet
                if (ACCOUNT_ID == "")
                    continue;

                int command = jsonValue["command"].asInt();
                uint32_t cameraId = jsonValue["cameraId"].asUInt();

                bool verified = false;

                string authCode = jsonValue["token"].asString();
                string key = AccountData::getInstance()->getUserAuthKey();
                std::string accountKey = Utils::HexStringtoString(key);
                std::string sessionKey = md5(accountKey);

                if (authCode != sessionKey)
                {
                    std::string md5_authCode = md5(authCode);
                    std::string cameraKeyAndMd5 = DecodeAES(md5_authCode, accountKey);
                    if (cameraKeyAndMd5.length() == 64)
                    {
                        string cameraKey = cameraKeyAndMd5.substr(0, 32);
                        string md5_cameraKey = cameraKeyAndMd5.substr(32, 32);
                        if (md5(cameraKey) == md5_cameraKey)
                        {
                            verified = true;
                            jsonValue["access_token"] = sessionKey;
                        }
                    }
                    else
                    {
                        verified = false;
                    }
                }
                else
                {
                    verified = true;
                }

                jsonValue["token"] = "******";

                if (!verified)
                {
                    this->webRTCVideoForCameraId = 0;
                    this->showMultipleViews = false;
                    jsonValue["status"] = -1;
                    PeerManager::getInstance()->sendFrame(FRAME_CMD_PLAYER_CAMERA_CONTROLLER, FRAME_CMD_RES, ABMUtils::getFastString(jsonValue), uSRC, FRAME_PLAIN, peer);
                    continue;
                }

                switch (command)
                {
                case 11:
                    this->showMultipleViews = false;
                    this->webRTCVideoForCameraId = cameraId;
                    break;
                case 13:
                    this->webRTCVideoForCameraId = 0;
                    break;
                case 21:
                    this->webRTCVideoForCameraId = 0;
                    this->showMultipleViews = true;
                    break;
                case 23:
                    this->webRTCVideoForCameraId = 0;
                    this->showMultipleViews = false;
                    break;
                default:;
                };

                jsonValue["status"] = 1;
                PeerManager::getInstance()->sendFrame(FRAME_CMD_PLAYER_CAMERA_CONTROLLER, FRAME_CMD_RES, ABMUtils::getFastString(jsonValue), uSRC, FRAME_PLAIN, peer);
                */
            }
            else
            {
                ServerConnection::getInstance()->onRequest(cmd, uSRC, uSequenceId, payloadStr);
            }
        }
        else
        {
            if (type == FRAME_CMD_REQ)
            {
                ServerConnection::getInstance()->onRequest(cmd, uSRC, uSequenceId, payloadStr);
            }
        }
    }
}

bool PeerConn::sendVideoSample(char *data, int length, int duration, int isKeyFrame)
{

    if (!bIsReady)
        return false;

    peer->sendVideoSample((void *)data, length, duration, isKeyFrame);
    return true;
}

bool PeerConn::sendAudioSample(char *data, int length, int duration)
{

    if (!bIsReady)
        return false;
    peer->sendAudioSample((void *)data, length, duration);
    return true;
}

bool PeerConn::sendData(char *data, int length)
{

    if (!bIsReady)
        return false;

    // Given the minimum MTU on the internet is 576 , and the size of the IPv4 header is 20 bytes,
    // and the UDP header 8 bytes. This leaves 548 bytes available for user data

    int left = length;
    int chunk = 512; // 65535; // math.MaxUint16 // message size limit for Chromium
    for (int i = 0; i < length; i += chunk)
    {
        if (left >= chunk)
            peer->sendData((void *)(data + i), chunk);
        else
            peer->sendData((void *)(data + i), left);
        left -= chunk;
    }

    return true;
}

PeerConn::~PeerConn()
{

    bIsReady = false;

    if (hearBeatTimer)
    {
        hearBeatTimer->stop();
    }

    if (peer != nullptr)
    {
        delete peer;
        peer = nullptr;
    }
}
