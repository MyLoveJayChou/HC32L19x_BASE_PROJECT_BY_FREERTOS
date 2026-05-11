/**
 * @file    bsp_display_port.h
 * @author  RUST HUEY
 * @brief   Display port header.
 * @version 1.0.0
 * @date    2026-05-09
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __BSP_DISPLAY_PORT_H__
#define __BSP_DISPLAY_PORT_H__

#include "bsp_display_wrapper.h"

/**
 * @brief Register OLED display device into wrapper.
 *
 * @return Status code.
 */
display_status_t bsp_display_port_init(void);

#endif /* __BSP_DISPLAY_PORT_H__ */
