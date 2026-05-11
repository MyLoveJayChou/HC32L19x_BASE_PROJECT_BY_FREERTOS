/**
 * @file    bsp_display_wrapper.h
 * @author  RUST HUEY
 * @brief   Display wrapper header.
 * @version 1.0.0
 * @date    2026-05-08
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __BSP_DISPLAY_WRAPPER_H__
#define __BSP_DISPLAY_WRAPPER_H__

#include <stdint.h>

#define BSP_DISPLAY_WRAPPER_DEV_NUM_MAX  (1U)

typedef struct bsp_display_wrapper_dev bsp_display_wrapper_dev_t;

/**
 * @brief Display wrapper status.
 */
typedef enum
{
    BSP_DISPLAY_WRAPPER_STATUS_OK             = 0U,
    BSP_DISPLAY_WRAPPER_STATUS_ERROR          = 1U,
    BSP_DISPLAY_WRAPPER_STATUS_ERROR_TIMEOUT  = 2U,
    BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE   = 3U,
    BSP_DISPLAY_WRAPPER_STATUS_ERROR_PARAM    = 4U,
    BSP_DISPLAY_WRAPPER_STATUS_ERROR_MEMORY   = 5U,
    BSP_DISPLAY_WRAPPER_STATUS_ERROR_ISR      = 6U,
    BSP_DISPLAY_WRAPPER_STATUS_ERROR_RESERVED = 0x7fffffffU,
} display_status_t;

/**
 * @brief Display device registration object.
 */
struct bsp_display_wrapper_dev
{
    uint32_t idx;
    void    *user_data;

    display_status_t (*pf_init)(bsp_display_wrapper_dev_t *const dev);
    display_status_t (*pf_deinit)(bsp_display_wrapper_dev_t *const dev);
    display_status_t (*pf_clear)(bsp_display_wrapper_dev_t *const dev);
    display_status_t (*pf_fill)(bsp_display_wrapper_dev_t *const dev,
                                uint16_t                   color);
    display_status_t (*pf_refresh)(bsp_display_wrapper_dev_t *const dev);
    display_status_t (*pf_draw_pixel)(bsp_display_wrapper_dev_t *const dev,
                                      uint16_t                   x,
                                      uint16_t                   y,
                                      uint16_t                   color);
    display_status_t (*pf_draw_text)(bsp_display_wrapper_dev_t *const dev,
                                     uint16_t                   x,
                                     uint16_t                   y,
                                     const char                *text);
    display_status_t (*pf_get_buffer)(bsp_display_wrapper_dev_t *const dev,
                                      uint8_t                **const buffer,
                                      uint16_t                *const size);
    display_status_t (*pf_set_backlight)(bsp_display_wrapper_dev_t *const dev,
                                         uint16_t                   resolution);
    display_status_t (*pf_draw_image)(bsp_display_wrapper_dev_t *const dev,
                                      uint16_t                   x,
                                      uint16_t                   y,
                                      uint16_t                   w,
                                      uint16_t                   h,
                                      const uint16_t            *data);
};

/**
 * @brief Register a display device.
 *
 * @return Status code.
 */
display_status_t bsp_display_wrapper_reg(void);

/**
 * @brief Get the internal display device object.
 *
 * @return Internal display device pointer.
 */
bsp_display_wrapper_dev_t *bsp_display_wrapper_get_dev(void);

/**
 * @brief Set the current device index.
 *
 * @param[in] index Device index.
 *
 * @return Status code.
 */
display_status_t bsp_display_wrapper_set_idx(uint32_t index);

/**
 * @brief Initialize the current device.
 *
 * @return Status code.
 */
display_status_t bsp_display_wrapper_init(void);

/**
 * @brief Deinitialize the current device.
 *
 * @return Status code.
 */
display_status_t bsp_display_wrapper_deinit(void);

/**
 * @brief Clear the current device.
 *
 * @return Status code.
 */
display_status_t bsp_display_wrapper_clear(void);

/**
 * @brief Fill the current device with a color.
 *
 * @param[in] color Fill color.
 *
 * @return Status code.
 */
display_status_t bsp_display_wrapper_fill(uint16_t color);

/**
 * @brief Refresh the current device.
 *
 * @return Status code.
 */
display_status_t bsp_display_wrapper_refresh(void);

/**
 * @brief Draw one pixel on the current device.
 *
 * @param[in] x     X coordinate.
 * @param[in] y     Y coordinate.
 * @param[in] color Pixel color.
 *
 * @return Status code.
 */
display_status_t bsp_display_wrapper_draw_pixel(uint16_t x,
                                                uint16_t y,
                                                uint16_t color);

/**
 * @brief Draw one text string on the current device.
 *
 * @param[in] x    X coordinate.
 * @param[in] y    Y coordinate.
 * @param[in] text Text string.
 *
 * @return Status code.
 */
display_status_t bsp_display_wrapper_draw_text(uint16_t    x,
                                               uint16_t    y,
                                               const char *text);

/**
 * @brief Get the current frame buffer.
 *
 * @param[out] buffer Frame buffer pointer.
 * @param[out] size   Frame buffer size.
 *
 * @return Status code.
 */
display_status_t bsp_display_wrapper_get_buffer(uint8_t  **const buffer,
                                                uint16_t  *const size);

/**
 * @brief Set backlight for the current device.
 *
 * @param[in] resolution Backlight resolution.
 *
 * @return Status code.
 */
display_status_t bsp_display_wrapper_set_backlight(uint16_t resolution);

/**
 * @brief Draw an image on the current device.
 *
 * @param[in] x    X coordinate.
 * @param[in] y    Y coordinate.
 * @param[in] w    Image width.
 * @param[in] h    Image height.
 * @param[in] data Image data.
 *
 * @return Status code.
 */
display_status_t bsp_display_wrapper_draw_image(uint16_t          x,
                                                uint16_t          y,
                                                uint16_t          w,
                                                uint16_t          h,
                                                const uint16_t   *data);

#endif /* __BSP_DISPLAY_WRAPPER_H__ */
