
#include "haicam/platform/AudioOutput.hpp"
#include "haicam/Utils.hpp"

#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include "gm8136/gmlib.h"

using namespace haicam::platform;

#define BITSTREAM_LEN 12800
#define PATTERN_NAME "audio_8khz_16bit"

// gm_system_t gm_system;
static void *play_groupfd = NULL; // return of gm_new_groupfd()
static void *play_bindfd;         // return of gm_bind()
static void *file_object;
static void *audio_render_object;
static pthread_t play_thread_id;
static int pb_exit = 0;

AudioOutput::AudioOutput()
{
}
AudioOutput::~AudioOutput()
{
}

static void *playback_thread(void *arg)
{
    AudioOutput *thiz = (AudioOutput *)arg;

    int ret;
    int length;
    char *bitstream_data;
    gm_dec_multi_bitstream_t multi_bs[1];

    bitstream_data = (char *)malloc(BITSTREAM_LEN);
    if (!bitstream_data)
    {
        //        printf("Error allocation\n");
        return NULL;
    }

    while (1)
    {
        if (pb_exit == 1)
            break;

        thiz->onData(bitstream_data, &length);
        //        printf("getPlayAudioData:%d",length);
        if (length == 0)
        {
            usleep(100000);
            //            printf("Invalid length, len(%d)\n", length);
            continue;
        }

        if (length > BITSTREAM_LEN)
        {
            usleep(100000);
            //            printf("Invalid length, len(%d)\n", length);
            continue;
        }

        memset(multi_bs, 0, sizeof(multi_bs)); // clear all mutli bs
        multi_bs[0].bindfd = play_bindfd;
        multi_bs[0].bs_buf = bitstream_data;
        multi_bs[0].bs_buf_len = length;

        if ((ret = gm_send_multi_bitstreams(multi_bs, 1, 500)) < 0)
        {
            //            printf("<send bitstream fail(%d)!>\n", ret);
            return 0;
        }
        usleep(100);
    }
    free(bitstream_data);
    return 0;
}

bool AudioOutput::open()
{
    int samplerate = 8000;

    if (play_groupfd)
    {
        return 0;
    }
    pb_exit = 0;
    int ch;
    DECLARE_ATTR(file_attr, gm_file_attr_t);
    DECLARE_ATTR(audio_render_attr, gm_audio_render_attr_t);

    ch = 0;
    Utils::log("startAudioPlay samplerate : %d\n", samplerate);

    play_groupfd = gm_new_groupfd();

    file_object = gm_new_obj(GM_FILE_OBJECT);
    audio_render_object = gm_new_obj(GM_AUDIO_RENDER_OBJECT);

    // sample_rate/sample_size/channel_type: the audio info of the input file
    file_attr.sample_rate = samplerate;
    file_attr.sample_size = 16;
    file_attr.channel_type = GM_MONO;
    gm_set_attr(file_object, &file_attr);

    // block_size: PCM(don't care) AAC(1024) ADPCM(256) G711(320)
    audio_render_attr.vch = ch;
    audio_render_attr.encode_type = GM_PCM;// or  GM_AAC

#if(GM_VERSION_CODE == 0x0034)
    audio_render_attr.block_size = 1024;
#else
    audio_render_attr.frame_samples = 1024;
#endif

    gm_set_attr(audio_render_object, &audio_render_attr);

    play_bindfd = gm_bind(play_groupfd, file_object, audio_render_object);
    if (gm_apply(play_groupfd) < 0)
    {
        //        perror("Error! gm_apply fail, AP procedure something wrong!");
        return -1;
    }

    if (pthread_create(&play_thread_id, NULL, playback_thread, (void *)this))
    {
        //        perror("Create au_thread[sample_send_audio_bitstream] failed");
        return -1;
    }
#if EZHOMELABS_CAMERA
    SPK_PWON();
#endif
    return 1;

    return true;
}

void AudioOutput::onData(void *data, int *len)
{
}

void AudioOutput::close()
{
    if (play_groupfd)
    {
        //        printf("stopAudioPlay 1\n");
        pb_exit = 1;
        pthread_join(play_thread_id, NULL);

        gm_unbind(play_bindfd);
        gm_apply(play_groupfd);
        gm_delete_obj(file_object);
        gm_delete_obj(audio_render_object);
        gm_delete_groupfd(play_groupfd);
        play_groupfd = NULL;
//        printf("stopAudioPlay 2\n");
#if EZHOMELABS_CAMERA
        SPK_SHUT();
#endif
    }
}