#include <stdint.h>
#include "stepper.h"

volatile stepper braking_stepper;
volatile stepper steering_stepper;

void configure_steppers()
{
	steering_stepper.is_active = 0;
	steering_stepper.is_exec_started = 0;
	steering_stepper.mode = CONTROLLER;
	steering_stepper.direction = IDLE;
	steering_stepper.MAX_ANGLE = 57;
	steering_stepper.current_angle = 0;
	/*
	steering_stepper.STEP_ANGLE = 0.9;
	steering_stepper.MAX_STEPS = 63;		// Corresponding to the 57 deg
	steering_stepper.STEPS_REV = 400;
	*/

	braking_stepper.is_active = 0;
	braking_stepper.is_exec_started = 0;
	braking_stepper.mode = CONTROLLER;
	braking_stepper.direction = IDLE;
	braking_stepper.MAX_ANGLE = 57;
	braking_stepper.current_angle = 0;

	//htim2.Instance->CCR1 = 5000;	// For duty cycle of 50%
	htim2.Instance->CCR1 = 500;	// For duty cycle of 50%
}

void start()
{
	HAL_GPIO_WritePin(GPIOC, STPR_EN_1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, STPR_EN_2_Pin | LVL_SFTR_OE_2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LVL_SFTR_OE_1_GPIO_Port, LVL_SFTR_OE_1_Pin, GPIO_PIN_SET);
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_4);
	steering_stepper.is_active = 1;
	braking_stepper.is_active = 1;
}

void pause(stepper_type stepper)
{
	switch(stepper)
	{
		case STEERING:
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
			steering_stepper.is_exec_started = 0;
			break;
		case BRAKING:
			HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_4);
			braking_stepper.is_exec_started = 0;
			break;
		default:
			break;
	}
}

void stop()
{
	HAL_GPIO_WritePin(GPIOC, STPR_EN_1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, STPR_EN_2_Pin | LVL_SFTR_OE_2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LVL_SFTR_OE_1_GPIO_Port, LVL_SFTR_OE_1_Pin, GPIO_PIN_RESET);
	HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_4);
	steering_stepper.is_active = 0;
	braking_stepper.is_active = 0;
	steering_stepper.direction = IDLE;
	braking_stepper.direction = IDLE;
	steering_stepper.is_exec_started = 0;
	braking_stepper.is_exec_started = 0;
}

void set_direction(const stepper_type stepper, uint8_t direction){
	switch(stepper)
	{
		case STEERING:
			if(direction != steering_stepper.direction)
			{
				steering_stepper.direction = direction;
				HAL_GPIO_WritePin(GPIOC, STPR_DIR_1_Pin, direction);
			}
			//steering_stepper.mode = CONTROLLER;
			break;
		case BRAKING:
			if(direction != braking_stepper.direction)
			{
				braking_stepper.direction = direction;
				HAL_GPIO_WritePin(GPIOC, STPR_DIR_2_Pin, direction);
			}
			//braking_stepper.mode = CONTROLLER;
			break;
		default:
			break;
	}
}

/*
void set_setpoint(const stepper_type stepper, uint16_t setpoint, int8_t direction){
	switch(stepper)
	{
		case STEERING:
			if(steering_stepper.req_steps != setpoint)
			{
				steering_stepper.req_steps = setpoint;
				steering_stepper.current_step = 0;
			}
			if(steering_stepper.direction != direction)
			{
				steering_stepper.direction = direction;
				HAL_GPIO_WritePin(GPIOC, STPR_DIR_1_Pin, direction);
				steering_stepper.current_step = 0;
			}
			break;
		case BRAKING:
			if(braking_stepper.req_steps != setpoint)
			{
				braking_stepper.req_steps = setpoint;
				braking_stepper.current_step = 0;
			}
			if(braking_stepper.direction != direction)
			{
				braking_stepper.direction = direction;
				HAL_GPIO_WritePin(GPIOC, STPR_DIR_2_Pin, direction);
				braking_stepper.current_step = 0;
			}
			break;
		default:
			break;
	}
}
*/

void steer()
{
	if(steering_stepper.is_active && steering_stepper.direction != IDLE)
	{
		// Check wheel angle for safety
		if(ifm_encoder.absolute_angle < steering_stepper.MAX_ANGLE)
		{
			// Start execution
			if(!steering_stepper.is_exec_started)
			{
				HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
				steering_stepper.is_exec_started = 1;
			}
		} else pause(STEERING);

	} else pause(STEERING);
}

void brake()
{}


void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2) {
	//steering_stepper.current_step++;
	//braking_stepper.current_step++;
	//HAL_GPIO_TogglePin(GPIOA, DEBUG_2_Pin);
  }
}
