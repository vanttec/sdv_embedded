/*
 * stepper.h
 *
 *  Created on: Mar 7, 2023
 *      Author: saveasmtz
 */

#ifndef INC_STEPPER_H
#define INC_STEPPER_H

#include "main.h"
#include "cmsis_os.h"
#include "stdlib.h"
#include <math.h>
#include "encoder.h"

extern TIM_HandleTypeDef htim2;

extern volatile encoder ifm_encoder;
extern volatile encoder britter_encoder;

typedef enum {
	STEERING,
	BRAKING
} stepper_type;

typedef enum {
	CONTROLLER,
	AUTONOMOUS
} stepper_mode;

typedef enum {
	CW,		// Clock-Wise
	CCW,	// Counter Clock-Wise
	IDLE	// No direction
} stepper_direction;

typedef struct {
	uint8_t is_active;			//0: inactive, 1: active
	uint8_t is_exec_started;
	stepper_mode mode;
	stepper_direction direction;

	float MAX_ANGLE;			// Degrees
	float current_angle; 		// Degrees

	/*
	float STEP_ANGLE;				// Degrees
	uint16_t STEPS_REV;				// Steps per revolution
	uint16_t MAX_STEPS;

	float desired_angle;		// Degrees
	uint16_t req_steps;			// Required steps to reach desired angle
	uint16_t current_step;
	*/
} stepper;

void configure_steppers();
void start();
void stop();
void set_direction(const stepper_type stepper, uint8_t direction);
//void set_setpoint(const stepper_type stepper, uint16_t setpoint, int8_t direction);

void steer();
void brake();

//void stepping_by_pwm(stepper *stpr, stepper_type id);
//void stepping_by_steps(stepper *stpr, stepper_type id);

#endif /* INC_STEPPER_H */
