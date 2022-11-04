#pragma once
#include <vector>
#include <memory>
#include <stdint.h>

namespace haicam
{

    class VideoInput
    {
    protected:
        std::vector<uint8_t> sps;
        std::vector<uint8_t> pps;
        std::vector<uint8_t> gop;

    public:
        VideoInput(){};
        virtual ~VideoInput(){};
        virtual bool open(){};
        virtual void onData(std::shared_ptr<uint8_t> pData, int len, uint8_t isKeyFrame = 1){};
        virtual void getSnapshot(std::shared_ptr<uint8_t> &pData, int &len, int &width, int &height){};
        virtual void h264Parser(std::shared_ptr<uint8_t> pData, int len, uint8_t isKeyFrame = 1);
        virtual void close(){};
    };

}
