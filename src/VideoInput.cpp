#include "haicam/VideoInput.hpp"
#include "haicam/PacketHeader.hpp"
#include "haicam/Utils.hpp"

using namespace haicam;

/*
H264/H265
https://blog.csdn.net/weixin_39331060/article/details/123767318

Each NAL begins with start code 0x00000001 or 0x000001
#define IS_NAL_UNIT_START(buffer_ptr) (!buffer_ptr[0] && !buffer_ptr[1] && !buffer_ptr[2] && (buffer_ptr[3] == 1))
#define IS_NAL_UNIT_START1(buffer_ptr) (!buffer_ptr[0] && !buffer_ptr[1] && buffer_ptr[2] == 1))

the next byte contains type of the NAL.
For H264, need to read value of (the byte & 0x1f) to get the type

enum Nal_Unit_Type
{
  NAL_UNIT_EXTERNAL                 = 0,
  NAL_UNIT_CODED_SLICE              = 1,
  NAL_UNIT_CODED_SLICE_DATAPART_A   = 2,
  NAL_UNIT_CODED_SLICE_DATAPART_B   = 3,
  NAL_UNIT_CODED_SLICE_DATAPART_C   = 4,
  NAL_UNIT_CODED_SLICE_IDR          = 5,
  NAL_UNIT_SEI                      = 6,
  NAL_UNIT_SPS                      = 7,
  NAL_UNIT_PPS                      = 8,
  NAL_UNIT_ACCESS_UNIT_DELIMITER    = 9,
  NAL_UNIT_END_OF_SEQUENCE          = 10,
  NAL_UNIT_END_OF_STREAM            = 11,
  NAL_UNIT_FILLER_DATA              = 12,
  NAL_UNIT_SUBSET_SPS               = 15,
  NAL_UNIT_CODED_SLICE_PREFIX       = 14,
  NAL_UNIT_CODED_SLICE_SCALABLE     = 20,
  NAL_UNIT_CODED_SLICE_IDR_SCALABLE = 21
};

For H265, need to read value of (the byte & 0x7e) >> 1 to get the type
// 0-9 P frame
// 16-21 I-frame
enum HEVCNALUnitType {
    HEVC_NAL_TRAIL_N    = 0,
    HEVC_NAL_TRAIL_R    = 1,
    HEVC_NAL_TSA_N      = 2,
    HEVC_NAL_TSA_R      = 3,
    HEVC_NAL_STSA_N     = 4,
    HEVC_NAL_STSA_R     = 5,
    HEVC_NAL_RADL_N     = 6,
    HEVC_NAL_RADL_R     = 7,
    HEVC_NAL_RASL_N     = 8,
    HEVC_NAL_RASL_R     = 9,
    HEVC_NAL_BLA_W_LP   = 16,
    HEVC_NAL_BLA_W_RADL = 17,
    HEVC_NAL_BLA_N_LP   = 18,
    HEVC_NAL_IDR_W_RADL = 19,
    HEVC_NAL_IDR_N_LP   = 20,
    HEVC_NAL_CRA_NUT    = 21,
    HEVC_NAL_VPS        = 32, // VPS
    HEVC_NAL_SPS        = 33, // SPS
    HEVC_NAL_PPS        = 34, // PPS
    HEVC_NAL_AUD        = 35,
    HEVC_NAL_EOS_NUT    = 36,
    HEVC_NAL_EOB_NUT    = 37,
    HEVC_NAL_FD_NUT     = 38,
    HEVC_NAL_SEI_PREFIX = 39,
    HEVC_NAL_SEI_SUFFIX = 40,
};
*/

void VideoInput::h264Parser(std::shared_ptr<uint8_t> pData, int len, uint8_t isKeyFrame)
{
    uint64_t startTime = Utils::getMillTimestmap();
    bool requiredIFrame = true;
    bool isIFRAME = false; // 5 I-Frame

    uint32_t timestamp = Utils::getMillTimestmap() - startTime;

    uint8_t *data = pData.get();

    // start 00 00 00 01
    uint8_t nal_unit_type = data[4] & 0x1F;

    switch (nal_unit_type)
    {
    case 5: // I Frame
        isIFRAME = true;
        break;
    case 6: // SEI
        return;
    case 7: // SPS
        sps.assign(data + 4, data + len);
        return;
    case 8: // PPS
        pps.assign(data + 4, data + len);
        return;
    default:
        break;
    } 

    if (requiredIFrame && !isIFRAME)
    {
        return;
    }

    requiredIFrame = false;

    PacketHeader header;
    header.type = 0;
    header.timestamp = timestamp;
    header.size = len;
    if (isIFRAME)
    {
        header.type = 2;
    }

    if (isIFRAME)
    {
        bool shrink = false;
        if (gop.capacity() > gop.size() * 3)
            shrink = true;
        gop.clear();
        if (shrink)
            std::vector<uint8_t>().swap(gop);
    }

    gop.insert(gop.end(), data, data + len);
}
