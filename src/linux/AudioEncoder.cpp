#include "haicam/platform/AudioEncoder.hpp"
#include "haicam/MacroDefs.hpp"
#include "libavutil/time.h"

#define STD_TIMEBASE 1000

using namespace haicam::platform;

AudioEncoder::AudioEncoder()
{
    codec = NULL;
    codecCtx = NULL;
    frame = NULL;
    pkt = NULL;
}

AudioEncoder::~AudioEncoder()
{
}

bool AudioEncoder::open(AudioCodec aCodec, int frequency, int channels, PCM_FMT pcm_fmt)
{
    if (aCodec == OPUS)
    {
        codec = avcodec_find_encoder_by_name("opus");
    } else {
        codec = avcodec_find_encoder_by_name("aac");
    }
    H_ASSERT(codec != NULL);

    codecCtx = avcodec_alloc_context3(codec);
    H_ASSERT(codecCtx);

    pkt = av_packet_alloc();
    H_ASSERT(pkt);

    //codecCtx->bit_rate = 64000;
   
    if (pcm_fmt == FMT_S16)
    {
        codecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    }
    else
    {
        codecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    }

    codecCtx->sample_rate    = frequency;
    codecCtx->channel_layout = channels == 1? AV_CH_LAYOUT_MONO : AV_CH_LAYOUT_STEREO;
    codecCtx->channels       = channels;

    int ret = avcodec_open2(codecCtx, codec, NULL);
    H_ASSERT_NOT(ret < 0);

    frame = av_frame_alloc();
    H_ASSERT_NOT(!frame);

    /*

    AAC uses a blocksize of 1024 or 960 samples

    OPUS codec. It supports frames with: 2.5, 5, 10, 20, 40 or 60 ms of audio data.

    One millisecond of audio with 16kHz is 16 samples (16000/1000). So for mono you can specify frame_size set to:

    16 * 2.5 = 40 (very rare)
    16 * 5 = 80 (rare)
    16 * 10 = 160
    16 * 20 = 320
    16 * 40 = 640
    16 * 60 = 960

    */

    // nb_samples - number of audio samples (per channel) described by this frame
    frame->nb_samples     = codecCtx->frame_size;
    frame->format         = codecCtx->sample_fmt;
    frame->channel_layout = codecCtx->channel_layout;

    ret = av_frame_get_buffer(frame, 0);
    H_ASSERT_NOT(ret < 0);

    return true;
}

void AudioEncoder::onDataInput(void *data, int len)
{
    int ret = av_frame_make_writable(frame);
    H_ASSERT_NOT(ret < 0);

    // for AV_SAMPLE_FMT_S16
    H_ASSERT(codecCtx->frame_size * 2 * codecCtx->channels == len);
    memcpy(frame->data[0], data, codecCtx->frame_size * 2 * codecCtx->channels);

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
        onDataOutput(pkt->data, pkt->size);
        av_packet_unref(pkt);
    }

}


void AudioEncoder::onDataOutput(void *data, int len)
{
}

void AudioEncoder::close()
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
        onDataOutput(pkt->data, pkt->size);
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
