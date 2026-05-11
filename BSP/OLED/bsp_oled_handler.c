/**
 * @file    bsp_oled_handler.c
 * @author  RUST HUEY
 * @brief   OLED handler source.
 * @version 1.0.0
 * @date    2026-05-09
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "bsp_oled_handler.h"
#include "bsp_oled_text.h"
#include "lvgl.h"
#include "elog.h"

#include <stddef.h>

#define OLED_HANDLER_NOT_INITED  (0U)
#define OLED_HANDLER_INITED      (1U)

static oled_handler_status_t
bsp_oled_handler_status_from_driver(oled_driver_status_t status)
{
    return (oled_handler_status_t)status;
}

static oled_handler_status_t bsp_oled_handler_draw_text_by_font_impl(
    bsp_oled_handler_t *const self,
    uint8_t                   x,
    uint8_t                   y,
    const char               *text,
    oled_text_font_t          font);

static oled_handler_status_t bsp_oled_handler_check(
    bsp_oled_handler_t *const self)
{
    if (NULL == self)
    {
        return OLED_HANDLER_STATUS_ERROR_PARAM;
    }

    if (NULL == self->intf.pf_write_cmd)
    {
        return OLED_HANDLER_STATUS_ERROR_PARAM;
    }

    if (NULL == self->intf.pf_write_data)
    {
        return OLED_HANDLER_STATUS_ERROR_PARAM;
    }

    return OLED_HANDLER_STATUS_OK;
}

static oled_handler_status_t bsp_oled_handler_check_ready(
    bsp_oled_handler_t *const self)
{
    oled_handler_status_t status = OLED_HANDLER_STATUS_OK;

    status = bsp_oled_handler_check(self);
    if (OLED_HANDLER_STATUS_OK != status)
    {
        log_e("bsp_oled_handler_check_ready: param error");
        return status;
    }

    if (OLED_HANDLER_INITED != self->is_inited)
    {
        log_e("bsp_oled_handler_check_ready: not inited");
        return OLED_HANDLER_STATUS_ERROR_SOURCE;
    }

    if (NULL == self->driver.pf_refresh)
    {
        log_e("bsp_oled_handler_check_ready: driver invalid");
        return OLED_HANDLER_STATUS_ERROR_SOURCE;
    }

    return OLED_HANDLER_STATUS_OK;
}

static oled_handler_status_t bsp_oled_handler_init_impl(
    bsp_oled_handler_t *const self)
{
    oled_handler_status_t handler_status = OLED_HANDLER_STATUS_OK;
    oled_driver_status_t  driver_status  = OLED_DRIVER_STATUS_OK;

    handler_status = bsp_oled_handler_check(self);
    if (OLED_HANDLER_STATUS_OK != handler_status)
    {
        log_e("bsp_oled_handler_init_impl: param error");
        return handler_status;
    }

    if (OLED_HANDLER_INITED == self->is_inited)
    {
        log_e("bsp_oled_handler_init_impl: already inited");
        return OLED_HANDLER_STATUS_ERROR_SOURCE;
    }

    driver_status = bsp_oled_driver_inst(&self->driver, &self->intf);
    if (OLED_DRIVER_STATUS_OK != driver_status)
    {
        log_e("bsp_oled_handler_init_impl: driver init failed");
        return bsp_oled_handler_status_from_driver(driver_status);
    }

    self->is_inited = OLED_HANDLER_INITED;

    return OLED_HANDLER_STATUS_OK;
}

static oled_handler_status_t bsp_oled_handler_deinit_impl(
    bsp_oled_handler_t *const self)
{
    oled_handler_status_t status        = OLED_HANDLER_STATUS_OK;
    oled_driver_status_t  driver_status = OLED_DRIVER_STATUS_OK;

    status = bsp_oled_handler_check_ready(self);
    if (OLED_HANDLER_STATUS_OK != status)
    {
        return status;
    }

    if (NULL != self->driver.pf_deinit)
    {
        driver_status = self->driver.pf_deinit(&self->driver);
        status = bsp_oled_handler_status_from_driver(driver_status);
    }

    self->is_inited = OLED_HANDLER_NOT_INITED;

    return status;
}

static oled_handler_status_t bsp_oled_handler_clear_impl(
    bsp_oled_handler_t *const self)
{
    oled_handler_status_t status        = OLED_HANDLER_STATUS_OK;
    oled_driver_status_t  driver_status = OLED_DRIVER_STATUS_OK;

    status = bsp_oled_handler_check_ready(self);
    if (OLED_HANDLER_STATUS_OK != status)
    {
        return status;
    }

    driver_status = self->driver.pf_clear(&self->driver);
    return bsp_oled_handler_status_from_driver(driver_status);
}

static oled_handler_status_t bsp_oled_handler_fill_impl(
    bsp_oled_handler_t *const self,
    oled_color_t               color)
{
    oled_handler_status_t status        = OLED_HANDLER_STATUS_OK;
    oled_driver_status_t  driver_status = OLED_DRIVER_STATUS_OK;

    status = bsp_oled_handler_check_ready(self);
    if (OLED_HANDLER_STATUS_OK != status)
    {
        return status;
    }

    driver_status = self->driver.pf_fill(&self->driver, color);
    return bsp_oled_handler_status_from_driver(driver_status);
}

static oled_handler_status_t bsp_oled_handler_refresh_impl(
    bsp_oled_handler_t *const self)
{
    oled_handler_status_t status        = OLED_HANDLER_STATUS_OK;
    oled_driver_status_t  driver_status = OLED_DRIVER_STATUS_OK;

    status = bsp_oled_handler_check_ready(self);
    if (OLED_HANDLER_STATUS_OK != status)
    {
        return status;
    }

    driver_status = self->driver.pf_refresh(&self->driver);
    return bsp_oled_handler_status_from_driver(driver_status);
}

static oled_handler_status_t bsp_oled_handler_draw_pixel_impl(
    bsp_oled_handler_t *const self,
    uint8_t                   x,
    uint8_t                   y,
    oled_color_t              color)
{
    oled_handler_status_t status        = OLED_HANDLER_STATUS_OK;
    oled_driver_status_t  driver_status = OLED_DRIVER_STATUS_OK;

    status = bsp_oled_handler_check_ready(self);
    if (OLED_HANDLER_STATUS_OK != status)
    {
        return status;
    }

    driver_status = self->driver.pf_draw_pixel(&self->driver,
                                               x,
                                               y,
                                               color);
    return bsp_oled_handler_status_from_driver(driver_status);
}

static oled_handler_status_t bsp_oled_handler_draw_text_impl(
    bsp_oled_handler_t *const self,
    uint8_t                   x,
    uint8_t                   y,
    const char               *text)
{
    return bsp_oled_handler_draw_text_by_font_impl(self,
                                                   x,
                                                   y,
                                                   text,
                                                   (0U != LVGL_USE_FONT_CN_14) ?
                                                   OLED_TEXT_FONT_14 :
                                                   OLED_TEXT_FONT_16);
}

static oled_handler_status_t bsp_oled_handler_draw_text_by_font_impl(
    bsp_oled_handler_t *const self,
    uint8_t                   x,
    uint8_t                   y,
    const char               *text,
    oled_text_font_t          font)
{
    oled_handler_status_t ready_status  = OLED_HANDLER_STATUS_OK;
    oled_driver_status_t  driver_status = OLED_DRIVER_STATUS_OK;

    if (NULL == text)
    {
        log_e("bsp_oled_handler_draw_text_impl: text error");
        return OLED_HANDLER_STATUS_ERROR_PARAM;
    }

    ready_status = bsp_oled_handler_check_ready(self);
    if (OLED_HANDLER_STATUS_OK != ready_status)
    {
        return ready_status;
    }

    driver_status = bsp_oled_text_draw_utf8_by_font(&self->driver,
                                                    x,
                                                    y,
                                                    text,
                                                    font);
    if (OLED_DRIVER_STATUS_OK != driver_status)
    {
        log_e("bsp_oled_handler_draw_text_by_font_impl: draw utf8 failed");
        return bsp_oled_handler_status_from_driver(driver_status);
    }

    return OLED_HANDLER_STATUS_OK;
}

static oled_handler_status_t bsp_oled_handler_get_driver_impl(
    bsp_oled_handler_t *const self,
    bsp_oled_driver_t **const p_driver)
{
    oled_handler_status_t status = OLED_HANDLER_STATUS_OK;

    if (NULL == p_driver)
    {
        log_e("bsp_oled_handler_get_driver_impl: output param error");
        return OLED_HANDLER_STATUS_ERROR_PARAM;
    }

    status = bsp_oled_handler_check_ready(self);
    if (OLED_HANDLER_STATUS_OK != status)
    {
        return status;
    }

    *p_driver = &self->driver;

    return OLED_HANDLER_STATUS_OK;
}

oled_handler_status_t bsp_oled_handler_inst(bsp_oled_handler_t *const self,
                                            oled_interface_t   *const p_intf)
{
    oled_handler_status_t status = OLED_HANDLER_STATUS_OK;

    if ((NULL == self) || (NULL == p_intf))
    {
        log_e("bsp_oled_handler_inst: param error");
        return OLED_HANDLER_STATUS_ERROR_PARAM;
    }

    if ((NULL == p_intf->pf_write_cmd) ||
        (NULL == p_intf->pf_write_data))
    {
        log_e("bsp_oled_handler_inst: interface param error");
        return OLED_HANDLER_STATUS_ERROR_PARAM;
    }

    self->is_inited      = OLED_HANDLER_NOT_INITED;
    self->intf           = *p_intf;
    self->pf_init        = bsp_oled_handler_init_impl;
    self->pf_deinit      = bsp_oled_handler_deinit_impl;
    self->pf_clear       = bsp_oled_handler_clear_impl;
    self->pf_fill        = bsp_oled_handler_fill_impl;
    self->pf_refresh     = bsp_oled_handler_refresh_impl;
    self->pf_draw_pixel  = bsp_oled_handler_draw_pixel_impl;
    self->pf_draw_text   = bsp_oled_handler_draw_text_impl;
    self->pf_draw_text_by_font = bsp_oled_handler_draw_text_by_font_impl;
    self->pf_get_driver  = bsp_oled_handler_get_driver_impl;

    status = self->pf_init(self);
    if (OLED_HANDLER_STATUS_OK != status)
    {
        log_e("bsp_oled_handler_inst: init failed");
        self->pf_init       = NULL;
        self->pf_deinit     = NULL;
        self->pf_clear      = NULL;
        self->pf_fill       = NULL;
        self->pf_refresh    = NULL;
        self->pf_draw_pixel = NULL;
        self->pf_draw_text  = NULL;
        self->pf_draw_text_by_font = NULL;
        self->pf_get_driver = NULL;
        return status;
    }

    log_i("bsp_oled_handler_inst success");
    return OLED_HANDLER_STATUS_OK;
}
