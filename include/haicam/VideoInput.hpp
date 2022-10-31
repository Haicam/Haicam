#pragma once

namespace haicam
{

    class VideoInput
    {
    private:
    public:
        VideoInput(){};
        virtual ~VideoInput(){};
        virtual bool open(){};
        virtual void onData(std::shared_ptr<uint8_t> data, int len){};
        virtual void close(){};
    };

}
