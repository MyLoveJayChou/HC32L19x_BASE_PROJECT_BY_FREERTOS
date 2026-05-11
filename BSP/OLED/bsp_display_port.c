/**
 * @file    bsp_display_port.c
 * @author  RUST HUEY
 * @brief   Display port source.
 * @version 1.0.0
 * @date    2026-05-09
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "bsp_display_port.h"
#include "FreeRTOS.h"
#include "board.h"
#include "bsp_oled_handler.h"
#include "elog.h"
#include "gpio.h"
#include "spi.h"
#include "system_hc32l19x.h"
#include "task.h"

#include <stddef.h>

static oled_interface_t   g_display_port_oled_intf;
static bsp_oled_handler_t g_display_port_oled_handler;
static uint8_t            g_display_port_is_ready = 0U;

static display_status_t bsp_display_port_status_from_handler(
    oled_handler_status_t status)
{
    return (display_status_t)status;
}

static void bsp_display_port_delay_loop(uint32_t count)
{
    volatile uint32_t index = 0U;

    for (index = 0U; count > index; index++)
    {
    }
}

static void bsp_display_port_delay_ms_busy(uint32_t delay_ms)
{
    uint32_t index = 0U;

    for (index = 0U; delay_ms > index; index++)
    {
        bsp_display_port_delay_loop(SystemCoreClock / 12000U);
    }
}

static oled_driver_status_t bsp_display_port_delay_ms(uint32_t delay_ms)
{
    if (0U == delay_ms)
    {
        return OLED_DRIVER_STATUS_OK;
    }

    if (taskSCHEDULER_RUNNING == xTaskGetSchedulerState())
    {
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
    else
    {
        bsp_display_port_delay_ms_busy(delay_ms);
    }

    return OLED_DRIVER_STATUS_OK;
}

static oled_driver_status_t bsp_display_port_spi_send(
    const uint8_t *data,
    uint16_t       size)
{
    uint16_t index = 0U;

    if ((NULL == data) || (0U == size))
    {
        return OLED_DRIVER_STATUS_ERROR_PARAM;
    }

    Spi_SetCS(M0P_SPI0, FALSE);
    Gpio_ClrIO(BOARD_OLED_CS_PORT, BOARD_OLED_CS_PIN);
    bsp_display_port_delay_loop(256U);

    for (index = 0U; size > index; index++)
    {
        (void)Spi_RWByte(M0P_SPI0, data[index]);
    }

    while (TRUE == Spi_GetStatus(M0P_SPI0, SpiBusy))
    {
    }

    bsp_display_port_delay_loop(256U);
    Gpio_SetIO(BOARD_OLED_CS_PORT, BOARD_OLED_CS_PIN);
    Spi_SetCS(M0P_SPI0, TRUE);

    return OLED_DRIVER_STATUS_OK;
}

static oled_driver_status_t bsp_display_port_write_cmd(
    const uint8_t *data,
    uint16_t       size)
{
    Gpio_ClrIO(BOARD_OLED_DC_PORT, BOARD_OLED_DC_PIN);
    return bsp_display_port_spi_send(data, size);
}

static oled_driver_status_t bsp_display_port_write_data(
    const uint8_t *data,
    uint16_t       size)
{
    Gpio_SetIO(BOARD_OLED_DC_PORT, BOARD_OLED_DC_PIN);
    return bsp_display_port_spi_send(data, size);
}

static oled_driver_status_t bsp_display_port_reset(uint8_t level)
{
    if (0U == level)
    {
        Gpio_ClrIO(BOARD_OLED_RES_PORT, BOARD_OLED_RES_PIN);
    }
    else
    {
        Gpio_SetIO(BOARD_OLED_RES_PORT, BOARD_OLED_RES_PIN);
    }

    return OLED_DRIVER_STATUS_OK;
}

static display_status_t bsp_display_port_get_handler(
    bsp_display_wrapper_dev_t *const dev,
    bsp_oled_handler_t       **const p_handler)
{
    if ((NULL == dev) || (NULL == p_handler))
    {
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_PARAM;
    }

    if (NULL == dev->user_data)
    {
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    *p_handler = (bsp_oled_handler_t *)dev->user_data;

    return BSP_DISPLAY_WRAPPER_STATUS_OK;
}

static display_status_t bsp_display_port_init_impl(
    bsp_display_wrapper_dev_t *const dev)
{
    display_status_t      status  = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_oled_handler_t   *handler = NULL;

    status = bsp_display_port_get_handler(dev, &handler);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_port_init_impl: handler error");
        return status;
    }

    if (0U != g_display_port_is_ready)
    {
        return BSP_DISPLAY_WRAPPER_STATUS_OK;
    }

    if (NULL == handler->pf_init)
    {
        log_e("bsp_display_port_init_impl: init not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    status = bsp_display_port_status_from_handler(handler->pf_init(handler));
    if (BSP_DISPLAY_WRAPPER_STATUS_OK == status)
    {
        g_display_port_is_ready = 1U;
    }

    return status;
}

static display_status_t bsp_display_port_deinit_impl(
    bsp_display_wrapper_dev_t *const dev)
{
    display_status_t      status  = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_oled_handler_t   *handler = NULL;

    status = bsp_display_port_get_handler(dev, &handler);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_port_deinit_impl: handler error");
        return status;
    }

    if (0U == g_display_port_is_ready)
    {
        return BSP_DISPLAY_WRAPPER_STATUS_OK;
    }

    if (NULL == handler->pf_deinit)
    {
        log_e("bsp_display_port_deinit_impl: deinit not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    status = bsp_display_port_status_from_handler(handler->pf_deinit(handler));
    if (BSP_DISPLAY_WRAPPER_STATUS_OK == status)
    {
        g_display_port_is_ready = 0U;
    }

    return status;
}

static display_status_t bsp_display_port_clear_impl(
    bsp_display_wrapper_dev_t *const dev)
{
    display_status_t      status  = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_oled_handler_t   *handler = NULL;

    status = bsp_display_port_get_handler(dev, &handler);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_port_clear_impl: handler error");
        return status;
    }

    if (NULL == handler->pf_clear)
    {
        log_e("bsp_display_port_clear_impl: clear not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    return bsp_display_port_status_from_handler(handler->pf_clear(handler));
}

static display_status_t bsp_display_port_fill_impl(
    bsp_display_wrapper_dev_t *const dev,
    uint16_t                   color)
{
    display_status_t      status      = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_oled_handler_t   *handler     = NULL;
    oled_color_t          oled_color  = OLED_COLOR_OFF;

    status = bsp_display_port_get_handler(dev, &handler);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_port_fill_impl: handler error");
        return status;
    }

    if (NULL == handler->pf_fill)
    {
        log_e("bsp_display_port_fill_impl: fill not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    oled_color = (0U == color) ? OLED_COLOR_OFF : OLED_COLOR_ON;

    return bsp_display_port_status_from_handler(
        handler->pf_fill(handler, oled_color));
}

static display_status_t bsp_display_port_refresh_impl(
    bsp_display_wrapper_dev_t *const dev)
{
    display_status_t      status  = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_oled_handler_t   *handler = NULL;

    status = bsp_display_port_get_handler(dev, &handler);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_port_refresh_impl: handler error");
        return status;
    }

    if (NULL == handler->pf_refresh)
    {
        log_e("bsp_display_port_refresh_impl: refresh not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    return bsp_display_port_status_from_handler(handler->pf_refresh(handler));
}

static display_status_t bsp_display_port_draw_pixel_impl(
    bsp_display_wrapper_dev_t *const dev,
    uint16_t                   x,
    uint16_t                   y,
    uint16_t                   color)
{
    display_status_t      status      = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_oled_handler_t   *handler     = NULL;
    oled_color_t          oled_color  = OLED_COLOR_OFF;

    if ((0xFFU < x) || (0xFFU < y))
    {
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_PARAM;
    }

    status = bsp_display_port_get_handler(dev, &handler);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_port_draw_pixel_impl: handler error");
        return status;
    }

    if (NULL == handler->pf_draw_pixel)
    {
        log_e("bsp_display_port_draw_pixel_impl: draw pixel not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    oled_color = (0U == color) ? OLED_COLOR_OFF : OLED_COLOR_ON;

    return bsp_display_port_status_from_handler(
        handler->pf_draw_pixel(handler,
                               (uint8_t)x,
                               (uint8_t)y,
                               oled_color));
}

static display_status_t bsp_display_port_draw_text_impl(
    bsp_display_wrapper_dev_t *const dev,
    uint16_t                   x,
    uint16_t                   y,
    const char                *text)
{
    display_status_t      status  = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_oled_handler_t   *handler = NULL;

    if (NULL == text)
    {
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_PARAM;
    }

    if ((0xFFU < x) || (0xFFU < y))
    {
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_PARAM;
    }

    status = bsp_display_port_get_handler(dev, &handler);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_port_draw_text_impl: handler error");
        return status;
    }

    if (NULL == handler->pf_draw_text)
    {
        log_e("bsp_display_port_draw_text_impl: draw text not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    return bsp_display_port_status_from_handler(
        handler->pf_draw_text(handler,
                              (uint8_t)x,
                              (uint8_t)y,
                              text));
}

static display_status_t bsp_display_port_get_buffer_impl(
    bsp_display_wrapper_dev_t *const dev,
    uint8_t                **const buffer,
    uint16_t                *const size)
{
    display_status_t        status   = BSP_DISPLAY_WRAPPER_STATUS_OK;
    bsp_oled_handler_t     *handler  = NULL;
    bsp_oled_driver_t      *driver   = NULL;
    oled_handler_status_t   h_status = OLED_HANDLER_STATUS_OK;
    oled_driver_status_t    d_status = OLED_DRIVER_STATUS_OK;

    if ((NULL == buffer) || (NULL == size))
    {
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_PARAM;
    }

    status = bsp_display_port_get_handler(dev, &handler);
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_port_get_buffer_impl: handler error");
        return status;
    }

    if (NULL == handler->pf_get_driver)
    {
        log_e("bsp_display_port_get_buffer_impl: get driver not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    h_status = handler->pf_get_driver(handler, &driver);
    if (OLED_HANDLER_STATUS_OK != h_status)
    {
        return bsp_display_port_status_from_handler(h_status);
    }

    if ((NULL == driver) || (NULL == driver->pf_get_buffer))
    {
        log_e("bsp_display_port_get_buffer_impl: buffer not support");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    d_status = driver->pf_get_buffer(driver, buffer, size);
    return (display_status_t)d_status;
}

display_status_t bsp_display_port_init(void)
{
    display_status_t          status         = BSP_DISPLAY_WRAPPER_STATUS_OK;
    oled_handler_status_t     handler_status = OLED_HANDLER_STATUS_OK;
    bsp_display_wrapper_dev_t *wrapper_dev   = NULL;

    g_display_port_oled_intf.pf_write_cmd  = bsp_display_port_write_cmd;
    g_display_port_oled_intf.pf_write_data = bsp_display_port_write_data;
    g_display_port_oled_intf.pf_reset      = bsp_display_port_reset;
    g_display_port_oled_intf.pf_delay_ms   = bsp_display_port_delay_ms;

    handler_status = bsp_oled_handler_inst(&g_display_port_oled_handler,
                                           &g_display_port_oled_intf);
    if (OLED_HANDLER_STATUS_OK != handler_status)
    {
        log_e("bsp_display_port_init: handler init failed");
        return bsp_display_port_status_from_handler(handler_status);
    }

    status = bsp_display_wrapper_reg();
    if (BSP_DISPLAY_WRAPPER_STATUS_OK != status)
    {
        log_e("bsp_display_port_init: wrapper reg failed");
        return status;
    }

    wrapper_dev = bsp_display_wrapper_get_dev();
    if (NULL == wrapper_dev)
    {
        log_e("bsp_display_port_init: wrapper dev error");
        return BSP_DISPLAY_WRAPPER_STATUS_ERROR_SOURCE;
    }

    g_display_port_is_ready      = 1U;
    wrapper_dev->idx             = 0U;
    wrapper_dev->user_data       = &g_display_port_oled_handler;
    wrapper_dev->pf_init         = bsp_display_port_init_impl;
    wrapper_dev->pf_deinit       = bsp_display_port_deinit_impl;
    wrapper_dev->pf_clear        = bsp_display_port_clear_impl;
    wrapper_dev->pf_fill         = bsp_display_port_fill_impl;
    wrapper_dev->pf_refresh      = bsp_display_port_refresh_impl;
    wrapper_dev->pf_draw_pixel   = bsp_display_port_draw_pixel_impl;
    wrapper_dev->pf_draw_text    = bsp_display_port_draw_text_impl;
    wrapper_dev->pf_get_buffer   = bsp_display_port_get_buffer_impl;
    wrapper_dev->pf_set_backlight = NULL;
    wrapper_dev->pf_draw_image    = NULL;

    log_i("bsp dispaly port init success\n");

    return BSP_DISPLAY_WRAPPER_STATUS_OK;
}
