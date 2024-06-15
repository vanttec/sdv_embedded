/*
 * requirements.c
 *
 *  Created on: May 22, 2023
 *      Author: Z0180064
 */

#include "cmsis_os.h"
#include "main.h"
#include "vanttec_canlib_rx_task.h"
#include "vanttec_canlib_tx_task.h"
#include <stddef.h>
#include <vanttec_sdv_ids.h>

osThreadId_t hbTaskHandle;
const osThreadAttr_t hbTaskAttributes = {
    .name = "canlib_hb",
    .stack_size = 128 * 1};

void hb_task(void *args)
{
    for (;;)
    {

        HAL_GPIO_TogglePin(DEBUG_2_GPIO_Port,DEBUG_2_Pin);
        osDelay(1000);
    }
}

void init_requirements_task()
{
    hbTaskHandle = osThreadNew(hb_task, NULL, &hbTaskAttributes);
}
