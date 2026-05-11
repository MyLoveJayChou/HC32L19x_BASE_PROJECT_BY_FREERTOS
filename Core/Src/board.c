/**
 * @file    board.c
 * @author  RUST HUEY
 * @brief   Board level resource source.
 * @version 1.0.0
 * @date    2026-05-09
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "board.h"
#include "ddl.h"
#include "flash.h"
#include "sysctrl.h"
#include "system_hc32l19x.h"

/**
 * @brief Initialize system clock to 48 MHz.
 */
static void board_clock_init(void)
{
    stc_sysctrl_pll_cfg_t stc_pll_cfg;

    Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);
    Sysctrl_ClkSourceEnable(SysctrlClkRCH, TRUE);

    Flash_WaitCycle(FlashWaitCycle1);

    DDL_ZERO_STRUCT(stc_pll_cfg);
    stc_pll_cfg.enInFreq    = SysctrlPllInFreq20_24MHz;
    stc_pll_cfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;
    stc_pll_cfg.enPllClkSrc = SysctrlPllRch;
    stc_pll_cfg.enPllMul    = SysctrlPllMul2;

    Sysctrl_SetPLLFreq(&stc_pll_cfg);
    Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
    Sysctrl_SysClkSwitch(SysctrlClkPLL);
    Sysctrl_SetHCLKDiv(SysctrlHclkDiv1);
    Sysctrl_SetPCLKDiv(SysctrlPclkDiv1);

    SystemCoreClock = Sysctrl_GetHClkFreq();
}

/**
 * @brief Configure one GPIO as push-pull output.
 *
 * @param[in] port GPIO port.
 * @param[in] pin  GPIO pin.
 */
static void board_gpio_output_init(en_gpio_port_t port,
                                   en_gpio_pin_t  pin)
{
    stc_gpio_cfg_t gpio_cfg;

    DDL_ZERO_STRUCT(gpio_cfg);
    gpio_cfg.enDir      = GpioDirOut;
    gpio_cfg.enDrv      = GpioDrvH;
    gpio_cfg.enPu       = GpioPuDisable;
    gpio_cfg.enPd       = GpioPdDisable;
    gpio_cfg.enOD       = GpioOdDisable;
    gpio_cfg.enCtrlMode = GpioAHB;
    gpio_cfg.bOutputVal = TRUE;

    Gpio_Init(port, pin, &gpio_cfg);
}

/**
 * @brief Initialize board level resources.
 */
void board_init(void)
{
    board_clock_init();

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);

    board_gpio_output_init(BOARD_OLED_CS_PORT, BOARD_OLED_CS_PIN);
    board_gpio_output_init(BOARD_OLED_DC_PORT, BOARD_OLED_DC_PIN);
    board_gpio_output_init(BOARD_OLED_RES_PORT, BOARD_OLED_RES_PIN);
    board_gpio_output_init(BOARD_OLED_SCL_PORT, BOARD_OLED_SCL_PIN);
    board_gpio_output_init(BOARD_OLED_MOSI_PORT, BOARD_OLED_MOSI_PIN);

    Gpio_SetIO(BOARD_OLED_CS_PORT, BOARD_OLED_CS_PIN);
    Gpio_ClrIO(BOARD_OLED_DC_PORT, BOARD_OLED_DC_PIN);
    Gpio_SetIO(BOARD_OLED_RES_PORT, BOARD_OLED_RES_PIN);
    Gpio_ClrIO(BOARD_OLED_SCL_PORT, BOARD_OLED_SCL_PIN);
    Gpio_ClrIO(BOARD_OLED_MOSI_PORT, BOARD_OLED_MOSI_PIN);
}
