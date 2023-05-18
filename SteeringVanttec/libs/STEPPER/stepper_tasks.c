/*
 * stepper_tasks.c
 *
 *  Created on: May 11, 2023
 *      Author: riky_
 */

#include "main.h"
#include "vanttec_canlib_tx_task.h"
#include "vanttec_canlib_rx_task.h"
#include "STEPPER/stepper_tasks.h"
#include "STEPPER/stepper.h"

osThreadId_t steerTaskHandle;
const osThreadAttr_t steeringTaskAttributes = {
    .name = "steering",
    .stack_size = 128 * 4};
osThreadId_t brakingTaskHandle;
const osThreadAttr_t brakingTaskAttributes = {
    .name = "steering",
    .stack_size = 128 * 4};

void steering_task()
{
	uint16_t frame = 0U;
	uint8_t dir = 0U;
	register_canlib_rx(0x51, 0x10, VANTTEC_CANLIB_SHORT, &frame, 2);
	configure_steppers();
	start();
	for(;;)
	{
		//	Msg byte structure: ID DIR
		dir = frame & 0x00FF;
		set_direction(STEERING, dir);
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
	brakingTaskHandle = osThreadNew(braking_task, NULL, &brakingTaskAttributes)
}

void init_stepper_tasks()
{
	init_steer_task();
	init_brake_task();
}
