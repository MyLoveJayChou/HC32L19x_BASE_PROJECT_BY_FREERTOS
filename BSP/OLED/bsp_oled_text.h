/**
 * @file    bsp_oled_text.h
 * @author  RUST HUEY
 * @brief   OLED UTF-8 text drawing header.
 * @version 1.0.0
 * @date    2026-05-09
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __BSP_OLED_TEXT_H__
#define __BSP_OLED_TEXT_H__

#include "bsp_oled_driver.h"

/**
 * @brief OLED text font size.
 */
typedef enum
{
    OLED_TEXT_FONT_14 = 14U,
    OLED_TEXT_FONT_16 = 16U,
} oled_text_font_t;

/**
 * @brief Draw one UTF-8 string by the built-in Chinese font.
 *
 * @param[in] self OLED driver instance.
 * @param[in] x    Line start X.
 * @param[in] y    Line start Y.
 * @param[in] text UTF-8 string pointer.
 *
 * @return Status code.
 *
 * @note Current font contains ASCII and custom Chinese glyphs.
 * @note Define `LVGL_USE_FONT_CN_14` in `lvgl.h` to switch between
 *       14px and 16px fonts.
 */
oled_driver_status_t bsp_oled_text_draw_utf8(
    bsp_oled_driver_t *const self,
    uint8_t                    x,
    uint8_t                    y,
    const char                *text);

/**
 * @brief Draw one UTF-8 string by the specified Chinese font.
 *
 * @param[in] self OLED driver instance.
 * @param[in] x    Line start X.
 * @param[in] y    Line start Y.
 * @param[in] text UTF-8 string pointer.
 * @param[in] font Font size selector.
 *
 * @return Status code.
 */
oled_driver_status_t bsp_oled_text_draw_utf8_by_font(
    bsp_oled_driver_t *const self,
    uint8_t                    x,
    uint8_t                    y,
    const char                *text,
    oled_text_font_t           font);

#endif /* __BSP_OLED_TEXT_H__ */
