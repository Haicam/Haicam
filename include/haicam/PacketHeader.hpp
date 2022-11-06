#pragma once

#include <stdint.h>

namespace haicam
{

#pragma pack(push, 1)

    typedef struct
    {
        uint8_t type; // 0-video, 1-audio ,2-video key
        uint32_t timestamp;
        uint32_t size;
    } PacketHeader;

#pragma pack(pop)

}