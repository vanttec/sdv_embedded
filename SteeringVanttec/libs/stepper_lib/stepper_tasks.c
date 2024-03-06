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
5: braking or steering ON/OFF
6: braking or steering pause/unpause

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
osThreadId_t xboxTaskHandle;
const osThreadAttr_t xboxTaskAttributes = {
    .name = "xbox",
    .stack_size = 128 * 4};

uint8_t drive_mode = 0U; // 0: Manual, 1: Controller/Autonomous
// uint8_t em_stop = 0U;
float desired_pos = 0.0f;

void steering_task()
{
	uint8_t es_steering = 0U;
	uint8_t drivemode_data = 0U;
	uint8_t xbox_mode_data = 0U;
	uint8_t init = 0U;
//	uint8_t es_steering = 0;   		//Debug
//	uint8_t drivemode_data = 1;		//Debug
//	uint8_t xbox_mode_data = 1;		//Debug
//	uint8_t init = 0;				//Debug
	uint8_t dir = IDLE; // For safety do not modify this initial value

	uint32_t encoder_pos = 0U;
	register_canlib_rx(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_STEERING_XBOX, VANTTEC_CANLIB_BYTE, &dir, 1); // For direction
	register_canlib_rx(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_STEERING, VANTTEC_CANLIB_FLOAT, &desired_pos, 4);
	register_canlib_rx(0x52, 0x11, VANTTEC_CANLIB_LONG, &encoder_pos, 4);												// To check IFM encoder angle
	register_canlib_rx(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_ES_STEERING, VANTTEC_CANLIB_BYTE, &es_steering, 1);	// For EM braking
	register_canlib_rx(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_DR_STEERING, VANTTEC_CANLIB_BYTE, &drivemode_data, 1); // For enable braking
	register_canlib_rx(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_EN_XBOX, VANTTEC_CANLIB_BYTE, &xbox_mode_data, 1);		// For xbox information
	//register_canlib_rx(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_XBOX, VANTTEC_CANLIB_BYTE, &xbox_mode_data, 1);		// For xbox information

	configure_steering();

	for (;;)
	{
		parse_ifm_encoder(encoder_pos);
		update_stepper_pos(STEERING);
		if (es_steering)
		{
			//HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_RESET);
			stop(STEERING);
			init = 0;
		}
		else
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

				// Autonomous mode
				if (xbox_mode_data)
				{
					// Steer by setpoint
//					desired_pos=0.3;		//Debug
					set_setpoint(STEERING, desired_pos);
				}
				else
				{
					// Steer by Xbox Joystick
					steer(dir);
					desired_pos = get_current_pos();
				}
				//HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_RESET);
				// Manual Mode
				//pause(STEERING);
				stop(STEERING);
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
	float desired_pos_brake = 0;
	register_canlib_rx(VANTTEC_CAN_ID_BRAKING_RX, VANTTEC_CAN_ID_ES_BRAKING, VANTTEC_CANLIB_BYTE, &es_braking, 1);	   // For EM braking
	register_canlib_rx(VANTTEC_CAN_ID_BRAKING_RX, VANTTEC_CAN_ID_DR_BRAKING, VANTTEC_CANLIB_BYTE, &drivemode_data, 1); // For enable braking
	register_canlib_rx(VANTTEC_CAN_ID_BRAKING_RX, VANTTEC_CAN_ID_BRAKING, VANTTEC_CANLIB_FLOAT, &desired_pos_brake, 4);
	register_canlib_rx(0x53, 0x11, VANTTEC_CANLIB_LONG, &pos, 4); // To check IFM encoder angle
	configure_braking();
	for (;;)
	{

		parse_briter_encoder(pos);
		update_stepper_pos(BRAKING);
		if (es_braking)
		{
			//HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_RESET);
			// em_stop(BRAKING);
			init = 0;
		}
		else
		{

			// Analyze drive mode
			if (drivemode_data)
			{
				// Start if not initialized
				if (!init)
				{
					HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_SET);
					start(BRAKING);
					init = 1;
				}
				// Auto mode
				set_setpoint(BRAKING, desired_pos_brake);
				//HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(DEBUG_2_GPIO_Port, DEBUG_2_Pin, GPIO_PIN_RESET);
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
			canlib_send_byte(VANTTEC_CAN_ID_DRIVER_FAULT, (uint8_t)0);
			canlib_send_byte(VANTTEC_CAN_ID_ESTOP, (uint8_t)0);
			// em_stop = 1;
		}
		else
		{
			HAL_GPIO_WritePin(DEBUG_3_GPIO_Port, DEBUG_3_Pin, GPIO_PIN_RESET);
		}

		if (brake_input)
		{
//			HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_SET);
			canlib_send_byte(VANTTEC_CAN_ID_PEDAL_BRAKE, (uint8_t)0);
			canlib_send_byte(VANTTEC_CAN_ID_ESTOP, (uint8_t)0);
			// em_stop = 1;
		}
		else
		{
//			HAL_GPIO_WritePin(DEBUG_4_GPIO_Port, DEBUG_4_Pin, GPIO_PIN_RESET);
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
            //Activate steer
            buf[0] = VANTTEC_CAN_ID_EN_XBOX;
            buf[1] = 0x1;
            update_table(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_EN_XBOX, buf, 2);

            ///desired_pos = get_current_angle();

            /*
            uint32_t current_angle = obtain_current_angle();
            buf[0] = VANTTEC_CAN_ID_STEERING;
			buf[1] = (current_angle & 0xFF) >> (8);
			buf[2] = (current_angle & 0x0000FF00) >> 8;
			buf[3] = (current_angle & 0x00FF0000) >> 8*2;
            buf[4] = (current_angle &  0xFF000000) >> (8*3);

            update_table(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_STEERING, buf, 5);

			*/
            buf[0] = VANTTEC_CAN_ID_XBOX;
            buf[1] = 0x3;
            update_table(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_XBOX, buf, 2);

        }
        else if (xbox_data==0){

            //Continue with setpoint control
            buf[0] = VANTTEC_CAN_ID_EN_XBOX;
            buf[1] = 0x0;
            update_table(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_EN_XBOX, buf, 2);

            //desired_pos = get_current_angle();

            /*
            uint32_t current_angle = obtain_current_angle();
            buf[0] = VANTTEC_CAN_ID_STEERING;
            buf[1] = (current_angle & 0xFF) >> (8);
			buf[2] = (current_angle & 0x0000FF00) >> 8;
			buf[3] = (current_angle & 0x00FF0000) >> 8*2;
			buf[4] = (current_angle &  0xFF000000) >> (8*3);

			update_table(VANTTEC_CAN_ID_STEPPER_RX, VANTTEC_CAN_ID_STEERING, buf, 5);
			*/

            buf[0] = VANTTEC_CAN_ID_XBOX;
            buf[1] = 0x3;
            update_table(VANTTEC_CAN_ID_GENERAL_TX, VANTTEC_CAN_ID_XBOX, buf, 2);
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

void init_xbox_task(){
    xboxTaskHandle = osThreadNew(xbox_task, NULL, &xboxTaskAttributes);
}

void init_stepper_tasks()
{
	// register_canlib_rx(VANTTEC_CAN_ID_GENERAL_RX, VANTTEC_CAN_ID_DRIVE_MODE, VANTTEC_CANLIB_BYTE, &drive_mode, 1);
	// register_canlib_rx(1, VANTTEC_CAN_ID_ESTOP, VANTTEC_CANLIB_BYTE, &em_stop, 1);
	init_steer_task();
	init_xbox_task();
	// init_brake_task();
	init_gpios_task();
}
