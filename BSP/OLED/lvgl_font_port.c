/**
 * @file    lvgl_font_port.c
 * @author  RUST HUEY
 * @brief   Minimal LVGL font port source.
 * @version 1.0.0
 * @date    2026-05-09
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "lvgl.h"

#include <stddef.h>

static uint16_t lvgl_font_port_find_glyph_id(
    const lv_font_fmt_txt_dsc_t *const font_dsc,
    uint32_t                     unicode_letter)
{
    const lv_font_fmt_txt_cmap_t *cmap     = NULL;
    uint16_t                      cmap_idx = 0U;
    uint16_t                      list_idx = 0U;
    uint32_t                      letter   = 0U;
    uint16_t                      glyph_id = 0U;

    if (NULL == font_dsc)
    {
        return 0U;
    }

    for (cmap_idx = 0U; font_dsc->cmap_num > cmap_idx; cmap_idx++)
    {
        cmap = &font_dsc->cmaps[cmap_idx];

        if ((cmap->range_start > unicode_letter) ||
            (unicode_letter >= cmap->range_start + cmap->range_length))
        {
            continue;
        }

        if (LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY == cmap->type)
        {
            glyph_id = (uint16_t)(cmap->glyph_id_start +
                       (unicode_letter - cmap->range_start));
            return glyph_id;
        }

        if (LV_FONT_FMT_TXT_CMAP_SPARSE_TINY == cmap->type)
        {
            for (list_idx = 0U; cmap->list_length > list_idx; list_idx++)
            {
                letter = cmap->range_start + cmap->unicode_list[list_idx];
                if (letter == unicode_letter)
                {
                    glyph_id = (uint16_t)(cmap->glyph_id_start + list_idx);
                    if (NULL != cmap->glyph_id_ofs_list)
                    {
                        glyph_id = (uint16_t)(glyph_id +
                                   cmap->glyph_id_ofs_list[list_idx]);
                    }

                    return glyph_id;
                }
            }
        }
    }

    return 0U;
}

const uint8_t *lv_font_get_bitmap_fmt_txt(const lv_font_t *font,
                                          uint32_t         unicode_letter)
{
    const lv_font_fmt_txt_dsc_t *font_dsc = NULL;
    uint16_t                     glyph_id = 0U;

    if ((NULL == font) || (NULL == font->dsc))
    {
        return NULL;
    }

    font_dsc = (const lv_font_fmt_txt_dsc_t *)font->dsc;
    glyph_id = lvgl_font_port_find_glyph_id(font_dsc, unicode_letter);
    if (0U == glyph_id)
    {
        if (NULL != font->fallback)
        {
            return lv_font_get_bitmap_fmt_txt(font->fallback,
                                              unicode_letter);
        }

        return NULL;
    }

    return &font_dsc->glyph_bitmap[font_dsc->glyph_dsc[glyph_id].
                                   bitmap_index];
}

bool lv_font_get_glyph_dsc_fmt_txt(const lv_font_t     *font,
                                   lv_font_glyph_dsc_t *dsc_out,
                                   uint32_t             unicode_letter,
                                   uint32_t             unicode_letter_next)
{
    const lv_font_fmt_txt_dsc_t       *font_dsc  = NULL;
    const lv_font_fmt_txt_glyph_dsc_t *glyph_dsc = NULL;
    uint16_t                           glyph_id   = 0U;

    (void)unicode_letter_next;

    if ((NULL == font) || (NULL == dsc_out) || (NULL == font->dsc))
    {
        return false;
    }

    font_dsc = (const lv_font_fmt_txt_dsc_t *)font->dsc;
    glyph_id = lvgl_font_port_find_glyph_id(font_dsc, unicode_letter);
    if (0U == glyph_id)
    {
        if (NULL != font->fallback)
        {
            return lv_font_get_glyph_dsc_fmt_txt(font->fallback,
                                                 dsc_out,
                                                 unicode_letter,
                                                 unicode_letter_next);
        }

        return false;
    }

    glyph_dsc               = &font_dsc->glyph_dsc[glyph_id];
    dsc_out->adv_w          = glyph_dsc->adv_w;
    dsc_out->box_w          = glyph_dsc->box_w;
    dsc_out->box_h          = glyph_dsc->box_h;
    dsc_out->ofs_x          = glyph_dsc->ofs_x;
    dsc_out->ofs_y          = glyph_dsc->ofs_y;
    dsc_out->bpp            = font_dsc->bpp;
    dsc_out->is_placeholder = 0U;

    return true;
}
