/**
 * @file    bsp_display_wrapper.c
 * @author  RUST HUEY
 * @brief   Display wrapper source.
 * @version 1.0.0
 * @date    2026-05-08
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "bsp_display_wrapper.h"
#include "elog.h"

#include <stddef.h>

static bsp_display_wrapper_dev_t g_display_wrapper_dev;

static uint32_t g_display_wrapper_current_idx = 0U;

static display_status_t bsp_display_wrapper_check_idx(uint32_t index)
{
    if (BSP_DISPLAY_WRAPPER_DEV_NUM_MAX <= index)
    {
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_PARAM;
    }

    return BSP_DISPLAY_WRAPPER_STATUS_OK;
}

static display_status_t
bsp_display_wrapper_get_current_dev(bsp_display_wrapper_dev_t **const dev)
{
    display_status_t status = BSP_DISPLAY_WRAPPER_STATUS_OK;

    if (NULL == dev)
    {
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_PARAM;
    }

    status = bsp_display_wrapper_check_idx(g_display_wrapper_current_idx);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        return status;
    }

    *dev = &g_display_wrapper_dev;
    return BSP_DISPLAY_WRAPPER_STATUS_OK;
}

display_status_t bsp_display_wrapper_reg(void)
{
    g_display_wrapper_dev             = (bsp_display_wrapper_dev_t){0};
    g_display_wrapper_dev.idx         = 0U;
    g_display_wrapper_current_idx     = 0U;

    return BSP_DISPLAY_WRAPPER_STATUS_OK;
}

bsp_display_wrapper_dev_t *bsp_display_wrapper_get_dev(void)
{
    return &g_display_wrapper_dev;
}

display_status_t bsp_display_wrapper_set_idx(uint32_t index)
{
    display_status_t status = BSP_DISPLAY_WRAPPER_STATUS_OK;

    status = bsp_display_wrapper_check_idx(index);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_wrapper_set_idx: index error");
        return status;
    }

    g_display_wrapper_current_idx = index;
    return BSP_DISPLAY_WRAPPER_STATUS_OK;
}

display_status_t bsp_display_wrapper_init(void)
{
    display_status_t          status = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_display_wrapper_dev_t *dev   = NULL;

    status = bsp_display_wrapper_get_current_dev(&dev);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_wrapper_init: current dev error");
        return status;
    }

    if (NULL == dev->pf_init)
    {
        log_e("bsp_display_wrapper_init: current display not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    return dev->pf_init(dev);
}

display_status_t bsp_display_wrapper_deinit(void)
{
    display_status_t          status = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_display_wrapper_dev_t *dev   = NULL;

    status = bsp_display_wrapper_get_current_dev(&dev);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_wrapper_deinit: current dev error");
        return status;
    }

    if (NULL == dev->pf_deinit)
    {
        log_e("bsp_display_wrapper_deinit: current display not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    return dev->pf_deinit(dev);
}

display_status_t bsp_display_wrapper_clear(void)
{
    display_status_t          status = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_display_wrapper_dev_t *dev   = NULL;

    status = bsp_display_wrapper_get_current_dev(&dev);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_wrapper_clear: current dev error");
        return status;
    }

    if (NULL == dev->pf_clear)
    {
        log_e("bsp_display_wrapper_clear: current display not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    return dev->pf_clear(dev);
}

display_status_t bsp_display_wrapper_fill(uint16_t color)
{
    display_status_t          status = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_display_wrapper_dev_t *dev   = NULL;

    status = bsp_display_wrapper_get_current_dev(&dev);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_wrapper_fill: current dev error");
        return status;
    }

    if (NULL == dev->pf_fill)
    {
        log_e("bsp_display_wrapper_fill: current display not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    return dev->pf_fill(dev, color);
}

display_status_t bsp_display_wrapper_refresh(void)
{
    display_status_t          status = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_display_wrapper_dev_t *dev   = NULL;

    status = bsp_display_wrapper_get_current_dev(&dev);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_wrapper_refresh: current dev error");
        return status;
    }

    if (NULL == dev->pf_refresh)
    {
        log_e("bsp_display_wrapper_refresh: current display not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    return dev->pf_refresh(dev);
}

display_status_t bsp_display_wrapper_draw_pixel(uint16_t x,
                                                uint16_t y,
                                                uint16_t color)
{
    display_status_t          status = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_display_wrapper_dev_t *dev   = NULL;

    status = bsp_display_wrapper_get_current_dev(&dev);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_wrapper_draw_pixel: current dev error");
        return status;
    }

    if (NULL == dev->pf_draw_pixel)
    {
        log_e("bsp_display_wrapper_draw_pixel: current display not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    return dev->pf_draw_pixel(dev, x, y, color);
}

display_status_t bsp_display_wrapper_draw_text(uint16_t    x,
                                               uint16_t    y,
                                               const char *text)
{
    display_status_t          status = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_display_wrapper_dev_t *dev   = NULL;

    if (NULL == text)
    {
        log_e("bsp_display_wrapper_draw_text: text error");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_PARAM;
    }

    status = bsp_display_wrapper_get_current_dev(&dev);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_wrapper_draw_text: current dev error");
        return status;
    }

    if (NULL == dev->pf_draw_text)
    {
        log_e("bsp_display_wrapper_draw_text: current display not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    return dev->pf_draw_text(dev, x, y, text);
}

display_status_t bsp_display_wrapper_get_buffer(uint8_t  **const buffer,
                                                uint16_t  *const size)
{
    display_status_t          status = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_display_wrapper_dev_t *dev   = NULL;

    if ((NULL == buffer) || (NULL == size))
    {
        log_e("bsp_display_wrapper_get_buffer: output param error");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_PARAM;
    }

    status = bsp_display_wrapper_get_current_dev(&dev);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_wrapper_get_buffer: current dev error");
        return status;
    }

    if (NULL == dev->pf_get_buffer)
    {
        log_e("bsp_display_wrapper_get_buffer: current display not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    return dev->pf_get_buffer(dev, buffer, size);
}

display_status_t bsp_display_wrapper_set_backlight(uint16_t resolution)
{
    display_status_t          status = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_display_wrapper_dev_t *dev   = NULL;

    status = bsp_display_wrapper_get_current_dev(&dev);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_wrapper_set_backlight: current dev error");
        return status;
    }

    if (NULL == dev->pf_set_backlight)
    {
        log_w("bsp_display_wrapper_set_backlight: current not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    return dev->pf_set_backlight(dev, resolution);
}

display_status_t bsp_display_wrapper_draw_image(uint16_t          x,
                                                uint16_t          y,
                                                uint16_t          w,
                                                uint16_t          h,
                                                const uint16_t   *data)
{
    display_status_t          status = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_display_wrapper_dev_t *dev   = NULL;

    if (NULL == data)
    {
        log_e("bsp_display_wrapper_draw_image: data error");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_PARAM;
    }

    status = bsp_display_wrapper_get_current_dev(&dev);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_wrapper_draw_image: current dev error");
        return status;
    }

    if (NULL == dev->pf_draw_image)
    {
        log_w("bsp_display_wrapper_draw_image: current not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    return dev->pf_draw_image(dev, x, y, w, h, data);
}
