/*
 * requirements.c
 *
 *  Created on: May 22, 2023
 *      Author: Z0180064
 */

#include "main.h"
#include "vanttec_canlib_tx_task.h"
#include "vanttec_canlib_rx_task.h"
#include "cmsis_os.h"
#include <stddef.h>
#include <vanttec_sdv_ids.h>

osThreadId_t emergencystopTaskHandle;
const osThreadAttr_t emergencystopTaskAttributes = {
    .name = "emergencystop",
    .stack_size = 128 * 4};
osThreadId_t hbTaskHandle;
const osThreadAttr_t hbTaskAttributes = {
    .name = "canlib_hb",
    .stack_size = 128 * 4};
osThreadId_t drivemodeTaskHandle;
const osThreadAttr_t drivemodeTaskAttributes = {
    .name = "drivemode",
    .stack_size = 128 * 4};
osThreadId_t driverpresentTaskHandle;
const osThreadAttr_t driverpresentTaskAttributes = {
    .name = "driverpresent",
    .stack_size = 128 * 4};
osThreadId_t reverseTaskHandle;
const osThreadAttr_t reverseTaskAttributes = {
    .name = "reserve",
    .stack_size = 128 * 4};
osThreadId_t frenomanualTaskHandle;
const osThreadAttr_t frenomanualTaskAttributes = {
    .name = "frenomanual",
    .stack_size = 128 * 4};
osThreadId_t driverfaultTaskHandle;
const osThreadAttr_t driverfaultTaskAttributes = {
    .name = "driverfault",
    .stack_size = 128 * 4};
osThreadId_t xboxTaskHandle;
const osThreadAttr_t xboxTaskAttributes = {
    .name = "xbox",
    .stack_size = 128 * 4};

void emergencystop_task(void *args)
{
    uint8_t buf[8];
    uint8_t emergencystop_data = 0;
    register_canlib_rx(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_ESTOP, VANTTEC_CANLIB_BYTE, &emergencystop_data, 1);
    for (;;)
    {
        if (emergencystop_data == 1)
        {
            // Velocity to 0
            buf[0] = 0x05;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x05, buf, 2);
            // Switch to manual pedal
            buf[0] = 0x06;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x06, buf, 2);
            buf[0] = 0x07;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x07, buf, 2);
            // Return emergency signal back to 0
            buf[0] = VANTTEC_CAN_ID_ESTOP;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_ESTOP, buf, 2);
        }
        osDelay(10);
    }
}
void hb_task(void *args)
{
    uint8_t data = 0;
    for (;;)
    {
        canlib_send_debug_string("Hello");
        canlib_send_byte(VANTTEC_CAN_ID_HB, data);
        data++;
        osDelay(1000);
    }
}
void drivemode_task(void *args)
{
    uint8_t buf[8];
    uint8_t drivemode_data = 3;
    register_canlib_rx(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_DRIVE_MODE, VANTTEC_CANLIB_BYTE, &drivemode_data, 1);
    for (;;)
    {
        if (drivemode_data == 1)
        {
            // Activate autonomous mode
            buf[0] = 0x07;
            buf[1] = 0x1;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x07, buf, 2);
            buf[0] = 0x05;
            buf[1] = 0x2;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x05, buf, 2);
            buf[0] = 0x06;
            buf[1] = 0x1;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x06, buf, 2);
            // Return drivemode signal back to 3
            buf[0] = VANTTEC_CAN_ID_DRIVE_MODE;
            buf[1] = 0x3;
            update_table(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_DRIVE_MODE, buf, 2);
        }
        else if (drivemode_data == 0)
        {
            // Activate manual mode
            buf[0] = 0x07;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x07, buf, 2);
            buf[0] = 0x05;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x05, buf, 2);
            buf[0] = 0x06;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x06, buf, 2);
            // Return drivemode signal back to 3
            buf[0] = VANTTEC_CAN_ID_DRIVE_MODE;
            buf[1] = 0x3;
            update_table(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_DRIVE_MODE, buf, 2);
        }
        osDelay(10);
    }
}
void driverpresent_task(void *args)
{
    for (;;)
    {
        osDelay(10);
    }
}
void reverse_task(void *args)
{
    uint8_t buf[8];
    uint8_t reverse_data = 0;
    register_canlib_rx(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_REVERSE, VANTTEC_CANLIB_BYTE, &reverse_data, 1);
    for (;;)
    {
        if (reverse_data == 1)
        {
            // Velocity to 0
            buf[0] = 0x05;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x05, buf, 2);
            // Switch to manual pedal
            buf[0] = 0x06;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x06, buf, 2);
            buf[0] = 0x07;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x07, buf, 2);
            // Return reverse signal back to 0
            buf[0] = VANTTEC_CAN_ID_REVERSE;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_REVERSE, buf, 2);
        }

        osDelay(10);
    }
}
void frenomanual_task(void *args)
{
    uint8_t buf[8];
    uint8_t frenomanual_data = 0;
    register_canlib_rx(VANTTEC_CAN_ID_THROTTLE_TX, VANTTEC_CAN_ID_FRENO_MANUAL, VANTTEC_CANLIB_BYTE, &frenomanual_data, 1);
    for (;;)
    {

        osDelay(10);
    }
}
void driverfault_task(void *args)
{
    uint8_t buf[8];
    uint8_t driverfault_data = 0;
    uint8_t last_driverfault_data = 0;
    register_canlib_rx(VANTTEC_CAN_ID_STEPPER_TX, VANTTEC_CAN_ID_DRIVER_FAULT, VANTTEC_CANLIB_BYTE, &driverfault_data, 1);
    for (;;)
    {

        if (driverfault_data == 1)
        {
            // Velocity to 0
            buf[0] = 0x05;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x05, buf, 2);
            // Switch to manual pedal
            buf[0] = 0x06;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x06, buf, 2);
            buf[0] = 0x07;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x07, buf, 2);
            // Return reverse signal back to 0
            buf[0] = VANTTEC_CAN_ID_DRIVER_FAULT;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_STEPPER_TX, VANTTEC_CAN_ID_DRIVER_FAULT, buf, 2);
        }
        osDelay(10);
    }
}
void xbox_task(void *args)
{
    uint8_t buf[8];
    uint8_t xbox_data = 0;
    register_canlib_rx(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_XBOX, VANTTEC_CANLIB_BYTE, &xbox_data, 1);
    for (;;)
    {

        if (xbox_data == 1)
        {
            // Return Velocity to 0 if auto mode is changed to Setpoint_Controller
            buf[0] = 0x05;
            buf[1] = 0x2;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x05, buf, 2);

            buf[0] = VANTTEC_CAN_ID_XBOX;
            buf[1] = 0x3;
            update_table(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_XBOX, buf, 2);

        }
        else if (xbox_data==0){
            // Return Velocity to 0 if auto mode is changed to Xbox_Controller
            buf[0] = 0x05;
            buf[1] = 0x2;
            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x05, buf, 2);

            buf[0] = VANTTEC_CAN_ID_XBOX;
            buf[1] = 0x3;
            update_table(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_XBOX, buf, 2);
        }
        osDelay(10);
    }
}

void init_requirements_task()
{
    emergencystopTaskHandle = osThreadNew(emergencystop_task, NULL, &emergencystopTaskAttributes);
    hbTaskHandle = osThreadNew(hb_task, NULL, &hbTaskAttributes);
    drivemodeTaskHandle = osThreadNew(drivemode_task, NULL, &drivemodeTaskAttributes);
    // driverpresentTaskHandle = osThreadNew(driverpresent_task, NULL, &driverpresentTaskAttributes);
    reverseTaskHandle = osThreadNew(reverse_task, NULL, &reverseTaskAttributes);
    frenomanualTaskHandle = osThreadNew(frenomanual_task, NULL, &frenomanualTaskAttributes);
    driverfaultTaskHandle = osThreadNew(driverfault_task, NULL, &driverfaultTaskAttributes);
    xboxTaskHandle = osThreadNew(xbox_task, NULL, &xboxTaskAttributes);

}
