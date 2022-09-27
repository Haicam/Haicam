#pragma once

#include <string.h>
#include <string>
#include "haicam/sdtype.h"

typedef signed char			int8;
typedef unsigned char		uint8;

typedef signed short		int16;
typedef unsigned short		uint16;

typedef signed int			int32;
typedef unsigned int		uint32;

typedef long long            int64;
typedef unsigned long long   uint64;

#define FRAME_CMD_SOF                            0xFF
#define FRAME_SIGN_HEART_BEAT                    0xFE

#define FRAME_RSA_1024                           0xFF
#define FRAME_AES_128                            0xFE
#define FRAME_PLAIN                              0xFD
#define FRAME_RSA_2048                           0xFC
#define FRAME_AES_256                            0xFB
#define FRAME_SSL_RSA_2048                       0xFA
#define FRAME_SSL_AES_256                        0xF9

#define FRAME_CMD_REQ                            0x00
#define FRAME_CMD_RES                            0x01

#define FRAME_UDP_CMD_REQ                     0x01
#define FRAME_UDP_CMD_RES                     0x02

#define FRAME_UDP_CMD_ADD_DEVICE                                   0x01
#define FRAME_UDP_CMD_DEVICE_INFO                                  0x02
#define FRAME_UDP_CMD_SET_WIFI                                     0x03
#define FRAME_UDP_CMD_SETTING_WIFI                                 0x04

#define FRAME_ADDR_SERVER						        0xFFFFFFFF
#define FRAME_ADDR_APP_BROADCAST				        0xFFFFFFFE
#define FRAME_ADDR_CAMERA_BROADCAST			            0xFFFFFFFD

#define NET_ERROR_WARNING                    0x00
#define NET_DECRYPTO_FAIL                    0x01
#define NET_ERROR_SOCKET_TIMEOUT             0x02
#define NET_SERVER_CHANGE                    0x03
#define NET_USER_ID_DISABLED                 0x04
#define NET_ADDR_ID_DISABLED                 0x05
#define NET_CAMERA_ID_DISABLED               0x06
#define NET_HUB_ID_DISABLED                  0x07


#define LOGIN_ERROR_TOKEN_NOT_EXIST        	 0x00000066


//APP Client Frame
#define FRAME_CMD_RSA_HANDSHAKE                         0xFF
#define FRAME_CMD_NET_ERROR                             0x00
#define FRAME_CMD_HEART_BEAT                            0x01
#define FRAME_CMD_GET_ADDR_ID                           0x02
#define FRAME_CMD_GET_RSA_KEY                           0x03
#define FRAME_CMD_GET_ADDR_STAUTS                       0x04
#define FRAME_CMD_GET_SERVER_INFO                       0x05
#define FRAME_CMD_CHANGE_SERVER				            0x06
#define FRAME_CMD_RESET_SAFE_CODE                       0x07
#define FRAME_CMD_VERSION						        0x08
#define FRAME_CMD_UPLOAD_LOG        					0x0e
#define FRAME_CMD_REGISTER                              0x0f

#define FRAME_CMD_USER_REGISTER                         0x10
#define FRAME_CMD_USER_LOGIN                            0x11
#define FRAME_CMD_USER_RELOGIN                          0x12
#define FRAME_CMD_USER_SET_MODE                         0x13
#define FRAME_CMD_USER_ADD						        0x14
#define FRAME_CMD_USER_REMOVE					        0x15
#define FRAME_CMD_USER_SET_PIN					        0x16
#define FRAME_CMD_USER_SET_INFO				            0x17
#define FRAME_CMD_USER_SET_ACTIVATED			        0x18
#define FRAME_CMD_USER_SET_PASSWORD			            0x19
#define FRAME_CMD_USER_GET_INFO				            0x1a
#define FRAME_CMD_USER_GET_LIST				            0x1b
#define FRAME_CMD_USER_FORGOT_PASSWORD                  0x1c
#define FRAME_CMD_USER_LOGOUT                           0x1d
#define FRAME_CMD_USER_AUTH                             0x1e
#define FRAME_CMD_USER_APPLY                            0x1f



#define FRAME_CMD_CAMERA_ADD                            0x20
#define FRAME_CMD_CAMERA_DELETE                         0x21
#define FRAME_CMD_CAMERA_EDIT                           0x22
#define FRAME_CMD_CAMERA_GET                            0x23
#define FRAME_CMD_CAMERA_LIST                           0x24
#define FRAME_CMD_CAMERA_PLAY                           0x25
#define FRAME_CMD_CAMERA_STOP                           0x26
#define FRAME_CMD_CAMERA_ONOFF                          0x27
#define FRAME_CMD_CAMERA_PANTILT                        0x28


#define FRAME_CMD_SECURITY_STATUS_BROADCAST             0x30
#define FRAME_CMD_SECURITY_GET_STATUS                   0x31
#define FRAME_CMD_SECURITY_DISARM                       0x32
#define FRAME_CMD_SECURITY_AWAY                         0x33
#define FRAME_CMD_SECURITY_STAY                         0x34

#define FRAME_CMD_EVENT_BROADCAST                       0x40
#define FRAME_CMD_EVENT_PRODUCE                         0x41
#define FRAME_CMD_EVENT_GET_LIST                        0x42
#define FRAME_CMD_EVENT_FTP_UPLAND       				0x43

#define FRAME_CMD_PLAYER_SOUND					        0x50
#define PLAYER_SOUND_ERROR	    0x00
#define PLAYER_SOUND_PLAY	    0x01
#define PLAYER_SOUND_STOP       0x02
#define FRAME_CMD_PLAYER_VIDEO                          0x51
#define FRAME_CMD_PLAYER_ARCHIVE                        0x52

#define FRAME_CMD_PLAYER_CAMERA_CONTROLLER		0x53
#define CAMERA_SIG_WORK	    0x00
#define CAMERA_SIG_ERROR	0x01
#define CAMERA_SIG_TIMEOUT  0x02
#define CAMERA_SIG_PAUSE	0x03
#define CAMERA_SIG_RESUME	0x04

#define FRAME_CMD_PLAYER_CAMERA_CHANNEL		    0x54
#define PACKET_TYPE_HEADER	0x01
#define PACKET_TYPE_KEY	    0x02
#define PACKET_TYPE_NORMAL	0x03

#define FRAME_CMD_RECORDING_STATUS				0x55
#define FRAME_CMD_LOCAL_STORAGE                         0x56
#define FRAME_CMD_LOCAL_ARCHIVE                         0x57
#define FRAME_CMD_SEND_COMMAND                          0x58
#define ERROR_TYPE               0
#define SET_WIFISSID_TYPE        1
#define UPATE_FIRMWARE_TYPE      2
#define REBOOT_TYPE              3
#define SDCARD_TYPE              4
#define SDCARD_DETECTED_TYPE     5
#define SDCARD_SUPPORT_TYPE      6
#define SDCARD_FORMAT_TYPE       7
#define SOFTREBOOT_TYPE          8
#define RESET_AND_BEEP_TYPE      9
#define CHECK_SDCARD_ISEXIST     10
#define REBOOT_NO_SOUND          11
#define PLAY_SOUND_BY_S3         12
#define FRAME_CMD_FACTORY_DEFAULT                       0x59
#define FRAME_CMD_CREATE_CAMERA_CHANNEL		0x5a
#define FRAME_CMD_CREATE_CLINET_CHANNEL		0x5b
#define FRAME_CMD_DEL_ARCHIVE					0x5c
#define FRAME_CMD_PLAYER_ARCHIVE_CHANNEL    0x5d

#define FRAME_CMD_SCHEDULE_ADD_SCHEDULE		            0x60
#define FRAME_CMD_SCHEDULE_GET_SCHEDULE		            0x66
#define FRAME_CMD_SCHEDULE_SET_SCHEDULE		            0x63
#define FRAME_CMD_SCHEDULE_DEL_SCHEDULE		            0x64

#define FRAME_CMD_HUB_REMOVE					0x71
#define FRAME_CMD_HUB_EDIT						0x72
#define FRAME_CMD_HUB_GET_HUB_INFO				0x73
#define FRAME_CMD_HUB_GET_HUB_LIST			    0x74
#define FRAME_CMD_SOURCE_VENDOR_SEARCH			0x75
#define FRAME_CMD_SOURCE_MODEL_SEARCH			0x76
#define FRAME_CMD_SOURCE_LIST					0x77
#define FRAME_CMD_SOURCE_ADD_WEIGHT			    0x78
#define FRAME_CMD_ADDR_LIST					    0x79
#define FRAME_CMD_ADDR_DELETE					0x7a
#define FRAME_CMD_ADDR_SET_ACTIVATED			0x7b
#define FRAME_CMD_USER_SET_AUTH_CODE			0x7c
#define FRAME_CMD_USER_SET_CHECK_PASSWORD		0x7d
#define FRAME_CMD_HUB_ADD 						0x7e

#define FRAME_CMD_PAY_GET_TOKEN		0x80
#define FRAME_CMD_PAY_PUT_NONCE		0x81
#define FRAME_CMD_GET_PAY_NONCE		0x82
#define FRAME_CMD_GET_PAY_LIST		0x83
#define FRAME_CMD_UN_BILLING		0x84
#define FRAME_CMD_EDIT_PAY_MENT		0x85
#define FRAME_CMD_PAY_MENT		    0x86
#define FRAME_CMD_GET_PLAN_LIST     0x87
#define FRAME_CMD_GET_GOOGLE_TOKEN		  0x88
#define FRAME_CMD_REVOKE_GOOGLE_TOKEN     0x89

#define FRAME_CMD_ZWAVE_EVENT                   0x90 //panel端主动推送
#define ZWAVE_EVENT_TYPE_INIT_INFO      0x01 //初始化完成
#define ZWAVE_EVENT_TYPE_VALUE_CHANGE   0x02 //节点状态改变
#define ZWAVE_EVENT_TYPE_CMD_STATE      0x03 //命令状态
#define ZWAVE_EVENT_TYPE_NODE_ADDED     0x04 //添加节点
#define ZWAVE_EVENT_TYPE_NODE_REMOVED   0x05 //删除节点
#define ZWAVE_EVENT_TYPE_NODE_INIT      0x06 //节点查询完成
#define ZWAVE_EVENT_TYPE_GET_MANUFACTURER     0x07 //厂家信息

#define FRAME_CMD_ZWAVE_ADD                     0x91
#define FRAME_CMD_ZWAVE_REMOVE                  0x92
#define FRAME_CMD_ZWAVE_CANCEL                  0x93
#define FRAME_CMD_ZWAVE_RESET                   0x94
#define FRAME_CMD_ZWAVE_GET_CONTROLLER_LIST     0x95
#define FRAME_CMD_ZWAVE_GET_DEVICE_LIST         0x96
#define FRAME_CMD_ZWAVE_GET_VALUE               0x97
#define FRAME_CMD_ZWAVE_SET_VALUE               0x98
#define FRAME_CMD_ZWAVE_DEVICE_SETTINGS         0x99
#define FRAME_CMD_ZWAVE_HAS_FAILE               0x9a
#define FRAME_CMD_ZWAVE_REMOVE_FAILE            0x9b
#define FRAME_CMD_ZWAVE_NETWORK_UPDATE          0x9c
#define FRAME_CMD_ZWAVE_SET_CONFIG              0x9d
#define FRAME_CMD_ZWAVE_GET_NODE_CONFIG         0x9e
#define FRAME_CMD_ZWAVE_GET_DISCOVERY     0xa2


#define FRAME_CMD_B2_GET_UPLOAD_URL                   0xb0
#define FRAME_CMD_IAM_GET_KEY                         0xb1
#define FRAME_CMD_PAYMENT_GET_BINDED_PLAN             0xb3

//Mode Camera Frame
#define MODE_NONE                                       0x00
#define MODE_APP                                        0x01
#define MODE_CAMERA                                     0x02
#define MODE_HUB                                        0x03

//Event type
#define EVETN_MOTION                                    0x01
#define EVNET_SOUND                                     0x02
#define EVENT_RING                                      0x03
#define EVENT_PEOPLE                                    0x05

#define FRAME_CMD_SECURITY_GET_CONFIG			        0x35
#define FRAME_CMD_SECURITY_SET_CONFIG			        0x36


#define CAMERA_TYPE_DIRECT           0x00
#define CAMERA_TYPE_MOBILE           0x01
#define CAMERA_TYPE_HUBLINK          0x02


std::string getCMDName(int cmd);
std::string getCMDType(int type);


typedef struct strTag_Camera_Info {
    UINT32 iCameraId;
    int iCameraHandlerId;
    int iCameraType;
    std::string strCameraName;
    std::string strUserName;
    std::string strPassword;
    std::string strMacAddress;
    std::string strIpAddress;
    int iHttpPort;
    int iRtspPort;
    std::string strParameters;
    std::string strDateCreated;
    std::string strDateUpdated;
    UINT32      iCameraSRC;
    UINT32      iHubId;
    bool        bIsPublic;
    std::string strAPI;
    bool        bIsLive;
    bool        bIsMotionDetect;
    bool        bIsSoundDetect;
    bool        bIRLEDs;
//    std::string strCameraInfo;
    bool        bIsRTSP;
    bool        bIsHls;
} Camera_Info;


typedef struct strTag_Hub_Info {
    int iId;
    int iAccountId;
    int iType;
    std::string strName;
    std::string strParameters;
    UINT32      iSRC;
    bool bIsSiren;
} Hub_Info;

struct NetworkRecData {
    uint8 m_cmd;
    uint8 m_type;
    uint32 m_uDST;
    uint32 uSequenceId;
    std::string m_payload;
};


struct CameraScheduleData
{
    int iScheduleId;
    int iAccountId;
    UINT32 iCameraId;
    std::string strName;
    std::string strFrom;
    std::string strTo;
    UINT16 uScheduleMask;
};

typedef struct strTag_Plan_Info {
    std::string strName;
    std::string strPlanId;
    std::string strPeriod;
    int iPlanTime;
    float         fPrice;
    float         fTotlePrice;
    std::string strCurrency;
} Plan_Info;


enum MoveDirection
{
    NO_Dt,
    UP_Dt,
    DOWN_Dt,
    LEFT_Dt,
    RIGHT_Dt
};

static int icounttt = 0;

static int heardSize = 6;

static int heardSize2 = 18;
#define MAX_PACKET_LEN 1024*8

