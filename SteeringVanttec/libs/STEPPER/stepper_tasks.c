/*
 * stepper_tasks.c
 *
 *  Created on: May 11, 2023
 *      Author: riky_
 */

#include "main.h"
#include "vanttec_canlib_tx_task.h"
#include "vanttec_canlib_rx_task.h"
#include "stepper_tasks.h"
#include "stepper.h"

osThreadId_t steerTaskHandle;
const osThreadAttr_t steeringTaskAttributes = {
    .name = "steering",
    .stack_size = 128 * 4,
	/*.priority = (osPriority_t) osPriorityAboveNormal4*/};
osThreadId_t brakingTaskHandle;
const osThreadAttr_t brakingTaskAttributes = {
    .name = "steering",
    .stack_size = 128 * 4};

void steering_task()
{
	//uint16_t frame = 0U;
	uint8_t dir = 0U;
	uint32_t pos = 0U;
	register_canlib_rx(0x51, 0x10, VANTTEC_CANLIB_BYTE, &dir, 1);		// For direction
	register_canlib_rx(0x52, 0x11, VANTTEC_CANLIB_LONG, &pos, 4);		// To check IFM encoder angle

	configure_steering();
	start(STEERING);

	for(;;)
	{
		parse_ifm_encoder(pos);
		update_stepper_pos(STEERING);
		set_direction(dir);
	}
}

void braking_task()
{
	/*
	float desired_pos = 0;
	uint32_t encoder_pos = 0U;
	register_canlib_rx(0x53, 0x12, VANTTEC_CANLIB_FLOAT, &desired_pos, 1);		// For desired position
	//register_canlib_rx(0x54, 0x13, VANTTEC_CANLIB_LONG, &encoder_pos, 4);		// To check briter encoder angle

	for(;;)
	{
		set_setpoint(BRAKING, desired_pos);
		update_stepper_pos(BRAKING);
		parse_briter_encoder(encoder_pos);
		brake_by_dir();
	}
	*/
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
