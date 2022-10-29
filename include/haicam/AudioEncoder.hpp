#pragma once

namespace haicam
{
    enum AudioCodec
    {
        AAC,
        OPUS,
        AAC_HARDWARE,
        AAC_RAW
    };

    class AudioEncoder
    {
    private:
    public:
        AudioEncoder(){};
        virtual ~AudioEncoder(){};
        virtual void open(){};
        virtual void onDataInput(void *data, int len){};
        virtual void onDataOutput(void *data, int len){};
        virtual void close(){};
    };

}
