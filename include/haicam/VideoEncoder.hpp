#pragma once

namespace haicam
{
    enum VideoCodec
    {
        LIBOPENH264,
        H264_V4L2M2M,
        H264_HARDWARE,
        H264_RAW
    };

    enum PIX_FMT
    {
        YUV420P,
        YUYV422
    };

    class VideoEncoder
    {
    private:
    public:
        VideoEncoder(){};
        virtual ~VideoEncoder(){};
        virtual void open(VideoCodec vCodec, int width, int height, PIX_FMT pix_fmt){};
        virtual void onDataInput(void *data, int len){};
        virtual void onDataOutput(void *data, int len){};
        virtual void close(){};
    };

}
