#include "gpio.h"
#include "sysctrl.h"
#include "elog.h"
#include "system_hc32l19x.h"
#include "ddl.h"
#include "flash.h"
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief Init system clock 48MHz
 * 
 */
static void clock_init(void)
{
    stc_sysctrl_pll_cfg_t  stcPllCfg;

    Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);
    Sysctrl_ClkSourceEnable(SysctrlClkRCH, TRUE);

    Flash_WaitCycle(FlashWaitCycle1);

    DDL_ZERO_STRUCT(stcPllCfg);
    stcPllCfg.enInFreq    = SysctrlPllInFreq20_24MHz;
    stcPllCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;
    stcPllCfg.enPllClkSrc = SysctrlPllRch;
    stcPllCfg.enPllMul    = SysctrlPllMul2;

    Sysctrl_SetPLLFreq(&stcPllCfg);
    Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
    Sysctrl_SysClkSwitch(SysctrlClkPLL);
    Sysctrl_SetHCLKDiv(SysctrlHclkDiv1);
    Sysctrl_SetPCLKDiv(SysctrlPclkDiv1);

    SystemCoreClock = Sysctrl_GetHClkFreq();
}

static void task_log_1s(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        log_d("task_log_1s: 1s tick");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void task_log_2s(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        log_d("task_log_2s: 2s tick");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}


int main(void)
{
    /* Set System clock freq 48MHz */
    clock_init();

    /* EasyLogger Init, use RTT print */
    elog_init();
    log_i("HC32L19x PROJECT RUNNING");
    log_d("System clock: %u Hz", SystemCoreClock);

    xTaskCreate(task_log_1s,
                "log1s",
                128,
                NULL,
                2,
                NULL);

    xTaskCreate(task_log_2s,
                "log2s",
                128,
                NULL,
                2,
                NULL);

    vTaskStartScheduler();

    for (;;)
    {
    }
}
