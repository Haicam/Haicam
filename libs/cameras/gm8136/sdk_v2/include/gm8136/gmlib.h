#ifndef __GMLIB_H__
#define __GMLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#define GM_VERSION_CODE 0x0041

/* return value definition */
#define GM_SUCCESS               0
#define GM_FD_NOT_EXIST         -1
#define GM_BS_BUF_TOO_SMALL     -2
#define GM_EXTRA_BUF_TOO_SMALL  -3
#define GM_TIMEOUT              -4
#define GM_DUPLICATE_FD         -5

/***********************************
 *        General Structure
 ***********************************/

/* Dimention definition */
typedef struct gm_dim {
    int width;
    int height;
} gm_dim_t;

/* Rectancle definition */
typedef struct gm_rect {
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
} gm_rect_t;

typedef enum {
    GM_CAP_OBJECT = 0xFEFE0001,         ///< Capture object type
    GM_ENCODER_OBJECT = 0xFEFE0002,     ///< Encoder object type
    GM_WIN_OBJECT = 0xFEFE0003,         ///< Window object type
    GM_FILE_OBJECT = 0xFEFE0004,        ///< File object type
    GM_AUDIO_GRAB_OBJECT = 0xFEFE0005,     ///< Audio grabber object type
    GM_AUDIO_ENCODER_OBJECT = 0xFEFE0006,  ///< Audio encoder object type
    GM_AUDIO_RENDER_OBJECT = 0xFEFE0007,   ///< Audio renderer object type
} gm_obj_type_t;

/***********************************
 *        System Structure
 ***********************************/
typedef struct {
    int valid;
    unsigned int chipid;
    int reserved[8];         ///< Reserved words
} gm_file_sys_info_t;

typedef enum {
    GM_INTERLACED,
    GM_PROGRESSIVE,
} gm_video_scan_method_t;

typedef enum {
    GM_AUTO = 0,
    GM_YUV420,
    GM_YUV422,
    GM_RGB1555,
    GM_RGB8888,
} gm_format_t;

typedef struct {
    int valid;
    unsigned int number_of_path;
    gm_video_scan_method_t scan_method;
    int framerate;
    gm_dim_t dim;
    unsigned int chipid;
    int is_present;
    int reserved[8];         ///< Reserved words
} gm_cap_sys_info_t;

typedef struct {
    int valid;
    int framerate;
    gm_dim_t dim;
    unsigned int chipid;
    int reserved[9];         ///< Reserved words
} gm_lcd_sys_info_t;

typedef enum {
    GM_MONO = 1,
    GM_STEREO,
} gm_audio_channel_type_t;

typedef struct {
    int valid;
    gm_audio_channel_type_t channel_type;
    int sample_rate;  /* 8000, 16000, 32000, 44100... */
    int sample_size;  /* bit number per sample, 8bit, 16bit... */
    int ssp;  /* Hardware SSP ID from 0,1... */
    int reserved[5];         ///< Reserved words
} gm_audio_grab_sys_info_t;

typedef struct {
    int valid;
    gm_audio_channel_type_t channel_type;
    int sample_rate;  /* 8000, 16000, 32000, 44100... */
    int sample_size;  /* bit number per sample, 8bit, 16bit... */
    int ssp;   /* Hardware SSP ID from 0,1... */
    int reserved[5];         ///< Reserved words
} gm_audio_render_sys_info_t;

#define FILE_VCH_NUMBER         128
#define CAPTURE_VCH_NUMBER      128
#define LCD_VCH_NUMBER          6
#define AUDIO_GRAB_VCH_NUMBER   32
#define AUDIO_RENDER_VCH_NUMBER 32

typedef struct {
    unsigned int graph_type;
    unsigned int gm_lib_version;
    gm_file_sys_info_t file[FILE_VCH_NUMBER];
    gm_cap_sys_info_t cap[CAPTURE_VCH_NUMBER];
    gm_lcd_sys_info_t lcd[LCD_VCH_NUMBER];
    gm_audio_grab_sys_info_t au_grab[AUDIO_GRAB_VCH_NUMBER];
    gm_audio_render_sys_info_t au_render[AUDIO_RENDER_VCH_NUMBER];
    int reserved[5];         ///< Reserved words
} gm_system_t;


/***********************************
 *       Poll Structure
 ***********************************/
#define GM_POLL_READ            1
#define GM_POLL_WRITE           2

typedef struct {
    unsigned int event;
    unsigned int bs_len;
    unsigned int extra_len;
    unsigned int keyframe;  // 1: keyframe, 0: non-keyframe
} gm_ret_event_t;

#define GM_FD_MAX_PRIVATE_DATA         5
typedef struct {
    void *bindfd;    ///< The return value of gm_bind
    unsigned int event;     ///< The poll event ID,
    gm_ret_event_t revent;  ///< The returned event value
    int fd_private[GM_FD_MAX_PRIVATE_DATA];  ///< Library internal data, don't use it!
} gm_pollfd_t;

/***********************************
 *  Encode/Decode Bitstream Structure
 ***********************************/
#define GM_FLAG_NEW_FRAME_RATE  (1 << 3) ///< Indicate the bitstream of new frame rate setting
#define GM_FLAG_NEW_GOP         (1 << 4) ///< Indicate the bitstream of new GOP setting
#define GM_FLAG_NEW_DIM         (1 << 6) ///< Indicate the bitstream of new resolution setting
#define GM_FLAG_NEW_BITRATE     (1 << 7) ///< Indicate the bitstream of new bitrate setting

typedef struct gm_enc_bitstream {
    /* provide by application */
    char *bs_buf;             ///< Bitstream buffer pointer
    unsigned int bs_buf_len;  ///< AP provide bs_buf max length
    char *extra_buf;          ///< NULL indicate no needed extra-data
    unsigned int extra_buf_len;  ///< NULL indicate no needed extra-data

    /* value return by gm_recv_bitstream() */
    unsigned int bs_len;      ///< Bitstream length
    unsigned int extra_len;   ///< Extra data length
    int keyframe;             ///< 1: keyframe 0: non-keyframe.
    unsigned int timestamp;   ///< Encode bitstream timestamp
    unsigned int newbs_flag;  ///< Flag notification of new seting, such as GM_FLAG_NEW_BITRATE
    unsigned int checksum;             ///< Checksum value
    int ref_frame;            ///< 1: reference frame, CANNOT skip, 0: not reference frame, can skip,
    unsigned int slice_offset[3];      ///< multi-slice offset 1~3 (first offset is 0)
    int reserved[5];          ///< Reserved words
} gm_enc_bitstream_t;

typedef struct gm_enc_multi_bitstream {
    void *bindfd;
    gm_enc_bitstream_t bs;
    int retval;  ///< less than 0: recv bistream fail.
    int reserved[6];         ///< Reserved words

#define GM_ENC_MAX_PRIVATE_DATA  29
    int enc_private[GM_ENC_MAX_PRIVATE_DATA]; ///< Library internal data, don't use it!
} gm_enc_multi_bitstream_t;

typedef enum {
    TIME_ALIGN_ENABLE  = 0xFEFE01FE,
    TIME_ALIGN_DISABLE = 0xFEFE07FE, 
    TIME_ALIGN_USER    = 0xFEFE09FE
} time_align_t;

/* NOTE: need 8bytes align for (* vpdDinBs_t)->timestamp */
typedef struct gm_dec_multi_bitstream {
    void *bindfd;
    char *bs_buf;
    unsigned int bs_buf_len;
    int retval;  ///< less than 0: send bistream fail.
    /* time_align: 
        TIME_ALIGN_ENABLE(default): playback time align by LCD period (ex. 60HZ is 33333us)
        TIME_ALIGN_DISABLE: play timestamp by gm_send_multi_bitstreams called
        TIME_ALIGN_USER: start to play at previous play point + time_diff(us)
     */
    time_align_t time_align;
    unsigned int time_diff; ///< time_diff(us): playback interval time by micro-second
    int reserved[4];         ///< Reserved words

#define GM_DEC_MAX_PRIVATE_DATA         23
    int dec_private[GM_DEC_MAX_PRIVATE_DATA];   ///< Library internal(vpd_put_copy_din_t) data, don't use it!
} gm_dec_multi_bitstream_t;

/***********************************
 *        Clear Window Structure
 ***********************************/
typedef enum {
    GM_ACTIVE_BY_APPLY,
    GM_ACTIVE_IMMEDIATELY,
} gm_clear_window_mode_t;

typedef struct {
    int in_w;   ///< minimun input dim: 64x32
    int in_h;   ///< minimun input dim: 64x32
    gm_format_t in_fmt;
    unsigned char *buf;
    int out_x;
    int out_y;
    int out_w;
    int out_h;
    gm_clear_window_mode_t  mode;
    int reserved[5];
} gm_clear_window_t;

/***********************************
 *        Snapshot Structure
 ***********************************/
typedef struct snapshot { /* encode snapshot JPEG */
    void *bindfd;
    int image_quality;  ///< The value of JPEG quality from 1(worst) ~ 100(best)
    char *bs_buf;
    unsigned int bs_buf_len; ///< User given parepred bs_buf length, gm_lib returns actual length
    int bs_width;   ///< bitstream width, support range 128 ~ 720
    int bs_height;  ///< bitstream height, support range 96 ~ 576

    int reserved1[3];
    unsigned int timestamp;   ///< bitstream timestamp
    int reserved2[2];
} snapshot_t;

typedef struct dec_snapshot { /* decode snapshot JPEG */
    void *bindfd;
    int image_quality;  ///< The value of JPEG quality from 1(worst) ~ 100(best)
    char *bs_buf;
    unsigned int bs_buf_len; ///< User given parepred bs_buf length, gm_lib returns actual length
    int bs_width;   ///< bitstream width, support range 128 ~ 720
    int bs_height;  ///< bitstream height, support range 96 ~ 576
    int reserved1[3];

    unsigned int timestamp;   ///< bitstream timestamp
    int reserved2[2];
} dec_snapshot_t;

#define SNAPSHOT_JPEG   0
#define SNAPSHOT_H264   1

typedef enum {
    GM_SNAPSHOT_P_FRAME = 0,
    GM_SNAPSHOT_I_FRAME = 2,
} slice_type_t;

typedef struct disp_snapshot {
    int lcd_vch;    ///< only support LCD0 now
    int image_quality;  ///< JPEG quality 1(worst) ~ 100(best), H.264 quant 1 ~ 51
    char *bs_buf;
    unsigned int bs_buf_len; ///< User given parepred bs_buf length, gm_lib returns actual length
    int bs_width;   ///< bitstream width, support range 128 ~ 720
    int bs_height;  ///< bitstream height, support range 96 ~ 576
    int bs_type;    ///< bitstream type ==> 0: SNAPSHOT_JPEG  1: SNAPSHOT_H264
    int slice_type; ///< frame type==> GM_SNAPSHOT_P_FRAME/GM_SNAPSHOT_I_FRAME
    int crop_enabled;
    gm_rect_t crop_rect;    ///< crop_enabled to do source crop
    unsigned int timestamp;   ///< bitstream timestamp
    int reserved[4];
} disp_snapshot_t;

/***********************************
 *        Decode Keyframe Structure
 ***********************************/
typedef struct decode_keyframe {
    int  bs_width;   ///< bitstream width
    int  bs_height;  ///< bitstream height
    char *bs_buf;
    int  bs_buf_len;

    int  yuv_width;   ///< Support YUV422 Only
    int  yuv_height;
    char *yuv_buf;
    int  yuv_buf_len;
    int reserved[5];
} decode_keyframe_t;

/***********************************
 *       Encode rate control
 ***********************************/
typedef enum {
    GM_CBR = 1, ///< Constant Bitrate
    GM_VBR,     ///< Variable Bitrate
    GM_ECBR,    ///< Enhanced Constant Bitrate
    GM_EVBR,    ///< Enhanced Variable Bitrate
} gm_enc_ratecontrol_mode_t;

typedef struct {
    gm_enc_ratecontrol_mode_t mode;
    int gop;        //default value = 30
    int init_quant; //default value = MPEG4(25), H264(30)
    int min_quant;  //default value = MPEG4(1), H264(1)
    int max_quant;  //default value = MPEG4(31), H264(51)
    int bitrate;
    int bitrate_max;
    int reserved[5];         ///< Reserved words
} gm_enc_ratecontrol_t;

/***********************************
 *       Get Raw Data
 ***********************************/
typedef struct rawdata {	
    gm_dim_t virtual_bg_dim;
    gm_rect_t virtual_crop;
    gm_dim_t out_dim;
    char *yuv_buf;
    unsigned int yuv_buf_len;
    int reserved[1];
} region_rawdata_t;


/***********************************
 *        Palette Structure
 ***********************************/
#define GM_MAX_PALETTE_IDX_NUM  16
typedef struct {
    int palette_table[GM_MAX_PALETTE_IDX_NUM]; ///< palette table, capture_mask: index 0 ~ 7, osg: index 0 ~ 15
} gm_palette_table_t;


/***********************************
 *        OSG Structure
 ***********************************/
typedef struct {
    unsigned int osg_image_idx;      ///< identified image index for osg
    int exist;                       ///< indicated non_exist:0, exist:1
    char *buf;                       ///< image buffer pointer     
    unsigned int buf_len;            ///< image buffer length
    gm_format_t buf_type;            ///< image buffer type
    gm_dim_t dim;                    ///< the dimension of image
    int reserved[5];                 ///< Reserved words
} gm_osg_image_t;

typedef enum {
    GM_ALIGN_TOP_LEFT = 0,
    GM_ALIGN_TOP_CENTER,
    GM_ALIGN_TOP_RIGHT,
    GM_ALIGN_BOTTOM_LEFT,
    GM_ALIGN_BOTTOM_CENTER,
    GM_ALIGN_BOTTOM_RIGHT,
    GM_ALIGN_CENTER,
} gm_align_type_t;

typedef enum {
    GM_ALPHA_0 = 0, ///< alpha 0%
    GM_ALPHA_25,    ///< alpha 25%
    GM_ALPHA_37_5,  ///< alpha 37.5%
    GM_ALPHA_50,    ///< alpha 50%
    GM_ALPHA_62_5,  ///< alpha 62.5%
    GM_ALPHA_75,    ///< alpha 75%
    GM_ALPHA_87_5,  ///< alpha 87.5%
    GM_ALPHA_100    ///< alpha 100%
} gm_alpha_t;

#define MAX_OSG_WINDOW_NUM 64   ///< per encode object
typedef struct {    
    int win_idx;    ///< Range: 0(upper layer) ~ 63(lower)
                    ///< Do unbind->apply will disable all settings
    int enabled;    ///< 0:enable, 1:disable
    int osg_img_idx;///< range:0 ~ 1023
    int x;          ///< range:0 ~ 4095 according to align_type. 
    int y;          ///< range:0 ~ 4095 according to align_type. 
    gm_alpha_t alpha;    ///< Only RGB1555 supported
    gm_align_type_t align_type;
    int reserved[5];
} gm_osg_window_t;

/***********************************
 *        MASK Structure
 ***********************************/
#define MAX_CAP_MASK_NUM 16   ///< per cap_vch
#define MAX_ENC_MASK_NUM 64   ///< per encode object
typedef struct {
    int win_idx;    ///< Capture support: 0(upper layer) ~ 15(lower)
                    ///< Encoder support: 0(upper layer) ~ 63(lower)
                    ///< Do unbind->apply will disable all settings
    int enabled;    ///< 1:enable, 0:disable
    gm_dim_t    virtual_bg_dim; ///< user-defined background plane.
    gm_rect_t   virtual_rect;   ///< user-defined area based on the virtual_bg_dim.
    gm_alpha_t alpha;
    int palette_idx;    ///< Capture support: 0 ~ 7
                        ///< Encoder support: index 0 ~ 15
    int reserved[6];
} gm_mask_t;

/***********************************
 *       Face Detection Structure
 ***********************************/
#define MAX_IVS_FDT_NUM 16
typedef struct {
    unsigned int win_idx;  ///< Range: 0 ~ 31
    unsigned int enabled;  ///< 1:enable, 0:disable
    gm_dim_t virtual_bg_dim;   ///< user-defined background plane
    gm_rect_t virtual_rect;    ///< user-defined area based on virtual_bg_dim

    unsigned char model_id;     ///< algorithm model type , range:0 ~ 15
    unsigned char en_tracking;  ///< enable tracking, 0:disable, 1:enable
    unsigned char tracking_sd;  ///< tracking smooth degree, range:0(low) ~ 5(high)
    unsigned char tracking_mrc; ///< max reset count for tracking, range:0~31
    unsigned char tracking_th;  ///< threshold for tracking, range:0(low) ~ 31(high)
    unsigned char sensitivity;  ///< sensitivity level, range:0(low) ~ 9(high)
    unsigned char x_shift;      ///< x shift number of sliding window, range:1 ~ 7
    unsigned char y_shift;      ///< y shift number of sliding window, range:1 ~ 7
    int reserved[20];
} gm_fdt_window_t;


/***********************************
 *       Attribute Structure
 ***********************************/
typedef struct {
    int data[8];
} gm_priv_t;

typedef struct {
    gm_priv_t priv; ///< Library internal data, don't use it!
    int vch;        ///< File virtual channel given by application
    int max_width;  ///< for video data, specify its max width
    int max_height; ///< for video data, specify its max height
    int max_fps;    ///< for video data, specify its max framerate
    unsigned short sample_rate;     ///< for audio data, specify its sample rate
    unsigned short sample_size;     ///< for audio data, specify its sample size
    gm_audio_channel_type_t channel_type;   ///< for audio data, specify its channel counts
    gm_format_t data_format;
    int reserved[2];         ///< Reserved words
} gm_file_attr_t;

typedef struct  {
    gm_priv_t priv; ///< Library internal data, don't use it!
    int cap_vch;    ///< Capture virtual channel
    int path;
    int reserved1[2]; ///< Reserved words, 

    unsigned int dma_path;    //DMA path 0, 1, ...
    int reserved2; ///< Reserved words, 
    unsigned int extra_buffers;  ///< extra bufffers
    int reserved3; ///< Reserved words, 
} gm_cap_attr_t;


typedef enum {
    GM_CAP_METHOD_FRAME,        ///> Get an even/odd interlacing frame
    GM_CAP_METHOD_ONE_FIELD,    ///> Get a field, skip another
    GM_CAP_METHOD_TWO_FIELD     ///> Get and output every field
} gm_cap_method_t;

typedef struct {
    gm_priv_t priv; ///< Library internal data, don't use it!
    int out_dim_enable;
    gm_dim_t out_dim;   ///< indicate liveview capture output dim
    gm_cap_method_t method; ///> the method to get an interlace frame (no use for progressive)
    int reserved[6];
} gm_cap_advanced_attr_t;

typedef struct  {
    gm_priv_t priv; ///< Library internal data, don't use it!
    int enabled;
    gm_dim_t virtual_bg_dim;
    gm_rect_t virtual_rect;
    char reserved[20];         ///< Reserved words
} gm_crop_attr_t;

typedef struct  {
    gm_priv_t priv; ///< Library internal data, don't use it!
    int enabled;
    gm_dim_t virtual_bg_dim;
    gm_rect_t virtual_rect;
    char reserved[20];         ///< Reserved words
} gm_pip_crop_attr_t;


typedef struct  {
    gm_priv_t priv; ///< Library internal data, don't use it!
    int deinterlace;  ///< 0(disabled), 1(temporal+spatial), 2(spatial only)
    int denoise;  ///< < 0(disabled), 1(temporal+spatial), 2(spatial only), 3(temporal only)
    int sharpness;  ///< 0(disabled), 1(enabled)
    int reserved[4];         ///< Reserved words
} gm_didn_attr_t; //set to capture object

typedef struct  {
    gm_priv_t priv; ///< Library internal data, don't use it!
    unsigned int spnr; ///< spatial denoise level, range:0(low) ~ 100(high)
    unsigned int tmnr; ///< temporal denoise level, range:0(low) ~ 64(high)
    unsigned int sharpness; ///< sharpness level, range:0(low) ~ 100(high)
    int reserved[4];         ///< Reserved words
} gm_image_quality_attr_t;

typedef enum {
    GM_ROTATE_NONE,
    GM_ROTATE_LEFT_90_DEGREE,
    GM_ROTATE_RIGHT_90_DEGREE
} gm_rotation_t;

typedef enum {
    GM_FLIP_NONE,
    GM_FLIP_HORIZONTAL,
    GM_FLIP_VERTICAL,
    GM_FLIP_VERTICAL_AND_HORIZONTAL
} gm_flip_t;

typedef struct  {
    gm_priv_t priv; ///< Library internal data, don't use it!
    gm_rotation_t direction;///< Active sequence: flip-->rotate direction
    gm_flip_t flip;         ///< Active sequence: flip-->rotate direction
    int reserved[5];        ///< Reserved words
} gm_rotation_attr_t;

#define GM_LCD0      0  ///< Indicate lcd_vch value
#define GM_LCD1      1  ///< Indicate lcd_vch value
#define GM_LCD2      2  ///< Indicate lcd_vch value
#define GM_LCD3      3  ///< Indicate lcd_vch value
#define GM_LCD4      4  ///< Indicate lcd_vch value
#define GM_LCD5      5  ///< Indicate lcd_vch value

typedef enum {
    WIN_LAYER1 = 0,         // background
    WIN_LAYER2,             // second layer
    COUNT_OF_WIN_LAYERS
} gm_win_layer_t;

typedef struct {
    gm_priv_t priv; ///< Library internal data, don't use it!
    int lcd_vch;   ///< Display virtual channel
    int visible;
    gm_rect_t rect;
    gm_win_layer_t layer;
    int reserved[6];         ///< Reserved words
} gm_win_attr_t;

typedef struct {
    gm_priv_t priv; ///< Library internal data, don't use it!
    int visible;
    gm_rect_t rect;
    int reserved[6];         ///< Reserved words
} gm_pip_win_attr_t;

typedef struct {
    gm_priv_t priv;
    int enabled;
    int palette_idx;  ///< Display window border color, GM8210 palette index 0~7
    int reserved[5];         ///< Reserved words
} gm_win_aspect_ratio_attr_t;

typedef enum {
    GM_H264E_DEFAULT_PROFILE = 0, 
    GM_H264E_BASELINE_PROFILE = 66, 
    GM_H264E_MAIN_PROFILE = 77,
    GM_H264E_HIGH_PROFILE = 100
} gm_h264e_profile_t;

typedef enum {
    GM_H264E_DEFAULT_LEVEL = 0, 
    GM_H264E_LEVEL_1 = 10,
    GM_H264E_LEVEL_1_1 = 11,
    GM_H264E_LEVEL_1_2 = 12,
    GM_H264E_LEVEL_1_3 = 13,
    GM_H264E_LEVEL_2 = 20,
    GM_H264E_LEVEL_2_1 = 21,
    GM_H264E_LEVEL_2_2 = 22,
    GM_H264E_LEVEL_3 = 30,
    GM_H264E_LEVEL_3_1 = 31,
    GM_H264E_LEVEL_3_2 = 32,
    GM_H264E_LEVEL_4 = 40,
    GM_H264E_LEVEL_4_1 = 41,
    GM_H264E_LEVEL_4_2 = 42,
    GM_H264E_LEVEL_5 = 50,
    GM_H264E_LEVEL_5_1 = 51
} gm_h264e_level_t;

typedef enum {
    GM_H264E_DEFAULT_CONFIG = 0, 
    GM_H264E_PERFORMANCE_CONFIG = 1,
    GM_H264E_LIGHT_QUALITY_CONFIG = 2,
    GM_H264E_QUALITY_CONFIG = 3
} gm_h264e_config_t;

typedef enum {
    GM_H264E_DEFAULT_CODING = 0, 
    GM_H264E_CAVLC_CODING = 1,
    GM_H264E_CABAC_CODING = 2,
} gm_h264e_coding_t;

typedef enum  {
    GM_CHECKSUM_NONE = 0x0,           // no checksum
    GM_CHECKSUM_ALL_CRC = 0x101,      // all frames use crc method
    GM_CHECKSUM_ALL_SUM = 0x0102,     // all frames use sum method
    GM_CHECKSUM_ALL_4_BYTE = 0x103,   // all frames use 4 bytes sum method
    GM_CHECKSUM_ONLY_I_CRC = 0x201,   // only I frames use crc method
    GM_CHECKSUM_ONLY_I_SUM = 0x0202,  // only I frames use sum method
    GM_CHECKSUM_ONLY_I_4_BYTE = 0x203 // only I frames use 4 bytes sum method
} gm_checksum_type_t;

typedef enum  {
    GM_FASTFORWARD_NONE = 0,           // no fastforward
    GM_FASTFORWARD_1_FRAME = 2,        // skip 1 frame
    GM_FASTFORWARD_3_FRAMES = 4        // skip 3 frames
} gm_fast_forward_t;

typedef struct {
    gm_priv_t priv; ///< Library internal data, don't use it!
    gm_dim_t dim;
    union {
        int framerate;
        struct {
            int numerator:16;
            int denominator:16;
        } fps_ratio;
    } frame_info;
    gm_enc_ratecontrol_t ratectl;
    int reserved0[2];
    struct {
        gm_h264e_profile_t profile:8;
        gm_h264e_level_t level:8;
        gm_h264e_config_t config:8;
        gm_h264e_coding_t coding:8;
    } profile_setting;
    struct {
        char ip_offset:8;    
        char roi_delta_qp:8;
        char reserved1:8;
        char reserved2:8;
    } qp_offset;
    gm_checksum_type_t checksum_type;
    gm_fast_forward_t fast_forward;
    int reserved1[1];         ///< Reserved words
} gm_h264e_attr_t;

typedef struct {
    gm_priv_t priv; ///< Library internal data, don't use it!
    gm_dim_t dim;
    union {
        int framerate;
        struct {
            int numerator:16;
            int denominator:16;
        } fps_ratio;
    } frame_info;
    gm_enc_ratecontrol_t ratectl;
    int reserved[5];         ///< Reserved words
} gm_mpeg4e_attr_t;

typedef struct {
    gm_priv_t priv; ///< Library internal data, don't use it!
    gm_dim_t dim;
    union {
        int framerate;
        struct {
            int numerator:16;
            int denominator:16;
        } fps_ratio;
    } frame_info;
    int quality;   ///< 1(bad quality) ~ 100(best quality)
    gm_enc_ratecontrol_mode_t mode;
    int bitrate;
    int bitrate_max;
    int reserved[2];         ///< Reserved words
} gm_mjpege_attr_t;

typedef struct  {
    gm_priv_t priv; ///< Library internal data, don't use it!
    int multi_slice; ///< multi slice number, max value is 4
    int field_coding; ///< 1:enable 0:disable
    int gray_scale; ///< 1:enable 0:disable
    int reserved[5];         ///< Reserved words
} gm_h264_advanced_attr_t;

#define GM_MAX_ROI_QP_NUMBER   8
typedef struct  {
    gm_priv_t priv; ///< Library internal data, don't use it!
    int enabled;
    gm_rect_t rect[GM_MAX_ROI_QP_NUMBER];
    unsigned int roi_root_time;  ///< root time from mdt's timestamp or others.
    int reserved[5];         ///< Reserved words
} gm_h264_roiqp_attr_t;

typedef struct  {
    gm_priv_t priv; ///< Library internal data, don't use it!
    int pattern;
    int reserved[5];         ///< Reserved words
} gm_h264_watermark_attr_t;

typedef struct  {
    gm_priv_t priv; ///< Library internal data, don't use it!
    union {
        int value;
        struct {
            char matrix_coefficient;
            char transfer_characteristics;
            char colour_primaries;
            char full_range : 1;
            char video_format : 3;
            char timing_info_present_flag : 1;
      } param;
    } param_info;
    union {
        int value;
        struct {
          unsigned short sar_width;
          unsigned short sar_height;
        } sar;
    } sar_info;
    int reserved[5];        ///< Reserved words
} gm_h264_vui_attr_t;

typedef struct {
    gm_priv_t priv; ///< Library internal data, don't use it!
    gm_dim_t dim;
    union {
        int framerate;
        struct {
            int numerator:16;
            int denominator:16;
        } fps_ratio;
    } frame_info;
    int reserved[5];         ///< Reserved words
} gm_raw_attr_t;

typedef struct  {
    gm_priv_t priv; ///< Library internal data, don't use it!
    int vch;
    int sample_rate;
    int sample_size;
    gm_audio_channel_type_t channel_type;
    int reserved[5];         ///< Reserved words
} gm_audio_grab_attr_t;

typedef enum {
    GM_PCM = 1,
    GM_AAC,
    GM_ADPCM,
    GM_G711_ALAW,
    GM_G711_ULAW
} gm_audio_encode_type_t;

typedef struct  {
    gm_priv_t priv; ///< Library internal data, don't use it!
    gm_audio_encode_type_t encode_type;
    int bitrate;  ///< ADPCM: 16000/32000  AAC: 14500~192000

    /* The count of samples per audio frame, the range is 250 ~ 2048(default).
     * Max. frame_samples can be configured in gmlib.cfg. Some codecs have restriction as following
     *   AAC: 1024*n(mono) 2048*n(stereo)
     *   ADPCM: 8*n+1(mono)  8*n+2(stereo)
     */
    int frame_samples;
    int reserved[4];         ///< Reserved words
} gm_audio_enc_attr_t;

#define SYNC_LCD_DISABLE    0xFEFEFEFE
typedef struct  {
    gm_priv_t priv; ///< Library internal data, don't use it!
    int vch;
    gm_audio_encode_type_t encode_type;

    /* for some codecs, the frame_samples has the following restriction
     *   AAC: 1024*n(mono) 2048*n(stereo)
     *   ADPCM: 8*n+1(mono)  8*n+2(stereo)
     */
    int frame_samples;
    int sync_with_lcd_vch;   ///< value: SYNC_LCD_DISABLE, GM_LCD0, GM_LCD1,...
    int reserved[4];         ///< Reserved words
} gm_audio_render_attr_t;

typedef struct {
    gm_priv_t priv;                 ///< Library internal data, don't use it!
    union {
        int framerate;
        struct {
            int numerator:16;
            int denominator:16;
        } fps_ratio;
    } frame_info;
    unsigned char ratio;            ///< scaling ratio from 1st to 2nd YUV, unit:percent, range:80~91
    unsigned short first_width;     ///< width of first YUV, range: 64~320
    unsigned short first_height;    ///< height of first YUV, range: 32~240
#define FACE_BY_JPEG  0x1
    unsigned int feature;           ///< 0(none), 1(jpeg face)
    unsigned int jpeg_quality;      ///< 1(bad quality) ~ 100(best quality)
    int reserved[4];                ///< Reserved words
} gm_fdt_attr_t;

/* face detection buffer contents */
typedef struct { //28bytes
    unsigned int tag_index; ///< face tag index number
    unsigned short x;       ///< face location (x, y) under (background_width, background_height)
    unsigned short y;       ///< face location (x, y) under (background_width, background_height)
    unsigned short width;   ///< face region (width, height) under (background_width, background_height)
    unsigned short height;  ///< face region (width, height) under (background_width, background_height)
    unsigned int jpeg_bs_size;  ///< jpeg bitstream size
    unsigned int jpeg_bs_offset;///< jpeg bitstream offset
    unsigned int reserved[2];   ///< reserved
} gm_fdt_area_t;

#define MAX_OF_FDT  64
typedef struct { //~1.8K
    unsigned int background_width;
    unsigned int background_height;
    unsigned int reserved[4];
    unsigned int num_of_fdt;    ///< number of fdt element (must less MAX_OF_FDT)
    gm_fdt_area_t fdt[MAX_OF_FDT];
} gm_fdt_result_t;

/***********************************
 * gmlib internal use
 ***********************************/
int gm_init_private(int version);  ///< Library internal use, don't use it!
void gm_init_attr(void *name, const char *struct_str, int struct_size, int version); ///< Library internal use, don't use it!


/***********************************
 *       gmlib API
 ***********************************/

/*!
 * @fn int gm_init(void)
 * @brief Init gmlib
 * @return 0 on success, negative value on error
 */
static inline int gm_init(void) { return (gm_init_private(GM_VERSION_CODE)); }

/*!
 * @fn int gm_release(void)
 * @brief Release gmlib
 * @return 0 on success, negative value on error
 */
int gm_release(void);


/*!
 * @fn int gm_get_sysinfo(gm_system_t *system_info)
 * @brief Get system information
 * @param system_info Input pointer, application must prepare this memory.
 * @return 0 on success, negative value on error
 */
int gm_get_sysinfo(gm_system_t *system_info);

/*!
 * @fn void *gm_new_obj(gm_obj_type_t obj_type)
 * @brief Create a new object by obj_type
 * @param obj_type Type of object
 * @return object pointer
*/
void *gm_new_obj(gm_obj_type_t obj_type);

/*!
 * @fn int gm_delete_obj(void *obj)
 * @brief Delete a exist object
 * @param obj Pointer of object
 * @return 0 on success, negative value on error
*/
int gm_delete_obj(void *obj);

/*!
 * @fn void *gm_bind(void *groupfd, void *in_obj, void *out_obj)
 * @brief Bind in object to out object to groupfd
 * @param groupfd The fd of group
 * @param in_obj Input object pointer
 * @param out_obj Out object pointer
 * @return The fd of binding result
 */
void *gm_bind(void *groupfd, void *in_obj, void *out_obj);

/*!
 * @fn int gm_unbind(void *bindfd)
 * @brief Unbind the bindfd
 * @param bindfd The fd of gm_bind
 * @return 0 on success, negative value on error
 */
int gm_unbind(void *bindfd);

/*!
 * @fn void *gm_new_groupfd(void)
 * @brief Create a new group
 * @return The fd of group
 */
void *gm_new_groupfd(void);

/*!
 * @fn void gm_delete_groupfd(void *groupfd)
 * @brief Delete a group by groupfd
 * @param groupfd The fd of group
 */
int gm_delete_groupfd(void *groupfd);

/*!
 * @fn void DECLARE_ATTR(var, structure)
 * @brief Declare a attribyte
 * @param var AP definied variable
 * @param structure Structure of attribute
 */
#define DECLARE_ATTR(var, structure) \
    structure var = ({gm_init_attr(&var, #structure, sizeof(structure), GM_VERSION_CODE); var;})

/*!
 * @fn int gm_set_attr(void *obj, void *attr)
 * @brief Set object's attribute for object
 * @param obj Object pointer
 * @param attr Attribute pointer
 * @return 0 on success, negative value on error
 */
int gm_set_attr(void *obj, void *attr);

/*!
 * @fn int gm_apply(void *groupfd)
 * @brief Apply a groupfd
 * @param groupfd The fd of group
 * @return 0 on success, negative value on error
 */
int gm_apply(void *groupfd);

/*!
 * @fn int gm_apply_attr(void *bindfd, void *attr)
 * @brief dynamicly update attribute for the bindfd.
 * @param bindfd The return fd of gm_bind()
 * @param attr Attribute pointer
 * @return 0 on success, negative value on error
 */
int gm_apply_attr(void *bindfd, void *attr);

/*!
 * @fn int gm_poll(gm_pollfd_t *poll_fds, int num_fds, int timeout_millisec)
 * @brief Blocking to poll a bitstream
 * @param poll_fds The poll fd sets
 * @param num_fds Indicate number of fd sets
 * @param timeout_millisec Timeout value (millisec)
 * @return 0 on success, -4 on timeout, -1 on error
 */
int gm_poll(gm_pollfd_t poll_fds[], int num_fds, int timeout_millisec);

/*!
 * @fn int gm_send_multi_bitstreams(gm_dec_multi_bitstream_t *multi_bs, int num_bs, int timeout_millisec)
 * @brief Send multiple bitstream to playback
 * @param multi_bs the Structure of decode bitstream
 * @param num_bs Indicate number of bs sets
 * @param timeout_millisec Timeout value to send bitstream (millisecond)
 * @return 0 on success, -4 on timeout, -1 on error
 */
int gm_send_multi_bitstreams(gm_dec_multi_bitstream_t multi_bs[], int num_bs, int timeout_millisec);

/*!
 * @fn int gm_recv_multi_bitstreams(gm_enc_multi_bitstream_t *multi_bs, int num_bs)
 * @brief Get multiple encode bitstream
 * @param multi_bs the Structure of decode bitstream
 * @param num_bs Indicate number of bs sets
 * @return 0 on success, -1 on error, -2 on bs_buf too small, -3 on extra_buf too small, -4 on timeout
 */
int gm_recv_multi_bitstreams(gm_enc_multi_bitstream_t multi_bs[], int num_bs);

/*!
 * @fn int gm_clear_window(int lcd_vch, gm_clear_window_t *cw_str)
 * @brief Trigger the clear window to display
 * @param lcd_vch The vch of LCD display (GM_LCD0, GM_LCD1...)
 * @param cw_str Clear window command structure
 * @return 0 on success, -1 on error
 */
int gm_clear_window(int lcd_vch, gm_clear_window_t *cw_str);

/*!
 * @fn int gm_request_snapshot(snapshot_t *snapshot, int timeout_millisec);
 * @brief Request a snapshot JPEG
 * @param snapshot The snapshot_t structure of snapshot
 * @param timeout_millisec Timeout value to snapshot a JPEG
 * @return positive value on success(JPEG length), -1 on error, -2 on buf too small, -4 on timeout
 */
int gm_request_snapshot(snapshot_t *snapshot, int timeout_millisec);

/*!
 * @fn int gm_request_dec_snapshot(dec_snapshot_t *snapshot, int timeout_millisec);
 * @brief Request a snapshot JPEG
 * @param snapshot The snapshot_t structure of snapshot
 * @param timeout_millisec Timeout value to snapshot a JPEG
 * @return positive value on success(JPEG length), -1 on error, -2 on buf too small, -4 on timeout
 */
int gm_request_dec_snapshot(dec_snapshot_t *snapshot, int timeout_millisec);

/*!
 * @fn int gm_request_snapshot(disp_snapshot_t *disp_snapshot, int timeout_millisec);
 * @brief Request a snapshot JPEG from LCD
 * @param disp_snapshot The disp_snapshot_t structure of snapshot from LCD
 * @param timeout_millisec Timeout value to snapshot a JPEG
 * @return positive value on success(bitstream length), -1 on error, -2 on buf too small, -4 on timeout
 */
int gm_request_disp_snapshot(disp_snapshot_t *disp_snapshot, int timeout_millisec);


/*!
 * @fn gm_request_keyframe(void *bindfd)
 * @brief Request a keyframe
 * @param bindfd The return fd of gm_bind()
 * @return 0 on success, negative value on error
 */
int gm_request_keyframe(void *bindfd);

/*!
 * @fn int gm_get_rawdata(void *obj, gm_rect_t region)
 * @brief Get capture raw data of region
 * @param obj ,cap or enc obj
 * @param region_rawdata The raw data of region
 * @param timeout_millisec Timeout value (millisec)
 * @return positive value on success, -1 on error, -2 on buffer too small, -4 on timeout
 */
int gm_get_rawdata(void *obj, region_rawdata_t *region_rawdata, int timeout_millisec);

/*!
 * @fn int gm_decode_keyframe(decode_keyframe_t *dec)
 * @brief Decode the keyframe into yuv buffer
 * @param dec The decode_keyframe_t structure of decoding keyframe
 * @return 0 on success, -1 on error, -2 on buffer too small
 */
int gm_decode_keyframe(decode_keyframe_t *dec);


/*!
 * @fn int gm_set_palette_table(gm_palette_table_t *palette)
 * @brief Set global palette table
 * @param palette Fill color with YCrYCb format to this palette table for usage.
 * @return 0 on success, -1 on error
 */
int gm_set_palette_table(gm_palette_table_t *palette);

/*!
 * @fn int gm_set_osg_images(gm_osg_image_t *images, int num_images);
 * @brief Set image to osg
 * @param images The multiple of image for osg
 * @param num_imgages Number of images
 * @return 0 on success, -1 on error
 */
int gm_set_osg_images(gm_osg_image_t images[], int num_images);

/*!
 * @fn int gm_set_osg_windows(void *obj, gm_osg_window_t osg_windows[], int num_osg_windows)
 * @brief Paste osg image on screen for encode
 * @param obj Object pointer of GM_ENCODER_OBJECT
 * @param osg_windows The multiple of osg window for osg
 * @param num_osg_windows Number of osg windows
 * @return 0 on success, -1 on error
 */
int gm_set_osg_windows(void *obj, gm_osg_window_t osg_windows[], int num_osg_windows);

/*!
 * @fn int gm_set_masks(void *obj, gm_mask_t masks[], int num_masks)
 * @brief Paste mask on screen for liveview or encode
 * @param obj Object pointer of GM_CAP_OBJECT or GM_ENCODER_OBJECT
 * @param masks The multiple of mask for mask
 * @param num_masks Number of mask
 * @return 0 on success, -1 on error
 */
int gm_set_masks(void *obj, gm_mask_t masks[], int num_masks);

/*!
 * @fn int gm_set_display_rate(int lcd_vch, int display_rate)
 * @brief Set frame rate to display
 * @param lcd_vch The vch of LCD display (GM_LCD0, GM_LCD1...)
 * @param display_rate Target frame rate
 * @return 0 on success, -1 on error
 */
int gm_set_display_rate(int lcd_vch, int display_rate);

/***********************************
 *        Notifications
 ***********************************/
typedef enum {
    GM_NOTIFY_SIGNAL_LOSS = 0,
    GM_NOTIFY_SIGNAL_PRESENT,
    GM_NOTIFY_FRAMERATE_CHANGE,
    GM_NOTIFY_HW_CONFIG_CHANGE,
    GM_NOTIFY_TAMPER_ALARM,
    GM_NOTIFY_TAMPER_ALARM_RELEASE,
    GM_NOTIFY_AUDIO_BUFFER_UNDERRUN,
    GM_NOTIFY_AUDIO_BUFFER_OVERRUN,
    MAX_GM_NOTIFY_COUNT,
} gm_notify_t;

/*!
 * @fn int gm_notify_handler_t(gm_obj_type_t obj_type, gm_notify_t notify, int vch)
 * @brief callback function for notify
 * @param groupfd The fd of group
 * @param obj_type the object which sends the notification
 * @param vch the channel that occurs the notification
 * @param notify the type of this notification
 * @param data the private data from registered function
 * @return return none
 */
typedef void (*gm_notify_handler_t)(void *groupfd, gm_obj_type_t obj_type, int vch,
                                    gm_notify_t notify, void *data);


/*!
 * @fn int gm_register_group_notify_handler(void *groupfd, gm_notify_t notify, gm_notify_handler_t fn_notify_handler)
 * @brief the notification that you want to receive from the specified group
 * @param groupfd The fd of group
 * @param notify the type of this notification
 * @param fn_notify_handler callback function for notify, 0 means deregister
 * @return positive value on success, -1 on error
 */
int gm_register_group_notify_handler(void *groupfd, gm_notify_t notify,
                                     gm_notify_handler_t fn_notify_handler, void *data);

/*!
 * @fn int gm_set_win_pattern(unsigned int index, gm_format_t format, char *pattern, unsigned int width, unsigned int height)
 * @brief Set default pattern by index
 * @param index The index of this pattern: 0(reserved), 1(index 0), >2(not support yet)
 * @param format The image format of this pattern (support GM_YUV422)
 * @param pattern The image pattern support only YUV422 patterns
 * @param width The width of this pattern (default maximum 320)
 * @param height The height of this pattern (default maximum 240)
 * @return 0 on success, -1 on error
 */
int gm_set_win_pattern(unsigned int index, gm_format_t format, char *pattern, unsigned int width,
                       unsigned int height);

/*!
 * @fn int gm_enable_win_pattern(void *win_obj, int index)
 * @brief Select the defaut pattern for the specified win object
 * @param win_obj The specified win object
 * @param index The index of pattern: 0(disable), 1(index 0), >2(not support yet)
 * @return 0 on success, -1 on error
 */
int gm_enable_win_pattern(void *win_obj, unsigned int index);

/*!
 * @fn int gm_set_fdt_windows(void *obj, gm_fdt_window_t fdt_windows[], int num_fdt_windows)
 * @brief face detection and return detected area
 * @param obj Object pointer of GM_ENCODER_OBJECT
 * @param fdt_windows The multiple of fdt window 
 * @param num_fdt_windows Number of fdt windows
 * @return 0 on success, -1 on error
*/
int gm_set_fdt_windows(void *obj, gm_fdt_window_t fdt_windows[], int num_fdt_windows);

#ifdef __cplusplus
}
#endif

#endif
