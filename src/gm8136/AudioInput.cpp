
#include "haicam/platform/AudioInput.hpp"
#include "haicam/Utils.hpp"
#include "haicam/MacroDefs.hpp"
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "gm8136/gmlib.h"

using namespace haicam::platform;

#define BITSTREAM_LEN 12800
#define MAX_BITSTREAM_NUM 1

// gm_system_t gm_system;
static void *audio_groupfd = NULL;
static void *audio_bindfd[MAX_BITSTREAM_NUM];
static void *audio_grab_object[MAX_BITSTREAM_NUM];
static void *audio_encode_object[MAX_BITSTREAM_NUM];
static pthread_t thread_id;
static int audio_enc_exit = 0;

AudioInput::AudioInput()
{
}
AudioInput::~AudioInput()
{
}

static void *encode_thread(void *arg)
{
    AudioInput *thiz = (AudioInput *)arg;

    int i, ret;
    char *bitstream_data[1];
    gm_pollfd_t poll_fds[1];
    gm_enc_multi_bitstream_t multi_bs[1];

    for (i = 0; i < 1; i++)
    {

        bitstream_data[i] = (char *)malloc(BITSTREAM_LEN);
        if (bitstream_data[i] == 0)
            return 0;
        memset(bitstream_data[i], 0, BITSTREAM_LEN);
    }

    memset(poll_fds, 0, sizeof(poll_fds));
    for (i = 0; i < 1; i++)
    {
        poll_fds[i].bindfd = audio_bindfd[i];
        poll_fds[i].event = GM_POLL_READ;
    }

    while (audio_enc_exit == 0)
    {
        ret = gm_poll(poll_fds, 1, 1000);
        if (ret == GM_TIMEOUT)
        {
            //            printf("audio Poll timeout!!\n");
            usleep(100000);
            continue;
        }

        memset(multi_bs, 0, sizeof(multi_bs));
        for (i = 0; i < 1; i++)
        {
            if (poll_fds[i].revent.event != GM_POLL_READ)
            {
                continue;
            }
            if (poll_fds[i].revent.bs_len > BITSTREAM_LEN)
            {
                //                printf("buffer length is not enough! %d, %d\n",poll_fds[i].revent.bs_len, BITSTREAM_LEN);
                continue;
            }
            multi_bs[i].bindfd = audio_bindfd[i];
            multi_bs[i].bs.bs_buf = bitstream_data[i];
            multi_bs[i].bs.bs_buf_len = BITSTREAM_LEN;
#if(GM_VERSION_CODE == 0x0034)
            multi_bs[i].bs.mv_buf = NULL;
            multi_bs[i].bs.mv_buf_len = 0;
#else
            multi_bs[i].bs.extra_buf = NULL;
            multi_bs[i].bs.extra_buf_len = 0;
#endif
        }

        if ((ret = gm_recv_multi_bitstreams(multi_bs, 1)) < 0)
        {
            //            printf("--1-- Error return value %d\n", ret);
            usleep(200);
        }
        else
        {
            for (i = 0; i < 1; i++)
            {
                if (!multi_bs[i].bindfd)
                    continue;
                if (multi_bs[i].retval < 0)
                {
                    //                    printf("get bitstreame error! ret = %d\n", ret);
                }
                else if (multi_bs[i].retval == GM_SUCCESS)
                {
                    H_MEM_SP(uint8_t, pData, multi_bs[i].bs.bs_len);
                    memcpy(pData.get(), multi_bs[i].bs.bs_buf, multi_bs[i].bs.bs_len);

                    thiz->onData(pData, multi_bs[i].bs.bs_len);
                }
            }
            usleep(100);
        }
    }

    for (i = 0; i < 1; i++)
    {
        free(bitstream_data[i]);
    }
    return 0;
}

bool AudioInput::open()
{
    if (audio_groupfd)
    {
        return false;
    }
    audio_enc_exit = 0;
    int ch, i, ret = 0;
    DECLARE_ATTR(audio_grab_attr, gm_audio_grab_attr_t);
    DECLARE_ATTR(audio_encode_attr, gm_audio_enc_attr_t);
    //    DECLARE_ATTR(audio_grab_attr2, gm_audio_grab_attr_t);
    //    DECLARE_ATTR(audio_encode_attr2, gm_audio_enc_attr_t);
    ch = 0;
    // printf("Audio record from input vch %d\n", ch);
    audio_groupfd = gm_new_groupfd();
    //    audio_groupfd2 = gm_new_groupfd();

    for (i = 0; i < MAX_BITSTREAM_NUM; i++)
    {
        audio_grab_object[i] = gm_new_obj(GM_AUDIO_GRAB_OBJECT);
        audio_encode_object[i] = gm_new_obj(GM_AUDIO_ENCODER_OBJECT);

        if (i == 0)
        {
            audio_grab_attr.vch = 0;
        }
        else
        {
            //            audio_grab_attr2.vch = 0;
        }

        if (i == 0)
        {
            audio_grab_attr.sample_rate = 8000;
            audio_grab_attr.sample_size = 16;
            audio_grab_attr.channel_type = GM_MONO;
            gm_set_attr(audio_grab_object[i], &audio_grab_attr);
            audio_encode_attr.encode_type = GM_AAC;
            audio_encode_attr.bitrate = 32000;
            audio_encode_attr.frame_samples = 1024;
            gm_set_attr(audio_encode_object[i], &audio_encode_attr);

            audio_bindfd[i] = gm_bind(audio_groupfd, audio_grab_object[i], audio_encode_object[i]);
            if (gm_apply(audio_groupfd) < 0)
            {
                Utils::log("Error! gm_apply fail, AP procedure something wrong!");
                close();
                return false;
            }
            ret = pthread_create(&thread_id, NULL, encode_thread, (void *)this);
            if (ret < 0)
            {
                Utils::log("create encode thread failed");
                close();
                return false;
            }
        }
    }

    return true;
}

void AudioInput::onData(std::shared_ptr<uint8_t> pData, int len)
{
}

void AudioInput::close()
{
    if (audio_groupfd)
    {
        audio_enc_exit = 1;
        int ch;
        pthread_join(thread_id, NULL);
        for (ch = 0; ch < MAX_BITSTREAM_NUM; ch++)
            gm_unbind(audio_bindfd[ch]);
        gm_apply(audio_groupfd);
        for (ch = 0; ch < MAX_BITSTREAM_NUM; ch++)
        {
            gm_delete_obj(audio_grab_object[ch]);
            gm_delete_obj(audio_encode_object[ch]);
        }
        gm_delete_groupfd(audio_groupfd);
        audio_groupfd = NULL;
    }
}