/**
 * @file    bsp_oled_text.c
 * @author  RUST HUEY
 * @brief   OLED UTF-8 text drawing source.
 * @version 1.0.0
 * @date    2026-05-09
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "bsp_oled_text.h"
#include "lvgl.h"
#include "lv_font_cn_14.h"
#include "lv_font_cn_16.h"

#include <stddef.h>

#define OLED_TEXT_GLYPH_ADV_SHIFT  (4U)

static uint32_t bsp_oled_text_utf8_decode(const char *text,
                                          uint8_t    *const step)
{
    const uint8_t *bytes = NULL;
    uint32_t       code   = 0U;

    if ((NULL == text) || (NULL == step))
    {
        return 0U;
    }

    bytes = (const uint8_t *)text;
    if (0x80U > bytes[0])
    {
        *step = 1U;
        return bytes[0];
    }

    if (0xC0U == (bytes[0] & 0xE0U))
    {
        *step = 2U;
        code  = ((uint32_t)(bytes[0] & 0x1FU) << 6U) |
                (uint32_t)(bytes[1] & 0x3FU);
        return code;
    }

    if (0xE0U == (bytes[0] & 0xF0U))
    {
        *step = 3U;
        code  = ((uint32_t)(bytes[0] & 0x0FU) << 12U) |
                ((uint32_t)(bytes[1] & 0x3FU) << 6U) |
                (uint32_t)(bytes[2] & 0x3FU);
        return code;
    }

    if (0xF0U == (bytes[0] & 0xF8U))
    {
        *step = 4U;
        code  = ((uint32_t)(bytes[0] & 0x07U) << 18U) |
                ((uint32_t)(bytes[1] & 0x3FU) << 12U) |
                ((uint32_t)(bytes[2] & 0x3FU) << 6U) |
                (uint32_t)(bytes[3] & 0x3FU);
        return code;
    }

    *step = 1U;
    return (uint32_t)'?';
}

static oled_driver_status_t bsp_oled_text_draw_glyph(
    bsp_oled_driver_t          *const self,
    int16_t                           x,
    int16_t                           y,
    const lv_font_t            *const font,
    const lv_font_glyph_dsc_t *const glyph_dsc,
    const uint8_t             *const bitmap)
{
    oled_driver_status_t status     = OLED_DRIVER_STATUS_OK;
    uint16_t             row        = 0U;
    uint16_t             col        = 0U;
    uint32_t             bit_index  = 0U;
    uint32_t             byte_index = 0U;
    uint8_t              bit_mask   = 0U;
    int16_t              pixel_x    = 0;
    int16_t              pixel_y    = 0;
    int16_t              glyph_x    = 0;
    int16_t              glyph_y    = 0;

    if ((NULL == self) || (NULL == font) ||
        (NULL == glyph_dsc) || (NULL == bitmap))
    {
        return OLED_DRIVER_STATUS_ERROR_PARAM;
    }

    glyph_x = (int16_t)(x + glyph_dsc->ofs_x);
    glyph_y = (int16_t)(y + font->line_height -
              font->base_line - glyph_dsc->box_h -
              glyph_dsc->ofs_y);

    for (row = 0U; glyph_dsc->box_h > row; row++)
    {
        for (col = 0U; glyph_dsc->box_w > col; col++)
        {
            bit_index  = (uint32_t)row * glyph_dsc->box_w + col;
            byte_index = bit_index / 8U;
            bit_mask   = (uint8_t)(0x80U >> (bit_index % 8U));
            if (0U == (bitmap[byte_index] & bit_mask))
            {
                continue;
            }

            pixel_x = (int16_t)(glyph_x + col);
            pixel_y = (int16_t)(glyph_y + row);
            if ((0 > pixel_x) || (0 > pixel_y))
            {
                continue;
            }

            if ((OLED_DRIVER_WIDTH <= pixel_x) ||
                (OLED_DRIVER_HEIGHT <= pixel_y))
            {
                continue;
            }

            status = self->pf_draw_pixel(self,
                                         (uint8_t)pixel_x,
                                         (uint8_t)pixel_y,
                                         OLED_COLOR_ON);
            if (OLED_DRIVER_STATUS_OK != status)
            {
                return status;
            }
        }
    }

    return OLED_DRIVER_STATUS_OK;
}

oled_driver_status_t bsp_oled_text_draw_utf8(
    bsp_oled_driver_t *const self,
    uint8_t                    x,
    uint8_t                    y,
    const char                *text)
{
    oled_driver_status_t status    = OLED_DRIVER_STATUS_OK;
    const lv_font_t     *font      = &LV_FONT_CN_CURRENT;
    lv_font_glyph_dsc_t  glyph_dsc;
    const uint8_t       *bitmap    = NULL;
    const char          *cursor    = NULL;
    uint32_t             unicode   = 0U;
    uint16_t             pen_x     = 0U;
    uint16_t             pen_y     = 0U;
    uint16_t             advance_x = 0U;
    uint8_t              step      = 0U;
    bool                 found     = false;

    if ((NULL == self) || (NULL == text))
    {
        return OLED_DRIVER_STATUS_ERROR_PARAM;
    }

    cursor = text;
    pen_x  = x;
    pen_y  = y;

    while ('\0' != *cursor)
    {
        if ('\n' == *cursor)
        {
            pen_x  = x;
            pen_y  = (uint16_t)(pen_y + font->line_height);
            cursor = cursor + 1;
            continue;
        }

        unicode = bsp_oled_text_utf8_decode(cursor, &step);
        if (0U == step)
        {
            cursor = cursor + 1;
            continue;
        }

        found = lv_font_get_glyph_dsc_fmt_txt(font,
                                              &glyph_dsc,
                                              unicode,
                                              0U);
        if (false == found)
        {
            unicode = (uint32_t)'?';
            found = lv_font_get_glyph_dsc_fmt_txt(font,
                                                  &glyph_dsc,
                                                  unicode,
                                                  0U);
        }

        if (false == found)
        {
            cursor = cursor + step;
            continue;
        }

        bitmap = lv_font_get_bitmap_fmt_txt(font, unicode);
        if (NULL == bitmap)
        {
            cursor = cursor + step;
            continue;
        }

        status = bsp_oled_text_draw_glyph(self,
                                          (int16_t)pen_x,
                                          (int16_t)pen_y,
                                          font,
                                          &glyph_dsc,
                                          bitmap);
        if (OLED_DRIVER_STATUS_OK != status)
        {
            return status;
        }

        advance_x = (uint16_t)((glyph_dsc.adv_w +
                    (1U << (OLED_TEXT_GLYPH_ADV_SHIFT - 1U))) >>
                    OLED_TEXT_GLYPH_ADV_SHIFT);
        pen_x     = (uint16_t)(pen_x + advance_x);
        cursor    = cursor + step;
    }

    return OLED_DRIVER_STATUS_OK;
}
