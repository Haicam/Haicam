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

bool VideoEncoder::open(VideoCodec vCodec, int width, int height, PIX_FMT pix_fmt)
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

    return true;
}

void VideoEncoder::onDataInput(void *data, int len)
{
    int ret = av_frame_make_writable(frame);
    H_ASSERT_NOT(ret < 0);

    H_ASSERT(1.5 * frame->height * frame->width == len);

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

    ret = avcodec_send_frame(codecCtx, frame);
    H_ASSERT_NOT (ret < 0);

     while (ret >= 0) {
        ret = avcodec_receive_packet(codecCtx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            H_ASSERT_ERR_STR("Error during encoding");
        }

        //printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        bool isKeyFrame = pkt->flags & AV_PKT_FLAG_KEY;
        onDataOutput(pkt->data, pkt->size, isKeyFrame);
        av_packet_unref(pkt);
    }

}

/*
 extradata: 0 0 0 1 sps 0 0 0 1 pps
 key frame: extradata 0 0 0 1 key_frame_data
 non_key_frame: 0 0 0 1 non_key_frame_data
*/

void VideoEncoder::onDataOutput(void *data, int len, bool isKeyFrame)
{
}

void VideoEncoder::close()
{
    // NULL flush packet. This signals the end of the stream
    int ret = avcodec_send_frame(codecCtx, NULL);
    H_ASSERT_NOT (ret < 0);

     while (ret >= 0) {
        ret = avcodec_receive_packet(codecCtx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            H_ASSERT_ERR_STR("Error during encoding");
        }

        //printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        bool isKeyFrame = pkt->flags & AV_PKT_FLAG_KEY;
        onDataOutput(pkt->data, pkt->size, isKeyFrame);
        av_packet_unref(pkt);
    }

    avcodec_free_context(&codecCtx);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    codec = NULL;
    codecCtx = NULL;
    frame = NULL;
    pkt = NULL;
}
