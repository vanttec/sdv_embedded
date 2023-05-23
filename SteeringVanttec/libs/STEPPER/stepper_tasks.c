/*
 * stepper_tasks.c
 *
 *  Created on: May 11, 2023
 *      Author: riky_
 */

#include "main.h"
#include "vanttec_canlib_tx_task.h"
#include "vanttec_canlib_rx_task.h"
#include "vanttec_canlib_generic_ids.h"
#include "stepper_tasks.h"
#include "stepper.h"
#include "encoder.h"

osThreadId_t steerTaskHandle;
const osThreadAttr_t steeringTaskAttributes = {
    .name = "steering",
    .stack_size = 128 * 4};
osThreadId_t brakingTaskHandle;
const osThreadAttr_t brakingTaskAttributes = {
    .name = "braking",
    .stack_size = 128 * 4};

void steering_task()
{
	//uint16_t frame = 0U;
	uint8_t dir = 0U;
	uint32_t pos = 0U;
	register_canlib_rx(VANTTEC_CAN_ID_STEPPER_RX, 0x10, VANTTEC_CANLIB_BYTE, &dir, 1);		// For direction
	register_canlib_rx(VANTTEC_CAN_ID_STEPPER_RX, 0x11, VANTTEC_CANLIB_LONG, &pos, 2);		// To check IFM encoder angle

	configure_steppers();
	start();

	for(;;)
	{
		//dir = frame & 0x00FF;
		set_direction(STEERING, dir);
		parse_ifm_encoder(pos);
		steer();
	}
}

void braking_task()
{
  for(;;)
  {
    osDelay(1);
  }
}

void init_steer_task()
{
	steerTaskHandle = osThreadNew(steering_task, NULL, &steeringTaskAttributes);
}

void init_brake_task()
{
	brakingTaskHandle = osThreadNew(braking_task, NULL, &brakingTaskAttributes);
}

void init_stepper_tasks()
{
	init_steer_task();
	init_brake_task();
}
