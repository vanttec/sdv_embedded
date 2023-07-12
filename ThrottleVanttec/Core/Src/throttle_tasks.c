/*
* throttle_tasks.c
*
*  Created on: May 9, 2023
*      Author: Z0180064
*/
#include "main.h"
#include "vanttec_canlib_tx_task.h"
#include "vanttec_canlib_rx_task.h"
#include "stm32l4xx_hal.h"
#include "vanttec_canlib_generic_ids.h"

osThreadId_t potTaskHandle;
const osThreadAttr_t potTaskAttributes = {
    .name = "pot",
    .stack_size = 128 * 4};
osThreadId_t motorTaskHandle;
const osThreadAttr_t motorTaskAttributes = {
    .name = "motor",
    .stack_size = 128 * 4};
osThreadId_t modeTaskHandle;
const osThreadAttr_t modeTaskAttributes = {
    .name = "mode",
    .stack_size = 128 * 4};
osThreadId_t brakeTaskHandle;
const osThreadAttr_t brakeTaskAttributes = {
    .name = "brake",
    .stack_size = 128 * 4};
#define MAX_VELOCITY 150 //10km/h (255-35km/h)
void pot_task(void *args)
{
    uint8_t pot_data = 0;
    uint8_t last_pot_data = 0;
    register_canlib_rx(VANTTEC_CAN_ID_THROTTLE_RX, 0x05, VANTTEC_CANLIB_BYTE, &pot_data, 1);

    for (;;)
    {
        if (pot_data != last_pot_data)
        {
            if (pot_data > MAX_VELOCITY)
            {
                pot_data = MAX_VELOCITY;
            }
            else if(pot_data==0){
            	pot_data=1;
            }
            // Change potentiometer
            writeWiper(pot_data);
            last_pot_data = pot_data;
        }
        osDelay(10);
    }
}
void motor_task(void *args)
{
    uint8_t motor_data = 0;
    uint8_t last_motor_data = 0;
    register_canlib_rx(VANTTEC_CAN_ID_THROTTLE_RX, 0x06, VANTTEC_CANLIB_BYTE, &motor_data, 1);

    for (;;)
    {
        if (motor_data != last_motor_data)
        {
            if (motor_data == 1)
            {
                HAL_GPIO_WritePin(RelayMotor_GPIO_Port, RelayMotor_Pin, GPIO_PIN_SET);
            }
            else
            {
                HAL_GPIO_WritePin(RelayMotor_GPIO_Port, RelayMotor_Pin, GPIO_PIN_RESET);
            }
            last_motor_data = motor_data;
        }
        osDelay(10);
    }
}

void mode_task(void *args)
{
	uint8_t mode_data = 0;
    uint8_t last_mode_data = 0;
    register_canlib_rx(VANTTEC_CAN_ID_THROTTLE_RX, 0x07, VANTTEC_CANLIB_BYTE, &mode_data, 1);

    for (;;)
    {
        if (mode_data != last_mode_data)
        {
            if (mode_data == 1)
            {
                HAL_GPIO_WritePin(Pot_GPIO_Port, Pot_Pin, GPIO_PIN_SET);
            }
            else
            {
                HAL_GPIO_WritePin(Pot_GPIO_Port, Pot_Pin, GPIO_PIN_RESET);
            }
            last_mode_data = mode_data;
        }
        osDelay(10);
    }
}
void brake_task(void *args)
{
	GPIO_PinState brake_status;
	uint8_t buf[8];
	uint8_t prev_brake = 2;
	for(;;)
	{
		brake_status = HAL_GPIO_ReadPin(hand_brake_GPIO_Port, hand_brake_Pin);
		if(prev_brake!=brake_status){
			if (brake_status){
		            // Velocity to 0
		            buf[0] = 0x05;
		            buf[1] = 0x0;
		            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x05, buf, 2);
		            // Switch to manual pedal
		            buf[0] = 0x07;
		            buf[1] = 0x0;
		            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x07, buf, 2);
		            buf[0] = 0x06;
		            buf[1] = 0x0;
		            update_table(VANTTEC_CAN_ID_THROTTLE_RX, 0x06, buf, 2);
				}
				canlib_send_byte(VANTTEC_CAN_ID_FRENO_MANUAL, brake_status);
		}
		prev_brake = brake_status;
		osDelay(10);
	}

}

void init_throttle_tasks()
{
    potTaskHandle = osThreadNew(pot_task, NULL, &potTaskAttributes);
    motorTaskHandle = osThreadNew(motor_task, NULL, &motorTaskAttributes);
    modeTaskHandle = osThreadNew(mode_task, NULL, &modeTaskAttributes);
    brakeTaskHandle = osThreadNew(brake_task, NULL, &brakeTaskAttributes);
}
