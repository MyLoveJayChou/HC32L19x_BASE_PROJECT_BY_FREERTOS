#include "board.h"
#include "bsp_display_port.h"
#include "bsp_display_wrapper.h"
#include "elog.h"
#include "FreeRTOS.h"
#include "system_hc32l19x.h"
#include "task.h"


int main(void)
{
    /* Init board level resources */
    board_init();

    /* EasyLogger Init, use RTT print */
    elog_init();
    log_i("HC32L19x PROJECT RUNNING");
    log_d("System clock: %u Hz", SystemCoreClock);

    bsp_display_port_init();

    bsp_display_wrapper_draw_text_by_font(30,
                                          6,
                                          "图鲸卫星",
                                          OLED_TEXT_FONT_14);
    bsp_display_wrapper_draw_text_by_font(26,
                                          30,
                                          "图鲸卫星",
                                          OLED_TEXT_FONT_16);

    bsp_display_wrapper_refresh();

    vTaskStartScheduler();

    for (;;)
    {
    }
}
