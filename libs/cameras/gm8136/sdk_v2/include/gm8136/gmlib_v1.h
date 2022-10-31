#ifndef __GMLIB_V1_H__
#define __GMLIB_V1_H__

/***********************************
 *        moditon detection
 ***********************************/
typedef struct {
    unsigned int id;    ///< specified motion parameter ID, such as VCAP_API_MD_PARAM_ALPHA...
    unsigned int value;
} gm_cap_motion_t;

typedef struct {
    unsigned short tamper_sensitive_b; ///< Tamper detection black sensitive, 0 ~ 100, 0: for disable
    unsigned short tamper_threshold; ///< Tamper detection luminance index, 1 ~ 255
    unsigned short tamper_sensitive_h; ///< Tamper detection homogenous sensitive, 0 ~ 100, 0: for disable
    int reserved[5];         ///< Reserved words
} gm_cap_tamper_t;

#define CAP_MOTION_SIZE     4096
typedef struct {
    /* provide md_buf by application, the max buf size is 4096*/
    char *md_buf;
    int md_buf_len;

    /* value return by gm_recv_multi_cap_md() */
    int is_valid;       ///< 1:valid, 0:invalid(It means cap_md has not been ready on this bindfd.)
    int md_len;
    gm_dim_t md_dim;    ///< MD Region dim
    gm_dim_t md_mb;     ///< MD Macro block
} gm_cap_md_info_t;

typedef struct gm_multi_cap_md {
    void *bindfd;
    gm_cap_md_info_t cap_md_info;
    int retval;   ///< less than 0: recv cap_md fail.

#define GM_MD_MAX_PRIVATE_DATA         16
    int cap_md_private[GM_MD_MAX_PRIVATE_DATA]; ///< Library internal data, don't use it!
} gm_multi_cap_md_t;

/***********************************
 *        OSD Structure
 ***********************************/
typedef enum {
    GM_OSD_ALIGN_TOP_LEFT = 0,
    GM_OSD_ALIGN_TOP_CENTER,
    GM_OSD_ALIGN_TOP_RIGHT,
    GM_OSD_ALIGN_BOTTOM_LEFT,
    GM_OSD_ALIGN_BOTTOM_CENTER,
    GM_OSD_ALIGN_BOTTOM_RIGHT,
    GM_OSD_ALIGN_CENTER,
    GM_OSD_ALIGN_NONE,                      ///< This mode is only for osd_mask with liveview_crop enabled.
} gm_osd_align_type_t;


typedef enum {
    GM_OSD_PRIORITY_MARK_ON_OSD = 0,        ///< Mark above OSD window
    GM_OSD_PRIORITY_OSD_ON_MARK             ///< Mark below OSD window
} gm_osd_priority_t;

typedef enum {
    GM_OSD_FONT_SMOOTH_LEVEL_WEAK = 0,      ///< weak smoothing effect
    GM_OSD_FONT_SMOOTH_LEVEL_STRONG         ///< strong smoothing effect
} gm_osd_font_smooth_level_t;

typedef struct {
    int enabled;    ///< OSD font smooth enable/disable 1 : enable, 0 : disable
    gm_osd_font_smooth_level_t level;      ///< OSD font smooth level
    int reserved[5];         ///< Reserved words
} gm_osd_smooth_t;

typedef enum {
    GM_OSD_MARQUEE_MODE_NONE = 0, ///< no marueee effect
    GM_OSD_MARQUEE_MODE_HLINE,    ///< one horizontal line marquee effect
    GM_OSD_MARQUEE_MODE_VLINE,    ///< one vertical line marquee effect
    GM_OSD_MARQUEE_MODE_HFLIP     ///< one horizontal line flip effect
} gm_osd_marquee_mode_t;

typedef enum {
    GM_OSD_MARQUEE_LENGTH_1024 = 0,   ///< 1024 step
    GM_OSD_MARQUEE_LENGTH_512,    ///< 512  step
    GM_OSD_MARQUEE_LENGTH_256,    ///< 256  step
    GM_OSD_MARQUEE_LENGTH_128,    ///< 128  step
    GM_OSD_MARQUEE_LENGTH_64,     ///< 64   step
    GM_OSD_MARQUEE_LENGTH_32,     ///< 32   step
    GM_OSD_MARQUEE_LENGTH_16,     ///< 16   step
    GM_OSD_MARQUEE_LENGTH_8,      ///< 8    step
    GM_OSD_MARQUEE_LENGTH_4       ///< 4    step
} gm_osd_marquee_length_t;

typedef struct {
    gm_osd_marquee_mode_t mode;
    /* Loop_time = (length(1024/512/256...) * (speed + 1)) / (30 or 60) */
    gm_osd_marquee_length_t length;  ///< OSD marquee length control
    int speed;   ///< OSD marquee speed  control, 1~3, 1:fastest, 3:slowest
    int reserved[5];         ///< Reserved words
} gm_osd_marquee_param_t;

typedef enum {
    GM_OSD_BORDER_TYPE_WIN = 0,     ///< treat border as window
    GM_OSD_BORDER_TYPE_FONT         ///< treat border as font
} gm_osd_border_type_t;

typedef struct {
    int enabled;        ///1:enable,0:disable
    int width;          ///< OSD window border width, 0~7, border width = 4x(n+1) pixels
    gm_osd_border_type_t type;  ///< OSD window border transparency type
    int palette_idx;    ///< OSD window border color, palette index 0~15
    int reserved[5];         ///< Reserved words
} gm_osd_border_param_t;

typedef enum {
    GM_OSD_FONT_ALPHA_0 = 0,  ///< alpha 0%
    GM_OSD_FONT_ALPHA_25,     ///< alpha 25%
    GM_OSD_FONT_ALPHA_37_5,   ///< alpha 37.5%
    GM_OSD_FONT_ALPHA_50,     ///< alpha 50%
    GM_OSD_FONT_ALPHA_62_5,   ///< alpha 62.5%
    GM_OSD_FONT_ALPHA_75,     ///< alpha 75%
    GM_OSD_FONT_ALPHA_87_5,   ///< alpha 87.5%
    GM_OSD_FONT_ALPHA_100     ///< alpha 100%
} gm_osd_font_alpha_t;

typedef enum {
    GM_OSD_FONT_ZOOM_NONE = 0,      ///< disable zoom
    GM_OSD_FONT_ZOOM_2X,   ///< horizontal and vertical zoom in 2x
    GM_OSD_FONT_ZOOM_3X,   ///< horizontal and vertical zoom in 3x
    GM_OSD_FONT_ZOOM_4X,   ///< horizontal and vertical zoom in 4x
    GM_OSD_FONT_ZOOM_ONE_HALF,      ///< horizontal and vertical zoom out 2x

    GM_OSD_FONT_ZOOM_H2X_V1X = 8,   ///< horizontal zoom in 2x
    GM_OSD_FONT_ZOOM_H4X_V1X,  ///< horizontal zoom in 4x
    GM_OSD_FONT_ZOOM_H4X_V2X,  ///< horizontal zoom in 4x and vertical zoom in 2x

    GM_OSD_FONT_ZOOM_H1X_V2X = 12,  ///< vertical zoom in 2x
    GM_OSD_FONT_ZOOM_H1X_V4X,  ///< vertical zoom in 4x
    GM_OSD_FONT_ZOOM_H2X_V4X,  ///< horizontal zoom in 2x and vertical zoom in 4x

    GM_OSD_FONT_ZOOM_2X_WITH_EDGE = 20,   ///< zoom in 2x with edge mode (2-pixel), GM8136 Only, influence all windows
    GM_OSD_FONT_ZOOM_4X_WITH_EDGE,   ///< zoom 4x with edge mode (2-pixel), GM8136 Only, influence all windows
} gm_osd_font_zoom_t;

#define GM_MAX_OSD_FONTS        31

/* gm_osd_font_t supoprts maximum GM_MAX_OSD_FONTS=31 fonts in single windows */
typedef struct {
    int win_idx;
    int enabled;
    gm_osd_align_type_t align_type;
    unsigned int x;     ///< The x axis in OSD window (pixel unit)
    unsigned int y;     ///< The y axis in OSD window (pixel unit)
    unsigned int h_words;   ///< The horizontal words in OSD window (word unit)
    unsigned int v_words;   ///< The vertical words in OSD window (word unit)
    unsigned int h_space;   ///< The horizontal space between charater and charater (pixel unit)
    unsigned int v_space;   ///< The vertical space between charater and charater (pixel unit)
    int font_index_len;     ///< Indicate used length of font index

    unsigned short font_index[GM_MAX_OSD_FONTS];   ///< OSD font index table in this OSD window
    gm_osd_font_alpha_t font_alpha; // GM8210:all font use the same color  GM8287:fonts can diff colors
    gm_osd_font_alpha_t win_alpha;

    int font_palette_idx;   ///< font color, GM8120 index from 0~15
    int win_palette_idx;    ///< window background color, GM8120 index from 0~15

    gm_osd_priority_t priority;  ///< OSD & Mark layer spriority
    gm_osd_smooth_t smooth;
    gm_osd_marquee_param_t marquee;
    gm_osd_border_param_t border;
    gm_osd_font_zoom_t font_zoom;
    int reserved[5];         ///< Reserved words
} gm_osd_font_t;

/* gm_osd_font2_t support dynamic font_index with user specified font number,
   compared with gm_osd_font_t */
typedef struct {
    int win_idx;
    int enabled;
    gm_osd_align_type_t align_type;
    unsigned int x;     ///< The x axis in OSD window (pixel unit)
    unsigned int y;     ///< The y axis in OSD window (pixel unit)
    unsigned int h_words;   ///< The horizontal words in OSD window (word unit)
    unsigned int v_words;   ///< The vertical words in OSD window (word unit)
    unsigned int h_space;   ///< The horizontal space between charater and charater (pixel unit)
    unsigned int v_space;   ///< The vertical space between charater and charater (pixel unit)
    int font_index_len;     ///< Indicate used length of font index
    unsigned short *font_index;   ///< OSD font index table in this OSD window, allocated by AP
    gm_osd_font_alpha_t font_alpha;
    gm_osd_font_alpha_t win_alpha;
    int font_palette_idx;   ///< font color, GM8120 index from 0~15
    int win_palette_idx;    ///< window background color, GM8120 index from 0~15
    gm_osd_priority_t priority;     ///< OSD & Mark layer spriority
    gm_osd_smooth_t smooth;
    gm_osd_marquee_param_t marquee;
    gm_osd_border_param_t border;
    gm_osd_font_zoom_t font_zoom;
    int reserved[5];         ///< Reserved words
} gm_osd_font2_t;

#define GM_OSD_FONT_MAX_ROW 18
typedef struct {
    int font_idx;                                 ///< font index, 0 ~ (osd_char_max - 1)
    unsigned short  bitmap[GM_OSD_FONT_MAX_ROW];    ///< GM8210 only 18 row (12 bits data + 4bits reserved)
    int reserved[5];         ///< Reserved words
} gm_osd_font_update_t;

typedef enum {
    GM_OSD_MASK_ALPHA_0 = 0, ///< alpha 0%
    GM_OSD_MASK_ALPHA_25,    ///< alpha 25%
    GM_OSD_MASK_ALPHA_37_5,  ///< alpha 37.5%
    GM_OSD_MASK_ALPHA_50,    ///< alpha 50%
    GM_OSD_MASK_ALPHA_62_5,  ///< alpha 62.5%
    GM_OSD_MASK_ALPHA_75,    ///< alpha 75%
    GM_OSD_MASK_ALPHA_87_5,  ///< alpha 87.5%
    GM_OSD_MASK_ALPHA_100    ///< alpha 100%
} gm_osd_mask_alpha_t;

typedef enum {
    GM_OSD_MASK_BORDER_TYPE_HOLLOW = 0,
    GM_OSD_MASK_BORDER_TYPE_TRUE
} gm_osd_mask_border_type_t;

typedef enum {
    GM_ALL_PATH = 0,
    GM_THIS_PATH
} gm_path_mode_t;

typedef struct {
    int width;    ///< Mask window border width when hollow on, 0~15, border width = 2x(n+1) pixels
    gm_osd_mask_border_type_t type;  ///< Mask window hollow/true
    int reserved[5];         ///< Reserved words
} gm_osd_mask_border_t;

typedef struct {
    /* GM8210/GM828x/GM8139:
        GM_ALL_PATH: apply in all capture path, implement by capture mask engine
            mask_idx= 0(lowest priority), 1, 2, ... 7 (highest priority)
        GM_THIS_PATH: apply in specified capture path, implement by capture OSD engine
                      (mask_idx & gm_osd_font_t->win_idx is indexed together)
            mask_idx= 0(highest priority), 1, 2, ... 7 (lowest priority)
    */
    int mask_idx;   ///< The mask window index
    int enabled;
    unsigned int x;    ///< Left position of mask window
    unsigned int y;    ///< Top position of mask window
    unsigned int width;    ///< The dimension width of mask window
    unsigned int height;   ///< The dimension height of mask window
    gm_osd_mask_alpha_t alpha;
    int palette_idx; ///< The mask palette_idx vlaue range:(0~3 at GM_THIS_PATH), (0~15 at GM_ALL_PATH)
    gm_osd_mask_border_t border;   ///< The feature is only available at GM_ALL_PATH.
    gm_osd_align_type_t align_type;
    int reserved[5];         ///< Reserved words
} gm_osd_mask_t;

#define GM_MAX_OSD_MARK_IMG_NUM 4
typedef enum {
    GM_OSD_MARK_DIM_16 = 0,  ///< 16  pixel/line
    GM_OSD_MARK_DIM_32,  ///< 32  pixel/line
    GM_OSD_MARK_DIM_64,  ///< 64  pixel/line
    GM_OSD_MARK_DIM_128, ///< 128 pixel/line
    GM_OSD_MARK_DIM_256, ///< 256 pixel/line
    GM_OSD_MARK_DIM_512, ///< 512 pixel/line
    GM_OSD_MARK_DIM_MAX,
} gm_osd_mark_dim_t;

typedef struct {
    int mark_exist;
    char *mark_yuv_buf;
    unsigned int mark_yuv_buf_len;
    gm_osd_mark_dim_t mark_width; ///< GM8210: (w*h*2)<16384 bytes   GM8287: (w*h*2)<8192 bytes, enable osg when width > 5
    gm_osd_mark_dim_t mark_height;///< GM8210: (w*h*2)<16384 bytes   GM8287: (w*h*2)<8192 bytes, enable osg when height > 5
    int osg_tp_color;            ///< reserved, do not use it.
    unsigned short  osg_mark_idx;      ///< mark_idx for osg
    char reserved[14];         ///< Reserved words
} gm_osd_img_param_t;

typedef struct {
    gm_osd_img_param_t mark_img[GM_MAX_OSD_MARK_IMG_NUM]; //osg mode: only mark_img[0]
    int reserved[5];         ///< Reserved words
} gm_osd_mark_img_table_t;

typedef enum {
    GM_OSD_MARK_ALPHA_0 = 0,///< alpha 0%
    GM_OSD_MARK_ALPHA_25,   ///< alpha 25%
    GM_OSD_MARK_ALPHA_37_5, ///< alpha 37.5%
    GM_OSD_MARK_ALPHA_50,   ///< alpha 50%
    GM_OSD_MARK_ALPHA_62_5, ///< alpha 62.5%
    GM_OSD_MARK_ALPHA_75,   ///< alpha 75%
    GM_OSD_MARK_ALPHA_87_5, ///< alpha 87.5%
    GM_OSD_MARK_ALPHA_100   ///< alpha 100%
} gm_osd_mark_alpha_t;

typedef enum {
    GM_OSD_MARK_ZOOM_1X = 0,///< zoom in lx
    GM_OSD_MARK_ZOOM_2X,    ///< zoom in 2X
    GM_OSD_MARK_ZOOM_4X     ///< zoom in 4x
} gm_osd_mark_zoom_t;

typedef struct {
    /* GM8210/GM828x/GM8139:
        mark_idx= 0(highest priority), 1, 2, 3 (lowest priority)
    */
    int mark_idx;   ///< The mask window index, normal:0~3, osg_mode: >= 4
    int enabled;
    unsigned int x; ///< Left position of mask window
    unsigned int y; ///< Top position of mask window
    gm_osd_mark_alpha_t alpha;
    gm_osd_mark_zoom_t zoom; ///< when osg mode, only support 0
    gm_osd_align_type_t align_type;
    unsigned short osg_mark_idx; ///< when osg mode, specify the mark image
    char reserved[18];         ///< Reserved words
} gm_osd_mark_t;


/*!
 * @fn int gm_set_osd_mark_image(gm_osd_mark_img_table_t *osd_mark_img)
 * @brief Set OSD mark
 * @param osd_mark_img The table is for loading mark image to system.
 * @return 0 on success, -1 on error
 */
int gm_set_osd_mark_image(gm_osd_mark_img_table_t *osd_mark_img);

/*!
 * @fn int gm_set_osd_mark(void *obj, gm_osd_mark_t *osd_mark)
 * @brief Set OSD mark
 * @param obj Object pointer, currently, support only GM_CAP_OBJECT
 * @param osd_mark The gm_osd_mark_t structure of OSD command
 * @return 0 on success, -1 on error
 */
int gm_set_osd_mark(void *obj, gm_osd_mark_t *osd_mark);

/*!
 * @fn int gm_set_cap_motion(int cap_vch, gm_cap_motion_t *cap_motion)
 * @brief Set motion parameter to capture
 * @param cap_vch The vch of capture
 * @param cap_motion Set parameter for capture motion detection
 * @return 0 on success, -1 on error 
 */
int gm_set_cap_motion(int cap_vch, gm_cap_motion_t *cap_motion);

/*!
 * @fn int gm_recv_multi_cap_md(gm_multi_cap_md_t *multi_cap_md, int num_cap_md)
 * @brief Get motion detection data (Multi)
 * @param multi_cap_md The array pointer specified the multi cap_md buffer and get it.
 * @param num_cap_md The number of multi cap_md.
 * @return 0 on success, -1 on error, -2 on buffer too small
 */
int gm_recv_multi_cap_md(gm_multi_cap_md_t *multi_cap_md, int num_cap_md);

/*!
 * @fn int gm_update_new_font(gm_osd_font_update_t new_font)
 * @brief update new font with specified index
 * @param gm_osd_font_update_t(font_idx) font index
 * @param gm_osd_font_update_t(bitmap)  font bitmap
 * @return positive value on success, -1 on error
 */
int gm_update_new_font(gm_osd_font_update_t *new_font);

/*!
 * @fn int gm_set_osd_font(void *obj, gm_osd_font_t *osd_font)
 * @brief Set OSD font
 * @param obj Object pointer, currently, support only GM_CAP_OBJECT
 * @param osd_font The gm_osd_font_t structure of OSD setup
 * @return 0 on success, -1 on error
 */
int gm_set_osd_font(void *obj, gm_osd_font_t *osd_font);

/*!
 * @fn int gm_set_osd_font2(void *obj, gm_osd_font2_t *osd_font)
 * @brief Set OSD font with string more than 31 bytes
 * @param obj Object pointer, currently, support only GM_CAP_OBJECT
 * @param osd_font The gm_osd_font_t structure of OSD setup
 * @return 0 on success, -1 on error
 */
int gm_set_osd_font2(void *obj, gm_osd_font2_t *osd_font);

/*!
 * @fn int gm_set_osd_mask(void *obj, gm_osd_mask_t *osd_mask, gm_path_mode_t path_mode)
 * @brief Set OSD mask
 * @param obj Object pointer, currently, support only GM_CAP_OBJECT
 * @param osd_mask The gm_osd_mask_t structure of OSD command
 * @param path_mode Specify the path of osd_mask
 * @return 0 on success, -1 on error
 */
int gm_set_osd_mask(void *obj, gm_osd_mask_t *osd_mask, gm_path_mode_t path_mode);

/*!
 * @fn int gm_set_cap_tamper(int cap_vch, gm_cap_tamper_t *cap_tamper)
 * @brief Set tamper_sensitive_b and tamper_threshold and tamper_sensitive_h to capture
 * @param cap_vch The vch of capture
 * @param cap_tamper Set parameter for capture tamper detection
 * @return 0 on success, -1 on error 
 */
int gm_set_cap_tamper(int cap_vch, gm_cap_tamper_t *cap_tamper);

#endif 
