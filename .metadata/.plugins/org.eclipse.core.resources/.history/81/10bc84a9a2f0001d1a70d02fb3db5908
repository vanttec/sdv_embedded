#include <stdint.h>
#include <STEPPER/stepper.h>
#include "CAN/can_bus.h"


volatile stepper braking_stepper;
volatile stepper steering_stepper;

void configure_steppers()
{
	/*
	steering_stepper.MAX_ANGLE = 57;
	steering_stepper.STEP_ANGLE = 0.9;
	steering_stepper.MAX_STEPS = 63;		// Corresponding to the 57 deg
	steering_stepper.STEPS_REV = 400;
	*/
	steering_stepper.current_step = 0;
	steering_stepper.active = 0;
	steering_stepper.exec_started = 0;

	/*
	braking_stepper.MAX_ANGLE = 1;			// TBD
	braking_stepper.STEP_ANGLE = 1.8;		// TBD
	steering_stepper.MAX_STEPS = 63;		// TBD
	braking_stepper.STEPS_REV = 200;		// TBD
	*/
	braking_stepper.current_step = 0;
	braking_stepper.active = 0;
	braking_stepper.exec_started = 0;

	//htim2.Instance->CCR1 = 5000;	// For duty cycle of 50%
	htim2.Instance->CCR1 = 500;	// For duty cycle of 50%
	//HAL_GPIO_WritePin(GPIOA, DEBUG_2_Pin|DEBUG_1_Pin|STPR_PWM_1_Pin, GPIO_PIN_SET);

}

void start()
{
	HAL_GPIO_WritePin(GPIOC, STPR_EN_1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, STPR_EN_2_Pin | LVL_SFTR_OE_2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LVL_SFTR_OE_1_GPIO_Port, LVL_SFTR_OE_1_Pin, GPIO_PIN_SET);
	HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
	steering_stepper.active = 1;
	braking_stepper.active = 1;
	steering_stepper.current_step = 0;
	braking_stepper.current_step = 0;
}

void stop()
{
	HAL_GPIO_WritePin(GPIOC, STPR_EN_1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, STPR_EN_2_Pin | LVL_SFTR_OE_2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LVL_SFTR_OE_1_GPIO_Port, LVL_SFTR_OE_1_Pin, GPIO_PIN_RESET);
	HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
	steering_stepper.active = 0;
	braking_stepper.active = 0;
	steering_stepper.direction = IDLE;
	braking_stepper.direction = IDLE;
}

void set_direction(const stepper_id stepper, uint8_t direction){
	switch(stepper)
	{
		case STEERING:
			steering_stepper.direction = direction;
			if(direction != steering_stepper.direction)
				HAL_GPIO_WritePin(GPIOC, STPR_DIR_1_Pin, direction);
			steering_stepper.mode = CONTROLLER;
			break;
		case BRAKING:
			braking_stepper.direction = direction;
			if(direction != braking_stepper.direction)
				HAL_GPIO_WritePin(GPIOC, STPR_DIR_2_Pin, direction);
			braking_stepper.mode = CONTROLLER;
			break;
		default:
			break;
	}
}

void set_setpoint(const stepper_id stepper, uint16_t setpoint, int8_t direction){
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

/*
void set_setpoint(const stepper_id stepper){
	if(stepper == STEERING)
		{
			//steering_stepper.desired_angle = setpoint;
			if(setpoint > steering_stepper.MAX_STEPS)
				setpoint = steering_stepper.MAX_STEPS;
			steering_stepper.req_steps = setpoint;
			steering_stepper.direction = direction;
			steering_stepper.current_step = 0;
			HAL_GPIO_WritePin(GPIOC, STPR_DIR_1_Pin, direction);
		} else {
			//braking_stepper.desired_angle = setpoint;
			if(setpoint > braking_stepper.MAX_STEPS)
				setpoint = braking_stepper.MAX_STEPS;
			braking_stepper.req_steps = setpoint;
			braking_stepper.direction = direction;
			braking_stepper.current_step = 0;
			HAL_GPIO_WritePin(GPIOB, STPR_DIR_2_Pin, direction);
		}
}
*/

void steer()
{
	if(steering_stepper.active == 1)
		if(steering_stepper.mode == CONTROLLER)			// XBOX CONTROLLER
		{
			if(steering_stepper.direction != IDLE)
			{
				if(steering_stepper.exec_started == 0)
				{
					HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
					steering_stepper.exec_started = 1;
				}
			}
			else {
				HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
				steering_stepper.exec_started = 0;
			}
		}

		/*
		else										// AUTONOMOUS MODE
			if(steering_stepper.req_steps <= steering_stepper.current_step)
				HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
		*/
}

void brake()
{
	//if(braking_stepper.desired_angle < can_rx_data.encoderAngle[1])
	set_setpoint(BRAKING, can_rx_data.motor_2_steps, can_rx_data.motor_2_direction);
	if(braking_stepper.current_step == 0)
		HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_4);
	if(braking_stepper.req_steps <= braking_stepper.current_step)
		HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_4);
}


void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2) {
	//steering_stepper.current_step++;
	//braking_stepper.current_step++;
	//HAL_GPIO_TogglePin(GPIOA, DEBUG_2_Pin);
  }
}

/*
void stepping_by_pwm(stepper *stpr, stepper_id id)
{
	//stpr->desired_angle = 45;	//can_rx_data.encoderAngle[0];

	int angle_d = (int) stpr->desired_angle;
	stpr->direction = abs(angle_d)/angle_d;

	if(fabs(stpr->desired_angle) > stpr->MAX_ANGLE)
		stpr->desired_angle = stpr->MAX_ANGLE;

	stpr->desired_angle *= stpr->direction; // to work only with positive numbers

	if(id == STEERING)
	{
	HAL_GPIO_WritePin(GPIOC, STPR_DIR_1_Pin, stpr->direction);
	//if(stpr->desired_angle < can_rx_data.encoderAngle[0])
	if(stpr->req_steps < stpr->current_step)
		HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_1);
	else
		HAL_TIM_PWM_Stop_IT(&htim1, TIM_CHANNEL_1);
	}
	else
	{
	HAL_GPIO_WritePin(GPIOB, STPR_DIR_2_Pin, stpr->direction);
	//if(stpr->desired_angle < can_rx_data.encoderAngle[1])
	if(stpr->req_steps < stpr->current_step)
		HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_1);
	else
		HAL_TIM_PWM_Stop_IT(&htim1, TIM_CHANNEL_1);
	}
}
void stepping_by_steps(stepper *stpr, stepper_id id)
{
	stpr->desired_angle = 45;	//can_rx_data.encoderAngle[0];
	int angle_d = (int) stpr->desired_angle;
	stpr->direction = abs(angle_d)/angle_d;

	if(fabs(stpr->desired_angle) > stpr->MAX_ANGLE)
		stpr->desired_angle = stpr->MAX_ANGLE;

	stpr->desired_angle *= stpr->direction; // to work only with positive numbers

	stpr->req_steps = stpr->desired_angle/stpr->STEP_ANGLE;
}
*/
