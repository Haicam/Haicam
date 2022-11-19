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
#ifdef HAICAM_USE_FFMPEG
            AVCodec *codec;
            AVCodecContext *codecCtx;
            AVFrame *frame;
            AVPacket *pkt;
#endif

        public:
            VideoEncoder();
            ~VideoEncoder();
            bool open(VideoCodec vCodec, int width, int height, PIX_FMT pix_fmt);
            void onDataInput(void *data, int len);
            void onDataOutput(void *data, int len, bool isKeyFrame);
            void close();
        };

    }

}
