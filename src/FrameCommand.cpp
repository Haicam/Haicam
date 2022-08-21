#include <string>
#include <stdio.h>
#include "haicam/FrameCommand.hpp"

std::string getCMDType(int type)
{
    switch (type) {
        case FRAME_CMD_RES: return "FRAME_CMD_RES";
        case FRAME_CMD_REQ: return "FRAME_CMD_REQ";
        default:{
            char cmdName[24];
            sprintf(cmdName, "FRAME_CMD_UNKNOW(0x%02x)",type);
            return std::string(cmdName);
        }
    }
};

std::string getCMDName(int cmd)
{
    switch (cmd) {
        case FRAME_CMD_SOF: return "FRAME_CMD_SOF";
        case FRAME_SIGN_HEART_BEAT: return "FRAME_SIGN_HEART_BEAT";
        case FRAME_CMD_NET_ERROR: return "FRAME_CMD_NET_ERROR";
        case FRAME_CMD_HEART_BEAT: return "FRAME_CMD_HEART_BEAT";
        case FRAME_CMD_GET_ADDR_ID: return "FRAME_CMD_GET_ADDR_ID";
        case FRAME_CMD_USER_REGISTER: return "FRAME_CMD_USER_REGISTER";
        case FRAME_CMD_USER_LOGIN: return "FRAME_CMD_USER_LOGIN";
        case FRAME_CMD_USER_RELOGIN: return "FRAME_CMD_USER_RELOGIN";
        case FRAME_CMD_USER_SET_MODE: return "FRAME_CMD_USER_SET_MODE";
        case FRAME_CMD_USER_ADD: return "FRAME_CMD_USER_ADD";
            
        case FRAME_CMD_USER_REMOVE: return "FRAME_CMD_USER_REMOVE";
        case FRAME_CMD_USER_SET_PIN: return "FRAME_CMD_USER_SET_PIN";
        case FRAME_CMD_USER_SET_INFO: return "FRAME_CMD_USER_SET_INFO";
        case FRAME_CMD_USER_SET_ACTIVATED: return "FRAME_CMD_USER_SET_ACTIVATED";
        case FRAME_CMD_USER_SET_PASSWORD: return "FRAME_CMD_USER_SET_PASSWORD";
        case FRAME_CMD_USER_GET_INFO: return "FRAME_CMD_USER_GET_INFO";
        case FRAME_CMD_USER_GET_LIST: return "FRAME_CMD_USER_GET_LIST";
        case FRAME_CMD_USER_FORGOT_PASSWORD: return "FRAME_CMD_USER_FORGOT_PASSWORD";
            
        case FRAME_CMD_USER_LOGOUT: return "FRAME_CMD_USER_LOGOUT";
        case FRAME_CMD_CAMERA_ADD: return "FRAME_CMD_CAMERA_ADD";
        case FRAME_CMD_CAMERA_DELETE: return "FRAME_CMD_CAMERA_DELETE";
        case FRAME_CMD_CAMERA_EDIT: return "FRAME_CMD_CAMERA_EDIT";
        case FRAME_CMD_CAMERA_GET: return "FRAME_CMD_CAMERA_GET";
        case FRAME_CMD_CAMERA_LIST: return "FRAME_CMD_CAMERA_LIST";
            
        case FRAME_CMD_CAMERA_PLAY: return "FRAME_CMD_CAMERA_PLAY";
        case FRAME_CMD_PLAYER_VIDEO: return "FRAME_CMD_PLAYER_VIDEO";
        case FRAME_CMD_CAMERA_STOP: return "FRAME_CMD_CAMERA_STOP";
        case FRAME_CMD_SECURITY_GET_STATUS: return "FRAME_CMD_SECURITY_GET_STATUS";
        case FRAME_CMD_SECURITY_STATUS_BROADCAST: return "FRAME_CMD_SECURITY_STATUS_BROADCAST";
        case FRAME_CMD_SECURITY_DISARM: return "FRAME_CMD_SECURITY_DISARM";
        case FRAME_CMD_SECURITY_AWAY: return "FRAME_CMD_SECURITY_AWAY";
        case FRAME_CMD_SECURITY_STAY: return "FRAME_CMD_SECURITY_STAY";
            
        case FRAME_CMD_EVENT_BROADCAST: return "FRAME_CMD_EVENT_BROADCAST";
        case FRAME_CMD_EVENT_PRODUCE: return "FRAME_CMD_EVENT_PRODUCE";
        case FRAME_CMD_EVENT_GET_LIST: return "FRAME_CMD_EVENT_GET_LIST";
        case FRAME_CMD_PLAYER_SOUND: return "FRAME_CMD_PLAYER_SOUND";
        default:{
            char cmdName[24];
            sprintf(cmdName, "FRAME_CMD_UNKNOW(0x%02x)",cmd);
            return std::string(cmdName);
        }
    }
};
