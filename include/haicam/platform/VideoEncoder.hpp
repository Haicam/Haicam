#pragma once
#include "haicam/VideoEncoder.hpp"

#if defined(HAICAM_PLATFORM_LINUX)
#include <libavcodec/avcodec.h>
#endif

namespace haicam
{
    namespace platform
    {

        class VideoEncoder : public haicam::VideoEncoder
        {
        private:
#if defined(HAICAM_PLATFORM_LINUX)
            AVCodec *codec;
            AVCodecContext *codecCtx;
            AVFrame *frame;
            AVPacket *pkt;

#endif

        public:
            VideoEncoder();
            ~VideoEncoder();
            void open(VideoCodec vCodec, int width, int height, PIX_FMT pix_fmt);
            void onDataInput(void *data, int len);
            void onDataOutput(void *data, int len);
            void close();
        };

    }

}
