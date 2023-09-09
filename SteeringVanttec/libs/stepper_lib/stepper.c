#include <stdint.h>
#include "math.h"
#include "stepper.h"
//#include "Utils/CANSerialization.h"

volatile stepper braking_stepper;
volatile stepper steering_stepper;

const static float STEER_RATIO = 1.5; // Stepper to steering wheel ratio
const static uint16_t MAX_STEERING_ANGLE = 600;
const static uint16_t MIN_STEERING_ANGLE = 400;
const static uint16_t PEDAL_LENGTH = 0.18;
const static uint16_t PULLEY_RADIUS = 0.0353;

void configure_steering()
{

	steering_stepper.is_active = 0;
	steering_stepper.is_exec_started = 0;
	steering_stepper.direction = IDLE;
	steering_stepper.MAX_ANGLE = MAX_STEERING_ANGLE*STEER_RATIO;	//Degrees
	steering_stepper.MIN_ANGLE = MIN_STEERING_ANGLE*STEER_RATIO;	//Degrees
	steering_stepper.current_angle = 0;
	steering_stepper.STEPPER_OFFSET = 10;//1.8;

	htim2.Instance->CCR1 = 2500;	// For duty cycle of 50%
	HAL_GPIO_WritePin(LVL_SFTR_OE_1_GPIO_Port, LVL_SFTR_OE_1_Pin, GPIO_PIN_SET);
}

void configure_braking()
{
	braking_stepper.is_active = 0;
	braking_stepper.is_exec_started = 0;
	braking_stepper.direction = IDLE;
	braking_stepper.MAX_ANGLE = 60; // Degrees
	braking_stepper.current_angle = 0;
	braking_stepper.STEPPER_OFFSET = 1.8;

	htim2.Instance->CCR1 = 392; // For duty cycle of 50%
	// HAL_GPIO_WritePin(GPIOB, LVL_SFTR_OE_2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LVL_SFTR_OE_1_GPIO_Port, LVL_SFTR_OE_1_Pin, GPIO_PIN_SET); // Temp steering
}

void start(const stepper_type stepper)
{
	switch (stepper)
	{
	case STEERING:
		HAL_GPIO_WritePin(GPIOC, STPR_EN_1_Pin, GPIO_PIN_RESET);
		steering_stepper.is_active = 1;
		HAL_GPIO_WritePin(DEBUG_5_GPIO_Port, DEBUG_5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(DEBUG_6_GPIO_Port, DEBUG_6_Pin, GPIO_PIN_RESET);
		break;
	case BRAKING:
		// HAL_GPIO_WritePin(GPIOB, STPR_EN_2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, STPR_EN_1_Pin, GPIO_PIN_RESET); // Temp Steering
		braking_stepper.is_active = 1;
		HAL_GPIO_WritePin(DEBUG_5_GPIO_Port, DEBUG_5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(DEBUG_6_GPIO_Port, DEBUG_6_Pin, GPIO_PIN_RESET);
			break;
		default:
			break;
	}
}

void pause(const stepper_type stepper)
{
	switch (stepper)
	{
	case STEERING:
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
		steering_stepper.is_exec_started = 0;
		HAL_GPIO_WritePin(DEBUG_6_GPIO_Port, DEBUG_6_Pin, GPIO_PIN_SET);
		break;
	case BRAKING:
		// HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1); // Temp Steering
		braking_stepper.is_exec_started = 0;
		HAL_GPIO_WritePin(DEBUG_6_GPIO_Port, DEBUG_6_GPIO_Port, GPIO_PIN_SET);
		break;
	default:
		break;
	}
}

void stop(const stepper_type stepper)
{
	switch (stepper)
	{
	case STEERING:
		HAL_GPIO_WritePin(GPIOC, STPR_EN_1_Pin, GPIO_PIN_SET);
		// HAL_GPIO_WritePin(LVL_SFTR_OE_1_GPIO_Port, LVL_SFTR_OE_1_Pin, GPIO_PIN_RESET);
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
		steering_stepper.is_active = 0;
		steering_stepper.direction = IDLE;
		steering_stepper.is_exec_started = 0;
		HAL_GPIO_WritePin(DEBUG_5_GPIO_Port, DEBUG_5_Pin, GPIO_PIN_RESET);
		break;
	case BRAKING:
		// HAL_GPIO_WritePin(GPIOB, STPR_EN_2_Pin, GPIO_PIN_SET);
		// HAL_GPIO_WritePin(GPIOB, LVL_SFTR_OE_2_Pin, GPIO_PIN_RESET);
		// HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
		HAL_GPIO_WritePin(STPR_EN_1_GPIO_Port, STPR_EN_1_Pin, GPIO_PIN_SET); // Temp Steering
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);			   // Temp Steering
		braking_stepper.is_active = 0;
		braking_stepper.direction = IDLE;
		braking_stepper.is_exec_started = 0;
		HAL_GPIO_WritePin(DEBUG_5_GPIO_Port, DEBUG_5_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(DEBUG_6_GPIO_Port, DEBUG_6_Pin, GPIO_PIN_RESET);
		break;
	default:
		break;
	}
}

void em_stop()
{
	// For the break, the stop has a different meaning
	float error = braking_stepper.desired_angle - braking_stepper.current_angle;
	uint8_t direction = CW;

	if (braking_stepper.direction != IDLE)
	{
		if (braking_stepper.direction != direction)
		{
			braking_stepper.direction = direction;
			// HAL_GPIO_WritePin(GPIOB, STPR_DIR_2_Pin, direction);
			HAL_GPIO_WritePin(GPIOB, STPR_DIR_1_Pin, direction); // Temp Steering
		}

		if (fabsf(error) > braking_stepper.STEPPER_OFFSET)
		{
			// Start execution
			if (!braking_stepper.is_exec_started)
			{
				// HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
				HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // Temp Steering
				braking_stepper.is_exec_started = 1;
			}
		}
		else
			pause(BRAKING);
	}
	else
		pause(BRAKING);
}


stepper_direction unsafe_direction;
void steer(uint8_t direction){
	//steering_stepper.mode = CONTROLLER;
	float SAT_ANGLE;

	if(steering_stepper.is_active)
	{
		if(direction != IDLE)
		{
			if(direction != steering_stepper.direction)
			{
				HAL_GPIO_WritePin(GPIOC, STPR_DIR_1_Pin, direction);
			}

			// Check max steering angle is not exceded
			if(direction == CCW)
				SAT_ANGLE = steering_stepper.MAX_ANGLE;
			else
				SAT_ANGLE = steering_stepper.MIN_ANGLE;


			if(fabsf(steering_stepper.current_angle) < SAT_ANGLE)
			{
				if(!steering_stepper.is_exec_started)
				{
					HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
					steering_stepper.is_exec_started = 1;
				}
				steering_stepper.direction = direction;
			} else {
				unsafe_direction = steering_stepper.direction;

				// Only can continue if direction is changed
				if(direction != unsafe_direction)
				{
					HAL_GPIO_WritePin(GPIOC, STPR_DIR_1_Pin, direction);
					if(!steering_stepper.is_exec_started)
					{
						HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
						steering_stepper.is_exec_started = 1;
					}
				} else pause(STEERING);
			}
		} else pause(STEERING);
	} else stop(STEERING);
}

uint32_t obtain_current_angle(){
	uint32_t angle = 0;
	float f_angle = -steering_stepper.current_angle/STEER_RATIO >= 0.0f ?
			steering_stepper.current_angle/steering_stepper.MAX_ANGLE:
			steering_stepper.current_angle/steering_stepper.MIN_ANGLE;
	angle =  serialize_float(f_angle);
	return angle;
}


float get_current_pos(){
	return steering_stepper.current_angle >= 0.0f ?
			-steering_stepper.current_angle/steering_stepper.MIN_ANGLE:
			-steering_stepper.current_angle/steering_stepper.MAX_ANGLE;
}


void set_setpoint(const stepper_type stepper, float setpoint){
	uint8_t direction = IDLE;
	float error = 0;

	switch(stepper)
	{
		case STEERING:
			// setpoint: [-1,1]
			if(setpoint >= 0)
				steering_stepper.desired_angle = -setpoint*steering_stepper.MAX_ANGLE;  // To account for gear counter rotation
			else
				steering_stepper.desired_angle = -setpoint*steering_stepper.MIN_ANGLE;  // To account for gear counter rotation

			error = steering_stepper.desired_angle - steering_stepper.current_angle;

			direction = fabsf(error) < steering_stepper.STEPPER_OFFSET ? IDLE:error > 0? CW:CCW;

			steer_by_setpoint(direction, error);
			break;
		case BRAKING:
			// setpoint: [0,1]

			braking_stepper.desired_angle = setpoint*braking_stepper.MAX_ANGLE;
			error = braking_stepper.desired_angle - braking_stepper.current_angle;

			direction = fabsf(error) < braking_stepper.STEPPER_OFFSET ? IDLE:error > 0? CW:CCW;

			brake_by_setpoint(direction, error);
			break;
	}
}

void steer_by_setpoint(uint8_t direction, float error)
{
	if(steering_stepper.is_active)
	{
		if(direction != IDLE)
		{
			if(steering_stepper.direction != direction)
			{
				steering_stepper.direction = direction;
				HAL_GPIO_WritePin(GPIOC, STPR_DIR_1_Pin, direction);
			}
			if(fabsf(error) > steering_stepper.STEPPER_OFFSET)
			{
				// Start execution
				if(!steering_stepper.is_exec_started)
				{
					HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
					steering_stepper.is_exec_started = 1;
				}
			} else pause(STEERING);
		} else pause(STEERING);
	} else stop(STEERING);
}

void brake_by_setpoint(uint8_t direction, float error)
{

	if (braking_stepper.is_active)
	{
		if (direction != IDLE)
		{
			if (braking_stepper.direction != direction)
			{
				braking_stepper.direction = direction;
				HAL_GPIO_WritePin(STPR_DIR_1_GPIO_Port, STPR_DIR_1_Pin, direction); // Temp Steering
				// HAL_GPIO_WritePin(GPIOC, STPR_DIR_1_Pin, direction);
			}

			if (fabsf(error) > braking_stepper.STEPPER_OFFSET)
			{
				// Start execution
				if (!braking_stepper.is_exec_started)
				{
					HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // Temp Steering
					// HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
					braking_stepper.is_exec_started = 1;
				}
			}
			else
				pause(BRAKING);
		}
		else
			pause(BRAKING);
	}
	else
		stop(BRAKING);
}

void update_stepper_pos(const stepper_type stepper)
{
	switch (stepper)
	{
	case STEERING:
		steering_stepper.current_angle = -ifm_encoder.absolute_angle * STEER_RATIO; // - To account for gear counter rotation
		break;
	case BRAKING:
		braking_stepper.current_angle = briter_encoder.absolute_angle * (PEDAL_LENGTH / PULLEY_RADIUS); // REQUIRED RELATIONSHIP FROM PULLEY TO BRAKE ANGLE
		break;
	}
}
/*
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2) {
	//steering_stepper.current_step++;
	//HAL_GPIO_TogglePin(GPIOA, DEBUG_2_Pin);
  }
}
*/
