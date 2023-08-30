/*
 * stepper_tasks.c
 *
 *  Created on: May 11, 2023
 *      Author: vSebas
 */

#include "main.h"
#include "vanttec_canlib_generic_ids.h"
#include "vanttec_canlib_tx_task.h"
#include "vanttec_canlib_rx_task.h"
#include "vanttec_sdv_ids.h"
#include "stepper_tasks.h"
#include "stepper.h"

/*
 * LEDs

1: heartbit
2: Drive Mode: 0: manual
3: Stepper Fault
4: brake/em stop
5: braking ON/OFF/pause
6: steering ON/OFF/pause

 * */

osThreadId_t steerTaskHandle;
const osThreadAttr_t steeringTaskAttributes = {
	.name = "steering",
	.stack_size = 128 * 4
	/*.priority = (osPriority_t) osPriorityAboveNormal4*/};
osThreadId_t brakingTaskHandle;
const osThreadAttr_t brakingTaskAttributes = {
	.name = "braking",
	.stack_size = 128 * 4};
const osThreadAttr_t GPIOsTaskAttributes = {
	.name = "gpios_monitoring",
	.stack_size = 128 * 4};

uint8_t steering = 0;
void steering_task()
{
	uint8_t es_steering = 0;
	uint8_t drivemode_data = 0;
	uint8_t init = 0;
	uint8_t dir = IDLE; // For safety do not modify this initial value
	uint32_t pos = 0U;
	register_canlib_rx(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_STEERING, VANTTEC_CANLIB_BYTE, &dir, 1);				// For direction
	register_canlib_rx(0x52, 0x11, VANTTEC_CANLIB_LONG, &pos, 4);														// To check IFM encoder angle
	register_canlib_rx(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_ES_STEERING, VANTTEC_CANLIB_BYTE, &es_steering, 1);	// For EM braking
	register_canlib_rx(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_DR_STEERING, VANTTEC_CANLIB_BYTE, &drivemode_data, 1); // For enable braking
	configure_steering();

	for (;;)
	{
		parse_ifm_encoder(pos);
		update_stepper_pos(STEERING);
		if (es_steering)
		{
			HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_SET);
			stop(STEERING);
			init = 0;
		}
		{

			// Analyze drive mode
			if (drivemode_data)
			{
				// Start if not initialized
				if (!init)
				{
					HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_SET);
					start(STEERING);
					init = 1;
				}
				// Auto mode
				steer(dir);
				HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_RESET);
				// Manual Mode
				pause(STEERING);
				init = 0;
			}
		}
		osDelay(10);
	}
}

void braking_task()
{
	uint8_t es_braking = 0;
	uint8_t drivemode_data = 0;
	uint8_t init = 0;
	uint32_t pos = 0U;
	float desired_pos = 0;
	int test = 0;
	register_canlib_rx(VANTTEC_CAN_ID_BRAKING_RX, VANTTEC_CAN_ID_ES_BRAKING, VANTTEC_CANLIB_BYTE, &es_braking, 1);	   // For EM braking
	register_canlib_rx(0x53, 0x13, VANTTEC_CANLIB_LONG, &pos, 4); // To check IFM encoder angle check angle and the way it dd  dd
	register_canlib_rx(VANTTEC_CAN_ID_BRAKING_RX, VANTTEC_CAN_ID_DR_BRAKING, VANTTEC_CANLIB_BYTE, &drivemode_data, 1); // For enable braking
	register_canlib_rx(VANTTEC_CAN_ID_BRAKING_RX, VANTTEC_CAN_ID_BRAKING, VANTTEC_CANLIB_FLOAT, &desired_pos, 4);
	configure_braking(); //no

	int pos_temp = 0;

	for (;;)
	{
		if(pos != 0){ //NO EXCEDER DE 180 EN DESIRED POS POR AHORA!!!!!
			int test = 1;
		}
		parse_briter_encoder(pos);
		update_stepper_pos(BRAKING);
		if (es_braking)
		{
			HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_RESET);
			em_stop(BRAKING);
			init = 0;
		}
		else
		{
			// Analyze drive mode
			if (drivemode_data)
			{
			test = 1;
			 	// Start if not initialized
				if (!init)
				{
					start(BRAKING);
					init = 1;
				}
				if(desired_pos != 0){
					test = 2;
				}


				if ( ! ( fabs(pos_temp - pos) >= 0.2) ) {
					// Auto mode
					set_setpoint(BRAKING, desired_pos);
					HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_SET);
				}

				pos_temp = pos;

			}
			else
			{
				HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_RESET);
				// Manual Mode
				pause(BRAKING);
				init = 0;
			}
		}
		osDelay(10);
	}
}

void gpios_task()
{
	GPIO_PinState steering_fault_state = 0U;
	GPIO_PinState braking_fault_state = 0U;
	GPIO_PinState brake_input = 0U;

	for (;;)
	{
		steering_fault_state = HAL_GPIO_ReadPin(GPIOC, STPR_FLT_1_Pin);
		braking_fault_state = HAL_GPIO_ReadPin(GPIOB, STPR_FLT_2_Pin);
		brake_input = HAL_GPIO_ReadPin(GPIOB, BRAKE_IN_Pin);

		if (steering_fault_state || braking_fault_state)
		{
			HAL_GPIO_WritePin(DEBUG_3_GPIO_Port, DEBUG_3_Pin, GPIO_PIN_SET);
			canlib_send_byte(VANTTEC_CAN_ID_DRIVER_FAULT, (uint8_t)1);
			canlib_send_byte(VANTTEC_CAN_ID_ESTOP, (uint8_t)1);
			// braking = 0;
			steering = 0;
		}
		else
		{
			HAL_GPIO_WritePin(DEBUG_3_GPIO_Port, DEBUG_3_Pin, GPIO_PIN_RESET);
		}

		if (brake_input)
		{
			HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_SET);
			canlib_send_byte(VANTTEC_CAN_ID_PEDAL_BRAKE, (uint8_t)1);
			canlib_send_byte(VANTTEC_CAN_ID_ESTOP, (uint8_t)1);
			// braking = 0;
			steering = 0;
		}
		else
		{
			HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_RESET);
		}

		osDelay(10);
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

void init_gpios_task()
{
	brakingTaskHandle = osThreadNew(gpios_task, NULL, &GPIOsTaskAttributes);
}

void init_stepper_tasks()
{
	// init_steer_task();
	init_brake_task();
	// init_gpios_task();
}
