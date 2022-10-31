#pragma once
#include "haicam/AudioEncoder.hpp"

#if defined(HAICAM_PLATFORM_LINUX)
#include <libavcodec/avcodec.h>
#endif

namespace haicam
{
    namespace platform
    {

        class AudioEncoder : public haicam::AudioEncoder
        {
        private:
#if defined(HAICAM_PLATFORM_LINUX)
            AVCodec *codec;
            AVCodecContext *codecCtx;
            AVFrame *frame;
            AVPacket *pkt;

#endif

        public:
            AudioEncoder();
            ~AudioEncoder();
            bool open(AudioCodec aCodec, int frequency, int channels, PCM_FMT pcm_fmt);
            void onDataInput(void *data, int len);
            void onDataOutput(void *data, int len);
            void close();
        };

    }

}
