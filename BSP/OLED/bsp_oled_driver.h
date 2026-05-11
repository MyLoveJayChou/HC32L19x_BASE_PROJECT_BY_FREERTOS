/**
 * @file    bsp_oled_driver.h
 * @author  RUST HUEY
 * @brief   SH1106 OLED driver header.
 * @version 1.0.0
 * @date    2026-05-08
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __BSP_OLED_DRIVER_H__
#define __BSP_OLED_DRIVER_H__

#include <stdint.h>

#define OLED_DRIVER_WIDTH       (128U)
#define OLED_DRIVER_HEIGHT      (64U)
#define OLED_DRIVER_PAGE_COUNT  (8U)
#define OLED_DRIVER_BUFFER_SIZE (1024U)
#define OLED_DRIVER_COL_OFFSET  (2U)

typedef struct bsp_oled_driver bsp_oled_driver_t;

/**
 * @brief OLED driver status.
 */
typedef enum
{
    OLED_DRIVER_STATUS_OK             = 0U,
    OLED_DRIVER_STATUS_ERROR          = 1U,
    OLED_DRIVER_STATUS_ERROR_TIMEOUT  = 2U,
    OLED_DRIVER_STATUS_ERROR_SOURCE   = 3U,
    OLED_DRIVER_STATUS_ERROR_PARAM    = 4U,
    OLED_DRIVER_STATUS_ERROR_MEMORY   = 5U,
    OLED_DRIVER_STATUS_ERROR_ISR      = 6U,
    OLED_DRIVER_STATUS_ERROR_RESERVED = 0x7fffffffU,
} oled_driver_status_t;

/**
 * @brief OLED pixel color.
 */
typedef enum
{
    OLED_COLOR_OFF = 0U,
    OLED_COLOR_ON  = 1U,
} oled_color_t;

/**
 * @brief OLED communication interface.
 */
typedef struct
{
    oled_driver_status_t (*pf_write_cmd)(const uint8_t *data,
                                          uint16_t      size);
    oled_driver_status_t (*pf_write_data)(const uint8_t *data,
                                          uint16_t      size);
    oled_driver_status_t (*pf_reset)(uint8_t level);
    oled_driver_status_t (*pf_delay_ms)(uint32_t delay_ms);
} oled_interface_t;

/**
 * @brief OLED driver instance.
 */
typedef struct bsp_oled_driver
{
    uint8_t             is_inited;
    oled_interface_t   *p_intf;
    uint8_t             buffer[OLED_DRIVER_BUFFER_SIZE];

    oled_driver_status_t (*pf_init)(bsp_oled_driver_t *const self);
    oled_driver_status_t (*pf_deinit)(bsp_oled_driver_t *const self);
    oled_driver_status_t (*pf_clear)(bsp_oled_driver_t *const self);
    oled_driver_status_t (*pf_fill)(bsp_oled_driver_t *const self,
                                    oled_color_t color);
    oled_driver_status_t (*pf_refresh)(bsp_oled_driver_t *const self);
    oled_driver_status_t (*pf_draw_pixel)(bsp_oled_driver_t *const self,
                                          uint8_t x,
                                          uint8_t y,
                                          oled_color_t color);
    oled_driver_status_t (*pf_get_buffer)(bsp_oled_driver_t *const self,
                                          uint8_t **const buffer,
                                          uint16_t *const size);
} bsp_oled_driver_t;

/**
 * @brief Construct an OLED driver instance.
 *
 * @param[in] self   Driver instance.
 * @param[in] p_intf Hardware interface.
 *
 * @return Status code.
 */
oled_driver_status_t bsp_oled_driver_inst(bsp_oled_driver_t *const self,
                                          oled_interface_t *const p_intf);

#endif /* __BSP_OLED_DRIVER_H__ */
