/*
 * steering_tasks.c
 *
 *  Created on: May 11, 2023
 *      Author: riky_
 */
#include "main.h"
#include "vanttec_canlib_tx_task.h"
#include "vanttec_canlib_rx_task.h"
#include "STEPPER/stepper.h"

osThreadId_t steeringTaskHandle;
const osThreadAttr_t steeringTaskAttributes = {
    .name = "steering",
    .stack_size = 128 * 4};
osThreadId_t brakingTaskHandle;
const osThreadAttr_t brakingTaskAttributes = {
    .name = "steering",
    .stack_size = 128 * 4};
void steering_task(void *argument)
{
  configure_steppers();
  start();
  for(;;)
  {
	  steer();
	  osDelay(1);
  }
}
void braking_task(void *argument)
{
  for(;;)
  {
    osDelay(1);
  }
}
void receiveID(void *argument)
{
    uint8_t id;
    register_canlib_rx(0x51, 0x05, VANTTEC_CANLIB_BYTE, &id, 1);
    for (;;)
    {
//    	if(id == STEPPERS_STATE){
//    		// Frame byte structure: ID X X X STATE
//    		//uint32_t frame = can_parse_long(data, header->DLC);
//    		uint8_t state = frame & 0x000000FF;
//    		if(state == 1) // Start
//    			start();
//    		else
//    			stop();
//    	} else if(id == AUTONOMOUS_STEERING){
//    		// Frame byte structure: ID #STEPS1 #STEPS2 X DIR
//    		uint32_t frame = can_parse_long(data, header->DLC);
//    		uint16_t steps = (frame & 0xFFFF0000) >> 16;
//    		uint8_t dir = frame & 0x000000FF;
//    		set_setpoint(STEERING, steps, dir);
//    	} else if(id == CONTROLLER_STEERING){
//    		// Frame byte structure: ID X X X DIR
//    		uint32_t frame = can_parse_long(data, header->DLC);
//    		//uint16_t steps = (frame & 0xFFFF0000) >> 16;
//    		uint8_t dir = frame & 0x000000FF;
//    		set_direction(STEERING, dir);
//    	} else if(id == ENCODER_ID_IFM){
//    		//?
//    	} else if(id == ENCODER_ID_BRITTER) {
//    		float val = can_parse_float(data, header->DLC);
//    		float angle = val/4096*360;
//    		//can_rx_data.encoderAngle[1] = angle;
//    	}
		osDelay(1);
	}
}
void init_steering_task()
{
	steeringTaskHandle = osThreadNew(steering_task, NULL, &steeringTaskAttributes);
	brakingTaskHandle = osThreadNew(braking_task, NULL, &brakingTaskAttributes);
	//receiveIDTaskHandle = osThreadNew(receiveID_task, NULL, &receiveIDTaskAttributes);

}
