/**
 * @file    lvgl.h
 * @author  RUST HUEY
 * @brief   Minimal LVGL font compatibility header.
 * @version 1.0.0
 * @date    2026-05-09
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __MINI_LVGL_H__
#define __MINI_LVGL_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define LVGL_VERSION_MAJOR     (8U)
#define LVGL_VERSION_MINOR     (3U)
#define LVGL_VERSION_PATCH     (0U)
#define LV_ATTRIBUTE_LARGE_CONST
#define LV_FONT_SUBPX_NONE     (0U)

#ifndef LVGL_USE_FONT_CN_14
#define LVGL_USE_FONT_CN_14    (1U)
#endif

#define LV_FONT_CN_14          (1U)
#define LV_FONT_CN_16          (1U)

#if (0U != LVGL_USE_FONT_CN_14)
#define LV_FONT_CN_CURRENT     (lv_font_cn_14)
#else
#define LV_FONT_CN_CURRENT     (lv_font_cn_16)
#endif

#define LV_VERSION_CHECK(major, minor, patch)                         \
    (((LVGL_VERSION_MAJOR) > (major)) ||                             \
     (((LVGL_VERSION_MAJOR) == (major)) &&                           \
      (((LVGL_VERSION_MINOR) > (minor)) ||                           \
       (((LVGL_VERSION_MINOR) == (minor)) &&                         \
        ((LVGL_VERSION_PATCH) >= (patch))))))

#define LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY  (0U)
#define LV_FONT_FMT_TXT_CMAP_SPARSE_TINY   (2U)

typedef struct _lv_font_struct lv_font_t;

extern const lv_font_t lv_font_cn_14;
extern const lv_font_t lv_font_cn_16;

/**
 * @brief Public glyph descriptor.
 */
typedef struct
{
    uint16_t adv_w;
    uint8_t  box_w;
    uint8_t  box_h;
    int8_t   ofs_x;
    int8_t   ofs_y;
    uint8_t  bpp;
    uint8_t  is_placeholder;
} lv_font_glyph_dsc_t;

/**
 * @brief Internal glyph descriptor.
 */
typedef struct
{
    uint32_t bitmap_index;
    uint16_t adv_w;
    uint8_t  box_w;
    uint8_t  box_h;
    int8_t   ofs_x;
    int8_t   ofs_y;
} lv_font_fmt_txt_glyph_dsc_t;

/**
 * @brief Internal character map descriptor.
 */
typedef struct
{
    uint32_t        range_start;
    uint16_t        range_length;
    uint16_t        glyph_id_start;
    const uint16_t *unicode_list;
    const uint16_t *glyph_id_ofs_list;
    uint16_t        list_length;
    uint8_t         type;
} lv_font_fmt_txt_cmap_t;

/**
 * @brief Internal kerning class descriptor.
 */
typedef struct
{
    const int8_t  *class_pair_values;
    const uint8_t *left_class_mapping;
    const uint8_t *right_class_mapping;
    uint8_t        left_class_cnt;
    uint8_t        right_class_cnt;
} lv_font_fmt_txt_kern_classes_t;

/**
 * @brief Internal cache descriptor.
 */
typedef struct
{
    uint32_t last_letter;
    uint16_t last_glyph_id;
} lv_font_fmt_txt_glyph_cache_t;

/**
 * @brief Internal font descriptor.
 */
typedef struct
{
    const uint8_t                         *glyph_bitmap;
    const lv_font_fmt_txt_glyph_dsc_t    *glyph_dsc;
    const lv_font_fmt_txt_cmap_t         *cmaps;
    const lv_font_fmt_txt_kern_classes_t *kern_dsc;
    uint16_t                              kern_scale;
    uint16_t                              cmap_num;
    uint8_t                               bpp;
    uint8_t                               kern_classes;
    uint8_t                               bitmap_format;
    lv_font_fmt_txt_glyph_cache_t        *cache;
} lv_font_fmt_txt_dsc_t;

/**
 * @brief Minimal public font descriptor.
 */
struct _lv_font_struct
{
    bool (*get_glyph_dsc)(const lv_font_t          *font,
                          lv_font_glyph_dsc_t      *dsc_out,
                          uint32_t                  unicode_letter,
                          uint32_t                  unicode_letter_next);
    const uint8_t *(*get_glyph_bitmap)(const lv_font_t *font,
                                       uint32_t         unicode_letter);
    uint8_t          line_height;
    int8_t           base_line;
    uint8_t          subpx;
    int8_t           underline_position;
    int8_t           underline_thickness;
    uint8_t          static_bitmap;
    const void      *dsc;
    const lv_font_t *fallback;
    void            *user_data;
};

/**
 * @brief Get glyph bitmap by Unicode code point.
 *
 * @param[in] font           Font object.
 * @param[in] unicode_letter Unicode code point.
 *
 * @return Glyph bitmap pointer.
 */
const uint8_t *lv_font_get_bitmap_fmt_txt(const lv_font_t *font,
                                          uint32_t         unicode_letter);

/**
 * @brief Get glyph description by Unicode code point.
 *
 * @param[in]  font                Font object.
 * @param[out] dsc_out             Glyph descriptor output.
 * @param[in]  unicode_letter      Unicode code point.
 * @param[in]  unicode_letter_next Next Unicode code point.
 *
 * @return `true` when glyph exists.
 */
bool lv_font_get_glyph_dsc_fmt_txt(const lv_font_t     *font,
                                   lv_font_glyph_dsc_t *dsc_out,
                                   uint32_t             unicode_letter,
                                   uint32_t             unicode_letter_next);

#endif /* __MINI_LVGL_H__ */
