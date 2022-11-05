#pragma once
#include <memory>
#include <stdint.h>

namespace haicam
{

    class AudioInput
    {
    private:
    public:
        AudioInput(){};
        virtual ~AudioInput(){};
        virtual bool open(){};
        virtual void onData(std::shared_ptr<uint8_t> pData, int len){};
        virtual void close(){};
    };

}
