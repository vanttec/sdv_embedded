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

uint8_t drive_mode = 0U;		//0: Manual, 1: Controller/Autonomous
uint8_t em_stop = 0U;

void steering_task()
{
	uint8_t init = 0;
	uint8_t dir = IDLE;		// For safety do not modify this initial value
	uint32_t pos = 0U;
	register_canlib_rx(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_STEERING, VANTTEC_CANLIB_BYTE, &dir, 1);		// For direction
	register_canlib_rx(0x52, 0x11, VANTTEC_CANLIB_LONG, &pos, 4);		// To check IFM encoder angle

	configure_steering();

	for(;;)
	{
		if(!em_stop)
		{
			if(drive_mode)
			{
				if(!init)
				{
					HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_SET);
					start(STEERING);
					init = 1;
				}
				parse_ifm_encoder(pos);
				update_stepper_pos(STEERING);
				steer(dir);
			} else {
				HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_RESET);
				stop(STEERING);
				init = 0;
			}
			HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_RESET);
		} else {
			HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_SET);
			stop(STEERING);
			init = 0;
		}
	}
}

void braking_task()
{
	uint8_t init = 0;
	uint32_t pos = 0U;
	float desired_pos = 0;
	register_canlib_rx(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_BRAKING, VANTTEC_CANLIB_FLOAT, &desired_pos, 4);
	register_canlib_rx(0x53, 0x11, VANTTEC_CANLIB_LONG, &pos, 4);		// To check briter encoder angle

	for(;;)
	{
		if(!em_stop){
			if(drive_mode)
			{
				if(!init)
				{
					//HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_SET);
					start(BRAKING);
					init = 1;
				}
				parse_briter_encoder(pos);
				update_stepper_pos(BRAKING);
				set_setpoint(BRAKING, desired_pos);

			} else {
				//HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_RESET);
				stop(BRAKING);
				init = 0;
			}
		} else {
			HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_SET);
			stop(BRAKING);
			init = 0;
		}
	}
}

void gpios_task()
{
	GPIO_PinState steering_fault_state = 0U;
	GPIO_PinState braking_fault_state = 0U;
	GPIO_PinState brake_input = 0U;

	for(;;)
	{
		steering_fault_state = HAL_GPIO_ReadPin(GPIOC, STPR_FLT_1_Pin);
		braking_fault_state = HAL_GPIO_ReadPin(GPIOB, STPR_FLT_2_Pin);
		brake_input = HAL_GPIO_ReadPin(GPIOB, BRAKE_IN_Pin);

		if(steering_fault_state || braking_fault_state)
		{
			HAL_GPIO_WritePin(DEBUG_3_GPIO_Port, DEBUG_3_Pin, GPIO_PIN_SET);
	        canlib_send_byte(VANTTEC_CAN_ID_DRIVER_FAULT, (uint8_t) 0);
	        canlib_send_byte(VANTTEC_CAN_ID_ESTOP, (uint8_t) 0);
			em_stop = 1;
		} else {
			HAL_GPIO_WritePin(DEBUG_3_GPIO_Port, DEBUG_3_Pin, GPIO_PIN_RESET);
		}

		if(brake_input)
		{
			HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_SET);
	        canlib_send_byte(VANTTEC_CAN_ID_PEDAL_BRAKE, (uint8_t) 0);
	        canlib_send_byte(VANTTEC_CAN_ID_ESTOP, (uint8_t) 0);
			//em_stop = 1;
		} else {
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
	register_canlib_rx(VANTTEC_CAN_ID_JETSON, VANTTEC_CAN_ID_DRIVE_MODE, VANTTEC_CANLIB_BYTE, &drive_mode, 1);
	register_canlib_rx(VANTTEC_CAN_ID_JETSON, VANTTEC_CAN_ID_ESTOP, VANTTEC_CANLIB_BYTE, &em_stop, 1);
	init_steer_task();
	//init_brake_task();
	init_gpios_task();
}
