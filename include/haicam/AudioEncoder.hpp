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

    enum PCM_FMT
    {
        FMT_S16
    };

    class AudioEncoder
    {
    private:
    public:
        AudioEncoder(){};
        virtual ~AudioEncoder(){};
        virtual bool open(AudioCodec aCodec, int frequency, int channels, PCM_FMT pcm_fmt){};
        virtual void onDataInput(void *data, int len){};
        virtual void onDataOutput(void *data, int len){};
        virtual void close(){};
    };

}
