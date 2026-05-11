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

#endif /* __BSP_OLED_TEXT_H__ */
