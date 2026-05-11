/**
 * @file    board.h
 * @author  RUST HUEY
 * @brief   Board level resource definition.
 * @version 1.0.0
 * @date    2026-05-09
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __BOARD_H__
#define __BOARD_H__

#include "gpio.h"

#define BOARD_OLED_CS_PORT    (GpioPortA)
#define BOARD_OLED_CS_PIN     (GpioPin4)
#define BOARD_OLED_DC_PORT    (GpioPortA)
#define BOARD_OLED_DC_PIN     (GpioPin3)
#define BOARD_OLED_RES_PORT   (GpioPortA)
#define BOARD_OLED_RES_PIN    (GpioPin10)
#define BOARD_OLED_SCL_PORT   (GpioPortA)
#define BOARD_OLED_SCL_PIN    (GpioPin5)
#define BOARD_OLED_MISO_PORT  (GpioPortA)
#define BOARD_OLED_MISO_PIN   (GpioPin6)
#define BOARD_OLED_MOSI_PORT  (GpioPortA)
#define BOARD_OLED_MOSI_PIN   (GpioPin7)

/**
 * @brief Initialize board level resources.
 */
void board_init(void);

#endif /* __BOARD_H__ */
