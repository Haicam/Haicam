#include "haicam/platform/VideoEncoder.hpp"
#include "haicam/MacroDefs.hpp"

#include "libavutil/time.h"

#define STD_TIMEBASE 1000

using namespace haicam::platform;

VideoEncoder::VideoEncoder()
{
    codec = NULL;
    codecCtx = NULL;
    frame = NULL;
    pkt = NULL;
}

VideoEncoder::~VideoEncoder()
{
}

void VideoEncoder::open(VideoCodec vCodec, int width, int height, PIX_FMT pix_fmt)
{
    if (vCodec == LIBOPENH264)
    {
        codec = avcodec_find_encoder_by_name("libopenh264");
    }
    H_ASSERT(codec != NULL);

    codecCtx = avcodec_alloc_context3(codec);
    H_ASSERT(codecCtx);

    pkt = av_packet_alloc();
    H_ASSERT(pkt);

    //codecCtx->bit_rate = 400000;
    /* resolution must be a multiple of two */
    codecCtx->width = width;
    codecCtx->height = height;
    /* frames per second */
    codecCtx->time_base = (AVRational){1, STD_TIMEBASE};
    //codecCtx->framerate = (AVRational){25, 1};

    codecCtx->gop_size = 10;
    codecCtx->max_b_frames = 1;

    if (pix_fmt == YUV420P)
    {
        codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    }
    else
    {
        codecCtx->pix_fmt = AV_PIX_FMT_YUYV422;
    }

    int ret = avcodec_open2(codecCtx, codec, NULL);
    H_ASSERT_NOT(ret < 0);

    frame = av_frame_alloc();
    H_ASSERT_NOT(!frame);

    frame->format = codecCtx->pix_fmt;
    frame->width = codecCtx->width;
    frame->height = codecCtx->height;
    ret = av_frame_get_buffer(frame, 0);
    H_ASSERT_NOT(ret < 0);

    ret = av_frame_make_writable(frame);
    H_ASSERT_NOT(ret < 0);
}

void VideoEncoder::onDataInput(void *data, int len)
{
    if (codecCtx->pix_fmt == AV_PIX_FMT_YUV420P || codecCtx->pix_fmt == AV_PIX_FMT_YUVJ420P)
    {
        int nYUVBufsize = 0;
        for (int i = 0; i < frame->height; i++)
        {
            memcpy(frame->data[0] + i * frame->linesize[0], data + nYUVBufsize, frame->width);
            nYUVBufsize += frame->width;
        }
        for (int i = 0; i < frame->height / 2; i++)
        {
            memcpy(frame->data[1] + i * frame->linesize[1], data + nYUVBufsize, frame->width / 2);
            nYUVBufsize += frame->width / 2;
        }
        for (int i = 0; i < frame->height / 2; i++)
        {
            memcpy(frame->data[2] + i * frame->linesize[2], data + nYUVBufsize, frame->width / 2);
            nYUVBufsize += frame->width / 2;
        }
    }
    else
    {
        int nYUVBufsize = 0;
        for (int i = 0; i < frame->height; i++)
        {
            memcpy(frame->data[0] + frame->linesize[0] * i, data + nYUVBufsize, frame->width);
            nYUVBufsize += frame->width;
        }
        for (int i = 0; i < frame->height / 2; i++)
        {
            memcpy(frame->data[1] + frame->linesize[1] * i, data + nYUVBufsize, frame->width);
            nYUVBufsize += frame->width;
        }
    }

    frame->pts = av_gettime() / STD_TIMEBASE;
}


void VideoEncoder::onDataOutput(void *data, int len)
{
}

void VideoEncoder::close()
{
    avcodec_free_context(&codecCtx);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    codec = NULL;
    codecCtx = NULL;
    frame = NULL;
    pkt = NULL;
}
