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

osThreadId_t emergencystopTaskHandle;
const osThreadAttr_t emergencystopTaskAttributes = {.name = "emergencystop",
                                                    .stack_size = 128 * 4};
osThreadId_t hbTaskHandle;
const osThreadAttr_t hbTaskAttributes = {.name = "canlib_hb",
                                         .stack_size = 128 * 1};
osThreadId_t drivemodeTaskHandle;
const osThreadAttr_t driveModeTaskAttributes = {.name = "drivemode",
                                                .stack_size = 128 * 4};
osThreadId_t driverpresentTaskHandle;
const osThreadAttr_t driverpresentTaskAttributes = {.name = "driverpresent",
                                                    .stack_size = 128 * 4};
osThreadId_t reverseTaskHandle;
const osThreadAttr_t reverseTaskAttributes = {.name = "reserve",
                                              .stack_size = 128 * 4};
osThreadId_t pedalBrakeTaskHandle;
const osThreadAttr_t pedalBrakeTaskAttributes = {.name = "frenomanual",
                                                 .stack_size = 128 * 4};
osThreadId_t driverfaultTaskHandle;
const osThreadAttr_t driverFaultTaskAttributes = {.name = "driverfault",
                                                  .stack_size = 128 * 4};

void emergency_stop_task(void *args) {
  uint8_t buf[8];
  uint8_t e_stop_data = 0;
  register_canlib_rx(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_ESTOP,
                     VANTTEC_CANLIB_BYTE, &e_stop_data, 1);
  for (;;) {
    if (e_stop_data == 1) {
      // Activate manual mode
      buf[0] = VANTTEC_CAN_ID_ES_STEERING;
      buf[1] = 0x0;
      update_table(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_ES_STEERING, buf,
                   2);

      // Return emergency signal back to 0
      buf[0] = VANTTEC_CAN_ID_ESTOP;
      buf[1] = 0x0;
      update_table(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_ESTOP, buf, 2);
    }
    osDelay(100);
  }
}

void hb_task(void *args) {
  uint8_t data = 0;
  for (;;) {
    canlib_send_byte(VANTTEC_CAN_ID_HB, data);
    HAL_GPIO_TogglePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin);
    data++;
    osDelay(5000);
  }
}
void drive_mode_task(void *args) {
  uint8_t buf[8];
  uint8_t drivemode_data = 3;
  register_canlib_rx(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_DRIVE_MODE,
                     VANTTEC_CANLIB_BYTE, &drivemode_data, 1);
  for (;;) {
    if (drivemode_data == 1) {
      // Activate autonomous mode
      buf[0] = VANTTEC_CAN_ID_DR_STEERING;
      buf[1] = 0x1;
      update_table(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_DR_STEERING, buf,
                   2);

      // Return drivemode signal back to 3
      buf[0] = VANTTEC_CAN_ID_DRIVE_MODE;
      buf[1] = 0x3;
      update_table(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_DRIVE_MODE, buf,
                   2);
    } else if (drivemode_data == 0) {
      // Activate manual mode
      buf[0] = VANTTEC_CAN_ID_DR_STEERING;
      buf[1] = 0x0;
      update_table(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_DR_STEERING, buf,
                   2);

      // Return drivemode signal back to 3
      buf[0] = VANTTEC_CAN_ID_DRIVE_MODE;
      buf[1] = 0x3;
      update_table(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_DRIVE_MODE, buf,
                   2);
    }
    osDelay(100);
  }
}

void reverse_task(void *args) {
  uint8_t buf[8];
  uint8_t reverse_data = 0;
  register_canlib_rx(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_REVERSE,
                     VANTTEC_CANLIB_BYTE, &reverse_data, 1);
  for (;;) {
    if (reverse_data == 1) {
      // Do something with steering

      // Return reverse signal back to 0
      buf[0] = VANTTEC_CAN_ID_REVERSE;
      buf[1] = 0x0;
      update_table(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_REVERSE, buf, 2);
    }

    osDelay(100);
  }
}

void pedal_brake_task(void *args) {
  uint8_t buf[8];
  uint8_t frenomanual_data = 0;
  register_canlib_rx(VANTTEC_CAN_ID_THROTTLE_TX, VANTTEC_CAN_ID_FRENO_MANUAL,
                     VANTTEC_CANLIB_BYTE, &frenomanual_data, 1);
  for (;;) {
    osDelay(100);
  }
}

void driverfault_task(void *args) {
  uint8_t buf[8];
  uint8_t driverfault_data = 0;
  uint8_t last_driverfault_data = 0;
  register_canlib_rx(VANTTEC_CAN_ID_STEPPER_TX, VANTTEC_CAN_ID_DRIVER_FAULT,
                     VANTTEC_CANLIB_BYTE, &driverfault_data, 1);
  for (;;) {

    if (driverfault_data == 1) {

      // Return reverse signal back to 0
      buf[0] = VANTTEC_CAN_ID_DRIVER_FAULT;
      buf[1] = 0x0;
      update_table(VANTTEC_CAN_ID_STEPPER_TX, VANTTEC_CAN_ID_DRIVER_FAULT, buf,
                   2);
    }
    osDelay(100);
  }
}

void init_requirements_task() {
    // TODO Fix these lol.
//   emergencystopTaskHandle =
//       osThreadNew(emergency_stop_task, NULL, &emergencystopTaskAttributes);
  hbTaskHandle = osThreadNew(hb_task, NULL, &hbTaskAttributes);
//   drivemodeTaskHandle =
//       osThreadNew(drive_mode_task, NULL, &driveModeTaskAttributes);
  // reverseTaskHandle = osThreadNew(reverse_task, NULL,
  // &reverseTaskAttributes); pedalBrakeTaskHandle =
  // osThreadNew(pedal_brake_task, NULL, &pedalBrakeTaskAttributes);
  // driverfaultTaskHandle = osThreadNew(driverfault_task, NULL,
  // &driverFaultTaskAttributes);
}
