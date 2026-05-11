/**
 * @file    bsp_oled_handler.h
 * @author  RUST HUEY
 * @brief   OLED handler header.
 * @version 1.0.0
 * @date    2026-05-09
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __BSP_OLED_HANDLER_H__
#define __BSP_OLED_HANDLER_H__

#include "bsp_oled_driver.h"
#include "bsp_oled_text.h"

typedef struct bsp_oled_handler bsp_oled_handler_t;

/**
 * @brief OLED handler status.
 */
typedef enum
{
    OLED_HANDLER_STATUS_OK             = 0U,
    OLED_HANDLER_STATUS_ERROR          = 1U,
    OLED_HANDLER_STATUS_ERROR_TIMEOUT  = 2U,
    OLED_HANDLER_STATUS_ERROR_SOURCE   = 3U,
    OLED_HANDLER_STATUS_ERROR_PARAM    = 4U,
    OLED_HANDLER_STATUS_ERROR_MEMORY   = 5U,
    OLED_HANDLER_STATUS_ERROR_ISR      = 6U,
    OLED_HANDLER_STATUS_ERROR_RESERVED = 0x7fffffffU,
} oled_handler_status_t;

/**
 * @brief OLED handler instance.
 */
typedef struct bsp_oled_handler
{
    uint8_t           is_inited;
    oled_interface_t  intf;
    bsp_oled_driver_t driver;

    oled_handler_status_t (*pf_init)(bsp_oled_handler_t *const self);

    oled_handler_status_t (*pf_deinit)(bsp_oled_handler_t *const self);

    oled_handler_status_t (*pf_clear)(bsp_oled_handler_t *const self);

    oled_handler_status_t (*pf_fill)(bsp_oled_handler_t *const self,
                                     oled_color_t               color);

    oled_handler_status_t (*pf_refresh)(bsp_oled_handler_t *const self);

    oled_handler_status_t (*pf_draw_pixel)(bsp_oled_handler_t *const self,
                                           uint8_t                    x,
                                           uint8_t                    y,
                                           oled_color_t               color);

    oled_handler_status_t (*pf_draw_text)(bsp_oled_handler_t *const self,
                                          uint8_t                    x,
                                          uint8_t                    y,
                                          const char                *text);

    oled_handler_status_t (*pf_draw_text_by_font)(
        bsp_oled_handler_t *const self,
        uint8_t                    x,
        uint8_t                    y,
        const char                *text,
        oled_text_font_t           font);

    oled_handler_status_t (*pf_get_driver)(bsp_oled_handler_t *const self,
                                           bsp_oled_driver_t **const p_driver);
} bsp_oled_handler_t;

/**
 * @brief Construct an OLED handler instance.
 *
 * @param[in] self   Handler instance.
 * @param[in] p_intf OLED communication interface.
 *
 * @return Status code.
 */
oled_handler_status_t bsp_oled_handler_inst(bsp_oled_handler_t *const self,
                                            oled_interface_t   *const p_intf);

#endif /* __BSP_OLED_HANDLER_H__ */
