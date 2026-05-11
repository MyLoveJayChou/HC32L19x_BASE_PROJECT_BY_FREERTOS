/**
 * @file    bsp_oled_driver.c
 * @author  RUST HUEY
 * @brief   SH1106 OLED driver source.
 * @version 1.0.0
 * @date    2026-05-08
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "bsp_oled_driver.h"
#include "elog.h"

#include <stddef.h>

#define OLED_NOT_INITED           (0U)
#define OLED_INITED               (1U)

#define SH1106_CMD_DISPLAY_OFF    (0xAEU)
#define SH1106_CMD_DISPLAY_ON     (0xAFU)
#define SH1106_CMD_SET_LOW_COL    (0x00U)
#define SH1106_CMD_SET_HIGH_COL   (0x10U)
#define SH1106_CMD_SET_START_LINE (0x40U)
#define SH1106_CMD_SET_CONTRAST   (0x81U)
#define SH1106_CMD_SEG_REMAP      (0xA1U)
#define SH1106_CMD_NORMAL_DISPLAY (0xA6U)
#define SH1106_CMD_SET_MULTIPLEX  (0xA8U)
#define SH1106_CMD_DC_DC_CONTROL  (0xADU)
#define SH1106_CMD_DC_DC_ON       (0x8BU)
#define SH1106_CMD_SET_OFFSET     (0xD3U)
#define SH1106_CMD_SET_CLOCK      (0xD5U)
#define SH1106_CMD_SET_PRECHARGE  (0xD9U)
#define SH1106_CMD_SET_COM_PINS   (0xDAU)
#define SH1106_CMD_SET_VCOM       (0xDBU)
#define SH1106_CMD_COM_SCAN_DEC   (0xC8U)
#define SH1106_CMD_PAGE_BASE      (0xB0U)

static oled_driver_status_t bsp_oled_driver_check(
    bsp_oled_driver_t *const self)
{
    if (NULL == self)
    {
        return OLED_DRIVER_STATUS_ERROR_PARAM;
    }

    if (NULL == self->p_intf)
    {
        return OLED_DRIVER_STATUS_ERROR_PARAM;
    }

    if (NULL == self->p_intf->pf_write_cmd)
    {
        return OLED_DRIVER_STATUS_ERROR_PARAM;
    }

    if (NULL == self->p_intf->pf_write_data)
    {
        return OLED_DRIVER_STATUS_ERROR_PARAM;
    }

    return OLED_DRIVER_STATUS_OK;
}

static oled_driver_status_t bsp_oled_driver_write_cmd(
    bsp_oled_driver_t *const self,
    uint8_t cmd)
{
    return self->p_intf->pf_write_cmd(&cmd, 1U);
}

static void bsp_oled_driver_memset(uint8_t  *buffer,
                                   uint8_t   value,
                                   uint16_t  size)
{
    uint16_t index = 0U;

    for (index = 0U; size > index; index++)
    {
        buffer[index] = value;
    }
}

static oled_driver_status_t bsp_oled_driver_init_sequence(
    bsp_oled_driver_t *const self)
{
    static const uint8_t init_cmds[] = {
        SH1106_CMD_DISPLAY_OFF,
        SH1106_CMD_SET_LOW_COL,
        SH1106_CMD_SET_HIGH_COL,
        SH1106_CMD_SET_START_LINE,
        SH1106_CMD_SET_CONTRAST,
        0xCFU,
        SH1106_CMD_SEG_REMAP,
        SH1106_CMD_COM_SCAN_DEC,
        SH1106_CMD_NORMAL_DISPLAY,
        SH1106_CMD_SET_MULTIPLEX,
        0x3FU,
        SH1106_CMD_SET_OFFSET,
        0x00U,
        SH1106_CMD_SET_CLOCK,
        0x80U,
        SH1106_CMD_SET_PRECHARGE,
        0xF1U,
        SH1106_CMD_SET_COM_PINS,
        0x12U,
        SH1106_CMD_SET_VCOM,
        0x40U,
        SH1106_CMD_DC_DC_CONTROL,
        SH1106_CMD_DC_DC_ON,
        SH1106_CMD_DISPLAY_ON,
    };

    return self->p_intf->pf_write_cmd(init_cmds,
                                      (uint16_t)sizeof(init_cmds));
}

static oled_driver_status_t bsp_oled_driver_init_impl(
    bsp_oled_driver_t *const self)
{
    oled_driver_status_t status = OLED_DRIVER_STATUS_OK;

    status = bsp_oled_driver_check(self);
    if (OLED_DRIVER_STATUS_OK != status)
    {
        log_e("bsp_oled_driver_init_impl: param error");
        return status;
    }

    if (OLED_INITED == self->is_inited)
    {
        log_e("bsp_oled_driver_init_impl: already inited");
        return OLED_DRIVER_STATUS_ERROR_SOURCE;
    }

    self->is_inited = OLED_NOT_INITED;
    bsp_oled_driver_memset(self->buffer, 0x00U, OLED_DRIVER_BUFFER_SIZE);

    if (NULL != self->p_intf->pf_reset)
    {
        status = self->p_intf->pf_reset(1U);
        if (OLED_DRIVER_STATUS_OK != status)
        {
            log_e("bsp_oled_driver_init_impl: reset high failed");
            return status;
        }

        if (NULL != self->p_intf->pf_delay_ms)
        {
            status = self->p_intf->pf_delay_ms(10U);
            if (OLED_DRIVER_STATUS_OK != status)
            {
                log_e("bsp_oled_driver_init_impl: delay failed");
                return status;
            }
        }

        status = self->p_intf->pf_reset(0U);
        if (OLED_DRIVER_STATUS_OK != status)
        {
            log_e("bsp_oled_driver_init_impl: reset low failed");
            return status;
        }

        if (NULL != self->p_intf->pf_delay_ms)
        {
            status = self->p_intf->pf_delay_ms(50U);
            if (OLED_DRIVER_STATUS_OK != status)
            {
                log_e("bsp_oled_driver_init_impl: delay failed");
                return status;
            }
        }

        status = self->p_intf->pf_reset(1U);
        if (OLED_DRIVER_STATUS_OK != status)
        {
            log_e("bsp_oled_driver_init_impl: reset high failed");
            return status;
        }

        if (NULL != self->p_intf->pf_delay_ms)
        {
            status = self->p_intf->pf_delay_ms(100U);
            if (OLED_DRIVER_STATUS_OK != status)
            {
                log_e("bsp_oled_driver_init_impl: delay failed");
                return status;
            }
        }
    }

    status = bsp_oled_driver_init_sequence(self);
    if (OLED_DRIVER_STATUS_OK != status)
    {
        log_e("bsp_oled_driver_init_impl: init sequence failed");
        return status;
    }

    self->is_inited = OLED_INITED;
    return self->pf_clear(self);
}

static oled_driver_status_t bsp_oled_driver_deinit_impl(
    bsp_oled_driver_t *const self)
{
    if (NULL == self)
    {
        log_e("bsp_oled_driver_deinit_impl: param error");
        return OLED_DRIVER_STATUS_ERROR_PARAM;
    }

    self->is_inited = OLED_NOT_INITED;
    bsp_oled_driver_memset(self->buffer, 0x00U, OLED_DRIVER_BUFFER_SIZE);

    return OLED_DRIVER_STATUS_OK;
}

static oled_driver_status_t bsp_oled_driver_clear_impl(
    bsp_oled_driver_t *const self)
{
    oled_driver_status_t status = OLED_DRIVER_STATUS_OK;

    status = bsp_oled_driver_check(self);
    if (OLED_DRIVER_STATUS_OK != status)
    {
        log_e("bsp_oled_driver_clear_impl: param error");
        return status;
    }

    bsp_oled_driver_memset(self->buffer, 0x00U, OLED_DRIVER_BUFFER_SIZE);

    return self->pf_refresh(self);
}

static oled_driver_status_t bsp_oled_driver_fill_impl(
    bsp_oled_driver_t *const self,
    oled_color_t color)
{
    oled_driver_status_t status = OLED_DRIVER_STATUS_OK;
    uint8_t              value  = 0x00U;

    status = bsp_oled_driver_check(self);
    if (OLED_DRIVER_STATUS_OK != status)
    {
        log_e("bsp_oled_driver_fill_impl: param error");
        return status;
    }

    if (OLED_COLOR_ON == color)
    {
        value = 0xFFU;
    }

    bsp_oled_driver_memset(self->buffer, value, OLED_DRIVER_BUFFER_SIZE);

    return self->pf_refresh(self);
}

static oled_driver_status_t bsp_oled_driver_refresh_impl(
    bsp_oled_driver_t *const self)
{
    oled_driver_status_t status = OLED_DRIVER_STATUS_OK;
    uint8_t              page   = 0U;
    uint8_t              low    = 0U;
    uint8_t              high   = 0U;
    uint16_t             offset = 0U;

    status = bsp_oled_driver_check(self);
    if (OLED_DRIVER_STATUS_OK != status)
    {
        log_e("bsp_oled_driver_refresh_impl: param error");
        return status;
    }

    low  = (uint8_t)(OLED_DRIVER_COL_OFFSET & 0x0FU);
    high = (uint8_t)(0x10U | (OLED_DRIVER_COL_OFFSET >> 4U));

    for (page = 0U; OLED_DRIVER_PAGE_COUNT > page; page++)
    {
        status = bsp_oled_driver_write_cmd(
            self,
            (uint8_t)(SH1106_CMD_PAGE_BASE | page));
        if (OLED_DRIVER_STATUS_OK != status)
        {
            log_e("bsp_oled_driver_refresh_impl: page cmd failed");
            return status;
        }

        status = bsp_oled_driver_write_cmd(
            self,
            (uint8_t)(SH1106_CMD_SET_LOW_COL | low));
        if (OLED_DRIVER_STATUS_OK != status)
        {
            log_e("bsp_oled_driver_refresh_impl: low col failed");
            return status;
        }

        status = bsp_oled_driver_write_cmd(self, high);
        if (OLED_DRIVER_STATUS_OK != status)
        {
            log_e("bsp_oled_driver_refresh_impl: high col failed");
            return status;
        }

        offset = (uint16_t)page * OLED_DRIVER_WIDTH;
        status = self->p_intf->pf_write_data(&self->buffer[offset],
                                             OLED_DRIVER_WIDTH);
        if (OLED_DRIVER_STATUS_OK != status)
        {
            log_e("bsp_oled_driver_refresh_impl: data write failed");
            return status;
        }
    }

    return OLED_DRIVER_STATUS_OK;
}

static oled_driver_status_t bsp_oled_driver_draw_pixel_impl(
    bsp_oled_driver_t *const self,
    uint8_t x,
    uint8_t y,
    oled_color_t color)
{
    oled_driver_status_t status = OLED_DRIVER_STATUS_OK;
    uint16_t             index  = 0U;
    uint8_t              mask   = 0U;

    status = bsp_oled_driver_check(self);
    if (OLED_DRIVER_STATUS_OK != status)
    {
        log_e("bsp_oled_driver_draw_pixel_impl: param error");
        return status;
    }

    if ((OLED_DRIVER_WIDTH <= x) || (OLED_DRIVER_HEIGHT <= y))
    {
        log_e("bsp_oled_driver_draw_pixel_impl: out of range");
        return OLED_DRIVER_STATUS_ERROR_PARAM;
    }

    index = (uint16_t)(y / 8U) * OLED_DRIVER_WIDTH + x;
    mask  = (uint8_t)(1U << (y % 8U));

    if (OLED_COLOR_ON == color)
    {
        self->buffer[index] |= mask;
    }
    else
    {
        self->buffer[index] &= (uint8_t)(~mask);
    }

    return OLED_DRIVER_STATUS_OK;
}

static oled_driver_status_t bsp_oled_driver_get_buffer_impl(
    bsp_oled_driver_t *const self,
    uint8_t **const buffer,
    uint16_t *const size)
{
    oled_driver_status_t status = OLED_DRIVER_STATUS_OK;

    status = bsp_oled_driver_check(self);
    if (OLED_DRIVER_STATUS_OK != status)
    {
        log_e("bsp_oled_driver_get_buffer_impl: param error");
        return status;
    }

    if ((NULL == buffer) || (NULL == size))
    {
        log_e("bsp_oled_driver_get_buffer_impl: output param error");
        return OLED_DRIVER_STATUS_ERROR_PARAM;
    }

    *buffer = self->buffer;
    *size   = OLED_DRIVER_BUFFER_SIZE;

    return OLED_DRIVER_STATUS_OK;
}

oled_driver_status_t bsp_oled_driver_inst(bsp_oled_driver_t *const self,
                                          oled_interface_t *const p_intf)
{
    oled_driver_status_t status = OLED_DRIVER_STATUS_OK;

    if ((NULL == self) || (NULL == p_intf))
    {
        log_e("bsp_oled_driver_inst: param error");
        return OLED_DRIVER_STATUS_ERROR_PARAM;
    }

    if ((NULL == p_intf->pf_write_cmd) ||
        (NULL == p_intf->pf_write_data))
    {
        log_e("bsp_oled_driver_inst: interface param error");
        return OLED_DRIVER_STATUS_ERROR_PARAM;
    }

    self->is_inited     = OLED_NOT_INITED;
    self->p_intf        = p_intf;
    self->pf_init       = bsp_oled_driver_init_impl;
    self->pf_deinit     = bsp_oled_driver_deinit_impl;
    self->pf_clear      = bsp_oled_driver_clear_impl;
    self->pf_fill       = bsp_oled_driver_fill_impl;
    self->pf_refresh    = bsp_oled_driver_refresh_impl;
    self->pf_draw_pixel = bsp_oled_driver_draw_pixel_impl;
    self->pf_get_buffer = bsp_oled_driver_get_buffer_impl;

    bsp_oled_driver_memset(self->buffer, 0x00U, OLED_DRIVER_BUFFER_SIZE);

    status = self->pf_init(self);
    if (OLED_DRIVER_STATUS_OK != status)
    {
        log_e("bsp_oled_driver_inst: init failed");
        self->p_intf        = NULL;
        self->pf_init       = NULL;
        self->pf_deinit     = NULL;
        self->pf_clear      = NULL;
        self->pf_fill       = NULL;
        self->pf_refresh    = NULL;
        self->pf_draw_pixel = NULL;
        self->pf_get_buffer = NULL;
        return status;
    }

    log_i("bsp_oled_driver_inst success");
    return OLED_DRIVER_STATUS_OK;
}
