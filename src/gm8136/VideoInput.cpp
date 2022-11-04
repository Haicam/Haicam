
#include "haicam/platform/VideoInput.hpp"
#include "haicam/Utils.hpp"
#include "haicam/MacroDefs.hpp"
#include "haicam/PacketHeader.hpp"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include "gm8136/gmlib.h"

using namespace haicam::platform;

static int BITSTREAM_LEN = (720 * 1280);
static int BITSTREAM_LEN2 = (720 * 1280);

#define MD_DATA_LEN (720 * 576 / 4)

static char NAL_START[] = {0, 0, 0, 1, 1};

static gm_system_t *gm_system = NULL;
static void *groupfd = NULL;  // return of gm_new_groupfd()
static void *bindfd = NULL;   // return of gm_bind()
static void *bindfd_2 = NULL; // return of gm_bind()
static void *capture_object = NULL;
static void *capture_object_2 = NULL;
static void *encode_object = NULL;
static void *encode_object_2 = NULL;

static bool is_single_stream;
static int enc_exit = 0;
pthread_mutex_t m_mutexExit;
static pthread_t enc_thread_id;

VideoInput::VideoInput()
{
}
VideoInput::~VideoInput()
{
}

static int getExit()
{
    int iExit;
    pthread_mutex_lock(&m_mutexExit);
    iExit = enc_exit;
    pthread_mutex_unlock(&m_mutexExit);
    return iExit;
}

static long long timeInMilliseconds(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

static long long _frameCountTime = 0;
static int _frameCount = 0;

static void *encode_thread(void *arg)
{
    VideoInput *thiz = (VideoInput *)arg;

    int i, ret, max_len[3] = {0};
    char *bitstream_data = NULL;
    char *bitstream_data_2 = NULL;
    int ibindfdNum = 1;
    gm_pollfd_t poll_fds[2];
    gm_enc_multi_bitstream_t multi_bs[2];

    bitstream_data = (char *)malloc(BITSTREAM_LEN);
    memset(bitstream_data, 0, BITSTREAM_LEN);

#if (GM_VERSION_CODE == 0x0034)
    char *mv_data = (char *)malloc(MD_DATA_LEN);
    memset(mv_data, 0, MD_DATA_LEN);
#endif

    memset(poll_fds, 0, sizeof(poll_fds));
    poll_fds[ibindfdNum - 1].bindfd = bindfd;
    poll_fds[ibindfdNum - 1].event = GM_POLL_READ;
    max_len[ibindfdNum - 1] = BITSTREAM_LEN;

    if (bindfd_2)
    {
        ibindfdNum += 1;
        if (!is_single_stream)
        {
            bitstream_data_2 = (char *)malloc(BITSTREAM_LEN2);
            memset(bitstream_data_2, 0, BITSTREAM_LEN2);

            poll_fds[ibindfdNum - 1].bindfd = bindfd_2;
            poll_fds[ibindfdNum - 1].event = GM_POLL_READ;
            max_len[ibindfdNum - 1] = BITSTREAM_LEN2;
        }
    }

    while (getExit() == 0)
    {
        /** poll bitstream until 500ms timeout */
        ret = gm_poll(poll_fds, ibindfdNum, 500);

        if (ret == GM_TIMEOUT)
        {
            usleep(100000);
            continue;
        }
        memset(multi_bs, 0, sizeof(multi_bs)); // clear all mutli bs
        for (i = 0; i < ibindfdNum; i++)
        {
            if (poll_fds[i].revent.event != GM_POLL_READ)
                continue;
            if (poll_fds[i].revent.bs_len > max_len[i])
            {

                continue;
            }
            multi_bs[i].bindfd = poll_fds[i].bindfd;
            if (i == 0)
            {
                multi_bs[i].bs.bs_buf = bitstream_data; // set buffer point
                multi_bs[i].bs.bs_buf_len = max_len[i]; // set buffer length
#if (GM_VERSION_CODE == 0x0034)
                multi_bs[i].bs.mv_buf = mv_data;
                multi_bs[i].bs.mv_buf_len = MD_DATA_LEN;
#else
                multi_bs[i].bs.extra_buf = NULL;
                multi_bs[i].bs.extra_buf_len = 0;
#endif
            }
            else if (i == 1)
            {
                multi_bs[i].bs.bs_buf = bitstream_data_2; // set buffer point
                multi_bs[i].bs.bs_buf_len = max_len[i];   // set buffer length
            }
        }

        if ((ret = gm_recv_multi_bitstreams(multi_bs, ibindfdNum)) < 0)
        {
            usleep(10000);
        }
        else
        {
            for (i = 0; i < ibindfdNum; i++)
            {
                if ((multi_bs[i].retval < 0) && multi_bs[i].bindfd)
                {
                }
                else if (multi_bs[i].retval == GM_SUCCESS)
                {

                    if (timeInMilliseconds() - _frameCountTime > 5 * 1000)
                    {
                        haicam::Utils::log("camera_settings video %i frameRate: %f \n", i, _frameCount / 5.0);
                        _frameCountTime = timeInMilliseconds();
                        _frameCount = 0;
                    }
                    _frameCount++;

                    if (i == 0) // stream 0
                    {
                        if (multi_bs[i].bs.bs_len > 4 && memcmp(multi_bs[i].bs.bs_buf, NAL_START, 4) == 0)
                        {
                            // split frame by 00 00 00 01
                            int len = multi_bs[i].bs.bs_len;
                            int offset = 0;
                            int last = 0;
                            char *data = multi_bs[i].bs.bs_buf;
                            char *buf = NULL;
                            bool done = false;

                            for (int i = 0; i < 10; i++) // max 10, in case dead loop
                            {
                                buf = data + offset;
                                while (memcmp(buf, NAL_START, 4) != 0)
                                {
                                    offset++;
                                    if (offset > len - 4)
                                    {
                                        offset = len;
                                        done = true;
                                        break;
                                    }
                                    buf = data + offset;
                                }

                                if (offset > 0)
                                {
                                    H_MEM_SP(uint8_t, pData, offset - last);
                                    memcpy(pData.get(), data + last, offset - last);
                                    thiz->onData(pData, offset - last, multi_bs[i].bs.keyframe);
                                }

                                if (done)
                                    break;
                                last = offset;
                                offset++;
                            }
                        }
                    }
                    else if (i == 1) // stream 1
                    {
                        // Only one steam now
                    }
                }
            }
        }
    }
    free(bitstream_data);
    if (bitstream_data_2)
    {
        free(bitstream_data_2);
    }
    return 0;
}

bool VideoInput::open()
{
    int width;
    int height;
    int frameRate;
    int bitRate;
    int iDetecteValue;
    void *pgmsystem;
    unsigned char uPeople;
    bool vflip;

    if (frameRate < 25)
        frameRate++;

    if (frameRate < 5)
    {
        frameRate = 5;
    }

    if (!pgmsystem)
    {
        return false;
    }
    if (groupfd)
    {
        return true;
    }

    pthread_mutex_init(&m_mutexExit, NULL);
    enc_exit = 0;

    int ch, ret = 0;
    DECLARE_ATTR(cap_attr, gm_cap_attr_t);
    DECLARE_ATTR(h264e_attr, gm_h264e_attr_t);
    //    DECLARE_ATTR(cap_flip_attr, gm_cap_flip_t);
    gm_system = (gm_system_t *)pgmsystem;

    groupfd = gm_new_groupfd();                    // create new record group fd
    capture_object = gm_new_obj(GM_CAP_OBJECT);    // new capture object
    encode_object = gm_new_obj(GM_ENCODER_OBJECT); // // create encoder object

#if (GM_VERSION_CODE != 0x0034)
    DECLARE_ATTR(m_flip, gm_rotation_attr_t);
    if (vflip)
    {
        m_flip.direction = GM_ROTATE_NONE;
        m_flip.flip = GM_FLIP_VERTICAL_AND_HORIZONTAL;
    }
    else
    {
        m_flip.direction = GM_ROTATE_NONE;
        m_flip.flip = GM_FLIP_NONE;
    }
    gm_set_attr(capture_object, &m_flip);
#endif

    ch = 0; // use capture virtual channel 0
    cap_attr.cap_vch = ch;

#if (GM_VERSION_CODE == 0x0034)
    if (height >= 720)
    {
        cap_attr.path = 2;
    }
    else
    {
        cap_attr.path = 3;
    }
    cap_attr.enable_mv_data = 1;
#else
    cap_attr.path = 0;
#endif

    gm_set_attr(capture_object, &cap_attr); // set capture attribute

#if (GM_VERSION_CODE == 0x0034)
    DECLARE_ATTR(dnr_attr, gm_3dnr_attr_t);
    dnr_attr.enabled = 1;
    gm_set_attr(capture_object, &dnr_attr);
#else
    DECLARE_ATTR(dnr_attr, gm_didn_attr_t);
    dnr_attr.denoise = 1; // DNR Denosie
    gm_set_attr(capture_object, &dnr_attr);
#endif

    is_single_stream = true;

    h264e_attr.dim.width = gm_system->cap[0].dim.width;
    h264e_attr.dim.height = gm_system->cap[0].dim.height;

    h264e_attr.dim.width = width;
    h264e_attr.dim.height = height;
    h264e_attr.frame_info.framerate = frameRate;
    h264e_attr.ratectl.mode = GM_EVBR;
    h264e_attr.ratectl.gop = frameRate * 2;
    h264e_attr.ratectl.bitrate = bitRate; // 2Mbps
    h264e_attr.ratectl.bitrate_max = bitRate;

#if (GM_VERSION_CODE == 0x0034)
    h264e_attr.b_frame_num = 0;    // B-frames per GOP (H.264 high profile)
    h264e_attr.enable_mv_data = 1; // enable H.264 motion data output
#endif

    gm_set_attr(encode_object, &h264e_attr);

    bindfd = gm_bind(groupfd, capture_object, encode_object);

#if (GM_VERSION_CODE == 0x0034)
    DECLARE_ATTR(h264e_advance_attr, gm_h264_advanced_attr_t);
    h264e_advance_attr.field_coding = 0;
    h264e_advance_attr.multi_slice = 0; ///< Divide 4 regions of multislice
    //    printf("Night_Vision :%d\n",Night_Vision);
    //    if (getightVision()) {
    ////        printf("Night_Vision 1\n");
    //        h264e_advance_attr.gray_scale = 1;
    //    }else{
    ////        printf("Night_Vision 0\n");
    //        h264e_advance_attr.gray_scale = 0;
    //    }
    if (encode_object)
    {
        gm_set_attr(encode_object, &h264e_advance_attr);
    }
    if (encode_object_2)
    {
        gm_set_attr(encode_object_2, &h264e_advance_attr);
    }

#endif

    if (gm_apply(groupfd) < 0)
    {
        Utils::log("Error! gm_apply fail, AP procedure something wrong!");
        return false;
    }

#if (GM_VERSION_CODE == 0x0034)
    gm_cap_flip_t m_flip = {0};
    if (vflip)
    {
        m_flip.h_flip_enabled = 1;
        m_flip.v_flip_enabled = 1;
    }
    else
    {
        m_flip.h_flip_enabled = 0;
        m_flip.v_flip_enabled = 0;
    }
    gm_set_cap_flip(0, &m_flip);
#endif

    ret = pthread_create(&enc_thread_id, NULL, encode_thread, (void *)this);
    if (ret < 0)
    {
        Utils::log("create encode thread failed\n");
        return false;
    }

    return true;
}

void VideoInput::getSnapshot(std::shared_ptr<uint8_t> &pData, int &len, int &width, int &height)
{
    int snapshot_len = 0;
    H_MEM_SP(uint8_t, buf, width * height / 2);

    snapshot_t snapshot;
    snapshot.bindfd = bindfd;
    snapshot.image_quality = 30; // The value of image quality from 1(worst) ~ 100(best)
    snapshot.bs_buf = (char *)buf.get();
    snapshot.bs_buf_len = width * height / 2;
    snapshot.bs_width = width;
    snapshot.bs_height = height;

    snapshot_len = gm_request_snapshot(&snapshot, 500); // Timeout value 500ms
    if (snapshot_len > 0)
    {
        pData = buf;
        len = snapshot_len;
    }
    else
    {
        len = 0;
    }
}

void VideoInput::onData(std::shared_ptr<uint8_t> pData, int len, uint8_t isKeyFrame)
{
    h264Parser(pData, len, isKeyFrame);
}

void VideoInput::close()
{
}